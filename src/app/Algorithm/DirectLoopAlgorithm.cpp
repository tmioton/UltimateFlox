#include "DirectLoopAlgorithm.hpp"

DirectLoopAlgorithm::DirectLoopAlgorithm(Vector bounds) : m_bounds(bounds) {}

void DirectLoopAlgorithm::update(DoubleBuffer<Boid> &boids, float delta) {
    constexpr float disruptiveRadius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    constexpr float cohesiveRadius = Boid::cohesiveRadius * Boid::cohesiveRadius;

    Boid *write = boids.write();
    Boid const *read = boids.read();
    const size_t count = boids.count();

    for (size_t i = 0; i < count; i++) {
        Boid &currentBoid = write[i];

        Vector centerSteer{0.0f, 0.0f};
        // Precursor to Rectangle class
        if (currentBoid.position.x - Boid::scale <= -m_bounds.x
            || currentBoid.position.x + Boid::scale >= m_bounds.x
            || currentBoid.position.y - Boid::scale <= -m_bounds.y
            || currentBoid.position.y + Boid::scale >= m_bounds.y
            ) {
            centerSteer -= currentBoid.position;
            centerSteer = currentBoid.steer(centerSteer);
        }

        // Desire to move at full speed
        // This is redundant when other forces are acting upon the boid, but that's not always the case.
        Vector fullSpeed = currentBoid.velocity;
        fullSpeed = currentBoid.steer(fullSpeed);

        Vector separation{0.0f, 0.0f};  // Desire to separate from flockmates
        Vector alignment{0.0f, 0.0f};  // Desire to align with the direction of other flockmates
        Vector cohesion{0.0f, 0.0f};  // Desire to shrink the distance between self and flockmates
        size_t cohesiveTotal = 0;
        size_t disruptiveTotal = 0;

        for (size_t j = 0; j < count; j++) {
            if (i == j) {
                continue;
            }

            const Boid &otherBoid = read[j];

            const float d2 = glm::distance2(currentBoid.position, otherBoid.position);
            if (d2 < disruptiveRadius && d2 > 0.0f) {
                Vector diff = currentBoid.position - otherBoid.position;
                separation += diff / d2;
                disruptiveTotal++;
            }

            if (d2 < cohesiveRadius) {
                alignment += otherBoid.velocity;
                cohesion += otherBoid.position;
                cohesiveTotal++;
            }
        }

        if (disruptiveTotal > 0) {
            separation /= static_cast<float>(disruptiveTotal);
            separation = currentBoid.steer(separation);
        }

        if (cohesiveTotal > 0) {
            const float countFactor = 1.0f / static_cast<float>(cohesiveTotal);
            alignment *= countFactor;
            alignment = currentBoid.steer(alignment);

            cohesion *= countFactor;
            cohesion -= currentBoid.position;
            cohesion = currentBoid.steer(cohesion);
        }

        Vector acceleration{0.0f, 0.0f};
        acceleration += centerSteer * Boid::primadonnaWeight;
        acceleration += fullSpeed * Boid::speedWeight;
        acceleration += separation * Boid::separationWeight;
        acceleration += alignment * Boid::alignmentWeight;
        acceleration += cohesion * Boid::cohesionWeight;

        acceleration = magnitude(acceleration, Boid::maxForce);

        currentBoid.velocity += acceleration;
        currentBoid.position += currentBoid.velocity * delta;
    }
}
