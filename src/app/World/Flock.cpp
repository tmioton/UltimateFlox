#include "pch.hpp"
#include "Flock.hpp"


Flock::Flock(size_t flock_size, Vector b) :
    m_count(flock_size), m_flock(flock_size), m_bounds(b),
    m_treeBounds(m_bounds), m_tree(m_treeBounds)
{
    // Set up boid starting locations
    Boid *writable = m_flock.write();

    float d = 7.5f;
    float angle = 0.0f;
    for (ptrdiff_t i = 0; i < m_count; i++) {
        Boid &boid = writable[i];
        //auto angle = static_cast<float>(i) * tauOverSize;
        float radius = glm::sqrt(static_cast<float>(i + 1));
        angle += glm::asin(1.0f / radius);
        Vector offsets{glm::cos(angle) * radius * d, glm::sin(angle) * radius * d};
        boid.position = offsets;
        boid.velocity = magnitude(10.0f * offsets + angle, Boid::maxSpeed);
    }

    m_flock.flip();
}

void Flock::update(float dt) {
    Boid *write = m_flock.write();
    Boid const *read = m_flock.read();
    const int count = static_cast<int>(m_flock.count());
    m_tree.clear();
    m_tree.bounds(m_treeBounds);
    Vector xBound {m_treeBounds.center.x - m_treeBounds.size.x, m_treeBounds.center.x + m_treeBounds.size.x};
    Vector yBound {m_treeBounds.center.y - m_treeBounds.size.y, m_treeBounds.center.y + m_treeBounds.size.y};

    for (int i = 0; i < count; ++i) {
        m_tree.insert(i, read[i].position);
    }

    const int threadCount = 4;
    std::thread threads[threadCount];
    int boidsPerThread = count / threadCount;
    int leftover = count - boidsPerThread * threadCount;
    int nextStart = 0;
    for (int i = 0; i < threadCount; ++i) {
        int actualCount = boidsPerThread;
        if (!i) { actualCount += leftover; }
        threads[i] = std::thread(
            thread_update, ThreadState{
                &m_tree, m_bounds,
                write, read,
                &m_results[i],
                nextStart,
                actualCount,
                dt
            }
        );

        nextStart += actualCount;
    }

    for (auto & update_thread : threads) {
        update_thread.join();
    }

    // Separate loop for thread-safety. Does this slow the program down?
    for (int i = 0; i < count; ++i) {
        Boid const &current = write[i];

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

    // Push changes to flock
    m_flock.flip();
}

Boid const *Flock::boids() {
    return m_flock.read();
}

Boidtree const &Flock::tree() const {
    return m_tree;
}

void Flock::resize(size_t size) {
    if (size > m_count) {

        Boid *write = m_flock.write();
        const float tauOverSize = glm::two_pi<float>() / static_cast<float>(m_count);
        for (size_t i = m_count; i < size; i++) {
            Boid &boid = write[i];
            auto angle = static_cast<float>(i) * tauOverSize;
            Vector offsets{cosf(angle), sinf(angle)};
            boid.position = 50.0f * offsets;
            boid.velocity = magnitude(10.0f * offsets + angle, Boid::maxSpeed);
        }
    }

    m_count = size;
}

void Flock::thread_update(Flock::ThreadState state) {
    Boidtree* tree = state.tree;
    Rectangle const& bounds = state.bounds;
    Boid *write = state.write;
    Boid const *read = state.read;
    Boidtree::ResultVector& results = *state.search;
    int start = state.start;
    int count = state.count;
    float delta = state.delta;

    const float disruptiveRadius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    const float cohesiveRadius = Boid::cohesiveRadius * Boid::cohesiveRadius;
    //Boidtree::ResultVector& results = *;

    Rectangle centerBound{bounds * 0.75f};
    Rectangle hardBound{bounds * 0.90f};

    Rectangle boidBound{Vector{Boid::scale}};
    Rectangle searchBound{Vector{Boid::cohesiveRadius}};
    for (int i = start; i < start + count; ++i) {
        Boid &current = write[i];

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
        tree->search(searchBound, results);  // Thread-unsafe
        for (auto k : results) {
            if (i == k) {
                continue;
            }

            const Boid &other = read[k];
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

Flock::ThreadState::ThreadState(Boidtree *t, Rectangle b, Boid *w, const Boid *r, Boidtree::ResultVector* res, int s, int c, float d):
    tree(t), bounds(b), write(w), read(r), search(res), start(s), count(c), delta(d)
{}

