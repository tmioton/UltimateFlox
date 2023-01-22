#include "pch.hpp"
#include "ThreadedAlgorithm.hpp"
#include "Structures/RawArray.hpp"


// ****** ThreadedAlgorithm ******
ThreadedAlgorithm::ThreadedAlgorithm(Vector b) : m_bounds(b), m_treeBounds(m_bounds), m_tree(m_treeBounds) {
    for(auto & m_result : m_results) {
        m_result.reserve(128);
    }

    m_pool.init();
}

ThreadedAlgorithm::~ThreadedAlgorithm() {
    m_pool.shutdown();
}

void ThreadedAlgorithm::update(DoubleBuffer<Boid> &boids, float delta) {
    Boid *write = boids.write();
    Boid const *read = boids.read();
    const int count = static_cast<int>(boids.count());
    m_tree.clear();
    m_tree.bounds(m_treeBounds);
    Vector xBound{m_treeBounds.center.x - m_treeBounds.size.x, m_treeBounds.center.x + m_treeBounds.size.x};
    Vector yBound{m_treeBounds.center.y - m_treeBounds.size.y, m_treeBounds.center.y + m_treeBounds.size.y};

    for (Boid const &boid: RawArray(read, count)) {
    //for (int i = 0; i < count; ++i) {
    //    m_tree.insert(i, read[i].position);
    //    m_tree.insert(boid, boid.position);
        m_tree.insert(&boid, boid.position);
    }

    const ptrdiff_t boids_per_thread = count / ThreadCount;
    const ptrdiff_t leftover = count - boids_per_thread * ThreadCount;
    std::future<void> futures[ThreadCount - 1];
    ptrdiff_t next_start = boids_per_thread + leftover;
    for (int i = 1; i < ThreadCount; ++i) {
        futures[i - 1] = m_pool.submit(
            [](ThreadWork thread_work) { thread_work(); },
            ThreadWork{
                this, i, delta,
                //write + next_start,
                read, write,
                boids_per_thread,
                next_start
            }
        );

        next_start += boids_per_thread;
    }

    //Do my work.
    ThreadWork{this, 0, delta, read, write, boids_per_thread + leftover, 0}();
    //ThreadWork{this, 0, delta, write, boids_per_thread + leftover}();

    // Wait for the others to finish their work.
    for (auto &update_future: futures) {
        update_future.get();
    }

    // Separate loop for thread-safety. Does this slow the program down?
    for (auto const &current: RawArray(write, count)) {
        if (current.position.x < xBound.r) {
            xBound.r = current.position.x;
        } else if (current.position.x > xBound.g) {
            xBound.g = current.position.x;
        }

        if (current.position.y < yBound.r) {
            yBound.r = current.position.y;
        } else if (current.position.y > yBound.g) {
            yBound.g = current.position.y;
        }
    }

    float xBoundCenter = (xBound.r + xBound.g) * 0.5f;
    float yBoundCenter = (yBound.r + yBound.g) * 0.5f;

    Rectangle newBounds{
        {xBoundCenter,            yBoundCenter},
        {xBound.g - xBoundCenter, yBound.g - yBoundCenter}
    };

    m_treeBounds = newBounds;
}

void ThreadedAlgorithm::ThreadWork::operator()() const {
    const Boidtree &tree = algorithm->m_tree;
    const Rectangle bounds = algorithm->m_bounds;
    Boidtree::ResultVector &results = algorithm->m_results[id];
    const float disruptiveRadius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    const float cohesiveRadius = Boid::cohesiveRadius * Boid::cohesiveRadius;

    const Rectangle centerBound{bounds * 0.75f};
    const Rectangle hardBound{bounds * 0.90f};

    Rectangle boidBound{Vector{Boid::scale}};
    Rectangle searchBound{Vector{Boid::cohesiveRadius}};
    //for (Boid &current: RawArray(write + start, count)) {
    for (ptrdiff_t i = start; i < start + count; ++i) {
        Boid &current = write[i];
        const Boid &previous = read[i];

        boidBound.center = current.position;
        searchBound.center = current.position;
        Vector centerSteer{0.0f, 0.0f};

        const bool inCenter = centerBound.intersects(boidBound);
        float centerSteerWeight = Boid::primadonnaWeight;
        if (!inCenter) {
            if (!hardBound.intersects(boidBound)) {
                centerSteerWeight *= 2.0f;
            }

            centerSteer -= current.position;
            centerSteer = current.steer(centerSteer);
        }

        Vector fullSpeed = current.velocity;
        fullSpeed = current.steer(fullSpeed);

        results.clear();
        //tree.search(searchBound, results);
        tree.search(&previous, searchBound, results);
        //std::sort(
        //    results.begin(), results.end(),
        //    [&current](const Boid &a, const Boid &b) {
        //        return glm::distance2(current.position, a.position) < glm::distance2(current.position, b.position);
        //    }
        //);

        Vector separation{0.0f, 0.0f};
        Vector alignment{0.0f, 0.0f};
        Vector cohesion{0.0f, 0.0f};
        size_t cohesiveTotal = 0;
        size_t disruptiveTotal = 0;

        for (const Boid& other : results) {
        //for (const ptrdiff_t j: results) {
            // It is theoretically possible that two boids share the same position.
            // Two or more birds in this situation using this if statement will then receive exactly the same
            //   forces acting upon it, ensuring they remain in the situation. Alleviated by random walk?
            //if (current.position == other.position) {
            //if (i == j) {
            //    continue;
            //}

            //const Boid &other = read[j];
            const float d2 = glm::distance2(current.position, other.position);
            if (d2 < disruptiveRadius && d2 > 0.0f) {
                Vector diff = current.position - other.position;
                separation += diff / d2;
                disruptiveTotal++;
            }

            const auto in_cohesive_range = d2 < cohesiveRadius;
            const auto fin_cohesive_range = static_cast<float>(d2 < cohesiveRadius);
            alignment += fin_cohesive_range * other.velocity;
            cohesion += fin_cohesive_range * other.position;
            cohesiveTotal += in_cohesive_range;

            //if (d2 < cohesiveRadius) {
            //    alignment += other.velocity;
            //    cohesion += other.position;
            //    cohesiveTotal++;
            //}
        }

        if (disruptiveTotal > 0) {
            separation /= static_cast<float>(disruptiveTotal);
            separation = current.steer(separation);
        }

        if (cohesiveTotal > 0) {
            const float countFactor = 1.0f / static_cast<float>(cohesiveTotal);
            alignment *= countFactor;
            alignment = current.steer(alignment);

            cohesion *= countFactor;
            cohesion -= current.position;
            cohesion = current.steer(cohesion);
        }

        const Vector acceleration = magnitude(
            Vector{
                centerSteer * centerSteerWeight
                + fullSpeed * Boid::speedWeight
                + separation * Boid::separationWeight
                + alignment * Boid::alignmentWeight
                + cohesion * Boid::cohesionWeight
            },
            Boid::maxForce
        );

        current.velocity += acceleration;
        current.position += current.velocity * delta;
    }
}

ThreadedAlgorithm::ThreadWork::ThreadWork(ThreadedAlgorithm *a, int i, float d, const Boid *r, Boid *w, ptrdiff_t c, ptrdiff_t s) :
    algorithm(a), id(i), delta(d), read(r), write(w), count(c), start(s) {}
//ThreadedAlgorithm::ThreadWork::ThreadWork(ThreadedAlgorithm *a, int i, float d, Boid *w, ptrdiff_t c) :
//    algorithm(a), id(i), delta(d), write(w), count(c) {}

Boidtree const &ThreadedAlgorithm::tree() const {
    return m_tree;
}
