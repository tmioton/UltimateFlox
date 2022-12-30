#include "pch.hpp"
#include "ThreadedAlgorithm.hpp"
#include "Structures/RawArray.hpp"

ThreadedAlgorithm::ThreadedAlgorithm(Vector bounds) : QuadtreeAlgorithm(bounds) {}

void ThreadedAlgorithm::update(DoubleBuffer<Boid> &boids, float delta) {
    Boid *write = boids.write();
    Boid const *read = boids.read();
    const int count = static_cast<int>(boids.count());
    m_tree.clear();
    m_tree.bounds(m_treeBounds);
    Vector xBound {m_treeBounds.center.x - m_treeBounds.size.x, m_treeBounds.center.x + m_treeBounds.size.x};
    Vector yBound {m_treeBounds.center.y - m_treeBounds.size.y, m_treeBounds.center.y + m_treeBounds.size.y};

    for (Boid const& boid : RawArray(read, count)) {
        m_tree.insert(boid, boid.position);
    }

    std::thread threads[ThreadCount];
    int boidsPerThread = count / ThreadCount;
    int leftover = count - boidsPerThread * ThreadCount;
    int nextStart = 0;
    for (int i = 0; i < ThreadCount; ++i) {
        int actualCount = boidsPerThread;
        if (!i) { actualCount += leftover; }
        threads[i] = std::thread(
            thread_update, ThreadState {
                &m_tree, m_bounds,
                write + nextStart,
                &m_results[i],
                actualCount,
                delta
            }
        );

        nextStart += actualCount;
    }

    for (auto & update_thread : threads) {
        update_thread.join();
    }

    // Separate loop for thread-safety. Does this slow the program down?
    for (auto const& current: RawArray(write, count)) {
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
        {xBoundCenter, yBoundCenter},
        {xBound.g - xBoundCenter, yBound.g - yBoundCenter}
    };

    m_treeBounds = newBounds;
}

void ThreadedAlgorithm::thread_update(ThreadedAlgorithm::ThreadState state) {
    Boidtree* tree = state.tree;
    Rectangle const& bounds = state.bounds;
    Boid *write = state.write;
    Boidtree::ResultVector &results = *state.search;
    int count = state.count;
    float delta = state.delta;

    const float disruptiveRadius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    const float cohesiveRadius = Boid::cohesiveRadius * Boid::cohesiveRadius;

    Rectangle centerBound{bounds * 0.75f};
    Rectangle hardBound{bounds * 0.90f};

    Rectangle boidBound{Vector{Boid::scale}};
    Rectangle searchBound{Vector{Boid::cohesiveRadius}};
    for (Boid &current : RawArray(write, count)) {
        boidBound.center = current.position;  // Currently thread-unsafe, easy fix
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

        Vector separation{0.0f, 0.0f};
        Vector alignment{0.0f, 0.0f};
        Vector cohesion{0.0f, 0.0f};
        size_t cohesiveTotal = 0;
        size_t disruptiveTotal = 0;

        results.clear();
        tree->search(searchBound, results);
        for (Boid const &other : results) {

            // It is theoretically possible that two boids share the same position.
            // Two or more birds in this situation using this if statement will then receive exactly the same
            //   forces acting upon it, ensuring they remain in the situation. Alleviated by random walk?
            if (current.position == other.position) {
                continue;
            }

            const float d2 = glm::distance2(current.position, other.position);
            if (d2 < disruptiveRadius && d2 > 0.0f) {
                Vector diff = current.position - other.position;
                separation += diff / d2;
                disruptiveTotal++;
            }

            if (d2 < cohesiveRadius) {
                alignment += other.velocity;
                cohesion += other.position;
                cohesiveTotal++;
            }
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

        Vector acceleration{0.0f, 0.0f};
        acceleration += centerSteer * centerSteerWeight;
        acceleration += fullSpeed * Boid::speedWeight;
        acceleration += separation * Boid::separationWeight;
        acceleration += alignment * Boid::alignmentWeight;
        acceleration += cohesion * Boid::cohesionWeight;

        acceleration = magnitude(acceleration, Boid::maxForce);

        current.velocity += acceleration;
        current.position += current.velocity * delta;
    }
}

ThreadedAlgorithm::ThreadState::ThreadState(Boidtree *t, Rectangle b, Boid *w, Boidtree::ResultVector *res, int c, float d):
    tree(t), bounds(b), write(w), search(res), count(c), delta(d)
{}
