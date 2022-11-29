#include "QuadtreeAlgorithm.hpp"

QuadtreeAlgorithm::QuadtreeAlgorithm(Vector b) : m_bounds(b), m_tree(m_bounds) {}

void QuadtreeAlgorithm::update(DoubleBuffer<Boid> &boids, float delta) {
    constexpr float disruptiveRadius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    constexpr float cohesiveRadius = Boid::cohesiveRadius * Boid::cohesiveRadius;

    Boid *write = boids.write();
    Boid const *read = boids.read();
    const int count = static_cast<int>(boids.count());

    m_failed.clear();
    m_tree.clear();
    for (int i = 0; i < count; i++) {
        if (!m_tree.insert(i, read[i].position)) {
            m_failed.push_back(i);
        }
    }

    const int failedCount = static_cast<int>(m_failed.size());

    Rectangle centerBound{m_bounds * 0.75f};
    Rectangle hardBound{m_bounds * 2.0f};

    Rectangle boidBound{Vector{Boid::scale}};
    Rectangle searchBound{Vector{Boid::cohesiveRadius}};
    for (int i = 0; i < count; ++i) {
        Boid &current = write[i];

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

        Vector separation{0.0f, 0.0f};
        Vector alignment{0.0f, 0.0f};
        Vector cohesion{0.0f, 0.0f};
        size_t cohesiveTotal = 0;
        size_t disruptiveTotal = 0;

        Boidtree::ResultVector result = m_tree.search(searchBound);
        const int resultCount = static_cast<int>(result.size());
        for (int j = 0; j < resultCount + failedCount; ++j) {
            if (inCenter && j >= resultCount) {
                break;
            }

            int k = j < resultCount ? result[j] : m_failed[j - resultCount];
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

Boidtree const &QuadtreeAlgorithm::tree() const {
    return m_tree;
}
