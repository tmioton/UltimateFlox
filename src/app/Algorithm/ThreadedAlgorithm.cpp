#include "pch.hpp"
#include "ThreadedAlgorithm.hpp"
#include "Structures/RawArray.hpp"


constexpr ptrdiff_t BOID_GROUP = 8;


// ****** ThreadedAlgorithm ******
ThreadedAlgorithm::ThreadedAlgorithm(Vector b) : m_bounds(b), m_treeBounds(m_bounds), m_tree(m_treeBounds) {
    for (auto &m_result: m_results) {
        m_result.reserve(128);
    }

    m_pool.init();
}

ThreadedAlgorithm::~ThreadedAlgorithm() {
    m_pool.shutdown();
}

void ThreadedAlgorithm::update(DoubleBuffer<Boid> &boids, const float delta) {
    const auto count = static_cast<ptrdiff_t>(boids.count());
    if (count == 0) { return; }

    const Boid *read = boids.read();
    Boid *write = boids.write();

    // Insert the boids into the quadtree
    populate_tree(boids.read(), count);

    // Distribute the calculation work evenly among the available threads.
    distribute_work(read, write, count, delta);

    // Recalculate the bounds of the quadtree to keep the birds inside.
    recalculate_bounds(boids.write(), count);
}

void ThreadedAlgorithm::populate_tree(const Boid *read, const ptrdiff_t count) {
    m_tree.clear();
    m_tree.bounds = m_treeBounds;
    for (Boid const &boid: RawArray(read, count)) {
        m_tree.insert(&boid, boid.position);
    }
}

void ThreadedAlgorithm::distribute_work(const Boid *read, Boid *write, const ptrdiff_t count, const float delta) {
    // Maybe compute these values only when flock size changes?
    // Compiler should see div/mod and combine operations.
    // Number of groups of boids.
    const ptrdiff_t boid_groups = count / BOID_GROUP;

    // Number of boids outside a group. Always less than boid group size. Fine. I'll process them myself.
    const ptrdiff_t leftover_boids = count % BOID_GROUP;

    // Number of boid groups per thread. 0 if less than ThreadCount groups.
    const ptrdiff_t boid_groups_per_thread = boid_groups / ThreadCount;

    // Number of groups not evenly distributed across the threads. Evenly distribute them across the threads.
    ptrdiff_t leftover_groups = boid_groups % ThreadCount;
    const ptrdiff_t boids_per_thread = BOID_GROUP * boid_groups_per_thread;

    //std::cout << '\n'
    //          << boid_groups << " groups.\n"
    //          << boid_groups_per_thread << " groups per thread.\n"
    //          << leftover_groups << " leftover groups.\n"
    //          << leftover_boids << " leftover boids.\n";

    // Leftover boids have to be added to the last thread.
    // The last thread only takes a leftover group if there are no leftover boids.
    // If the first thread took two birds extra, each of the other threads' processing would start offset by two.

    const bool primary_thread_needs_work = !(boids_per_thread || leftover_boids);

    // See if we need to take a group here.
    const ptrdiff_t final_start {
        primary_thread_needs_work ?
        BOID_GROUP * (boid_groups_per_thread * (ThreadCount - 1) + leftover_groups - (leftover_groups > 0)) :
        BOID_GROUP * (boid_groups_per_thread * (ThreadCount - 1) + leftover_groups)
    };

    // Take the group here.
    const ptrdiff_t final_count {
        primary_thread_needs_work ?
        boids_per_thread + leftover_boids + (leftover_groups-- > 0) * BOID_GROUP :
        boids_per_thread + leftover_boids
    };

    ThreadFutures futures {};
    if (boid_groups_per_thread > 0 || leftover_groups > 0) {
        ptrdiff_t next_start = 0;
        for (int i = 0; i < ThreadCount - 1; ++i) {
            if (boid_groups_per_thread == 0 && leftover_groups <= 0) { break; }
            futures[i] = m_pool.submit(
                [](ThreadWork thread_work) { thread_work(); },
                ThreadWork {
                    this, i, delta,
                    read, write,
                    boids_per_thread + (leftover_groups > 0) * BOID_GROUP,  // I might take a group below.
                    next_start
                }
            );

            next_start += boids_per_thread + (leftover_groups-- > 0) * BOID_GROUP;  // Take a group here.
        }
    }

    //Do my work.
    ThreadWork {this, ThreadCount - 1, delta, read, write, final_count, final_start}();

    // Wait for the others to finish their work.
    for (auto &update_future: futures) {
        if (update_future.valid()) {
            update_future.get();
        }
    }
}

void ThreadedAlgorithm::recalculate_bounds(Boid *write, const ptrdiff_t count) {
    // Separate loop for thread-safety. Does this slow the program down?
    // Maybe have an array where the results of this test from each thread are stored and join them after.
    Vector x_bound {m_treeBounds.center.x - m_treeBounds.size.x, m_treeBounds.center.x + m_treeBounds.size.x};
    Vector y_bound {m_treeBounds.center.y - m_treeBounds.size.y, m_treeBounds.center.y + m_treeBounds.size.y};
    for (auto const &current: RawArray(write, count)) {
        if (current.position.x < x_bound.r) {
            x_bound.r = current.position.x;
        } else if (current.position.x > x_bound.g) {
            x_bound.g = current.position.x;
        }

        if (current.position.y < y_bound.r) {
            y_bound.r = current.position.y;
        } else if (current.position.y > y_bound.g) {
            y_bound.g = current.position.y;
        }
    }

    float x_bound_center = (x_bound.r + x_bound.g) * 0.5f;
    float y_bound_center = (y_bound.r + y_bound.g) * 0.5f;

    Rectangle newBounds {
        {x_bound_center,             y_bound_center},
        {x_bound.g - x_bound_center, y_bound.g - y_bound_center}
    };

    m_treeBounds = newBounds;
}

Boidtree const &ThreadedAlgorithm::tree() const {
    return m_tree;
}
