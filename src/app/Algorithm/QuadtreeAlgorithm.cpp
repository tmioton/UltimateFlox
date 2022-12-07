#include "QuadtreeAlgorithm.hpp"
#include "Structures/RawArray.hpp"

QuadtreeAlgorithm::QuadtreeAlgorithm(Vector b) : m_bounds(b), m_treeBounds(m_bounds), m_tree(m_treeBounds) {}

void QuadtreeAlgorithm::update(DoubleBuffer<Boid> &boids, float delta) {
    const float disruptiveRadius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    const float cohesiveRadius = Boid::cohesiveRadius * Boid::cohesiveRadius;

    Boid *write = boids.write();
    Boid const *read = boids.read();
    const int count = static_cast<int>(boids.count());

    m_tree.clear();
    m_tree.bounds(m_treeBounds);
    Vector xBound {m_treeBounds.center.x - m_treeBounds.size.x, m_treeBounds.center.x + m_treeBounds.size.x};
    Vector yBound {m_treeBounds.center.y - m_treeBounds.size.y, m_treeBounds.center.y + m_treeBounds.size.y};

    for (int i = 0; i < count; i++) {
        m_tree.insert(i, read[i].position);
    }

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

        m_results.clear();
        m_tree.search(searchBound, m_results);
        for (int k : m_results) {
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

    // Separate loop. Does this slow the program down?
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

Boidtree const &QuadtreeAlgorithm::tree() const {
    return m_tree;
}
