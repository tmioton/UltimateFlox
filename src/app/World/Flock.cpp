#include "pch.hpp"
#include "Flock.hpp"


Flock::Flock(size_t flock_size, int width, int height) :
    flockSize(flock_size), m_bounds(calculateBounds(static_cast<float>(width) / static_cast<float>(height)))
{
    // Set up boid starting locations
    const float tauOverSize = glm::two_pi<float>() / static_cast<float>(flockSize);
    for (ptrdiff_t i = 0; i < flockSize; i++) {
        Boid &boid = m_primaryFlock[i];
        auto angle = static_cast<float>(i) * tauOverSize;
        Vector offsets {cosf(angle), sinf(angle)};
        boid.position = 50.0f * offsets;
        boid.velocity = magnitude(10.0f * offsets + angle, Boid::maxSpeed);
    }
}

void Flock::update(float dt) {
    constexpr float disruptiveRadius = Boid::disruptiveRadius * Boid::disruptiveRadius;
    constexpr float cohesiveRadius = Boid::cohesiveRadius * Boid::cohesiveRadius;

    for (size_t i = 0; i < flockSize; i++) {
        m_secondaryFlock[i] = m_primaryFlock[i];
    }

    for (size_t i = 0; i < flockSize; i++) {
        Boid &currentBoid = m_secondaryFlock[i];

        Vector centerSteer {0.0f, 0.0f};
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

        Vector separation {0.0f, 0.0f};  // Desire to separate from flockmates
        Vector alignment {0.0f, 0.0f};  // Desire to align with the direction of other flockmates
        Vector cohesion {0.0f, 0.0f};  // Desire to shrink the distance between self and flockmates
        size_t cohesiveTotal = 0;
        size_t disruptiveTotal = 0;

        for(size_t j = 0; j < flockSize; j++) {
            if (i == j) {
                continue;
            }

            const Boid& otherBoid = m_primaryFlock[j];

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

        Vector acceleration {0.0f, 0.0f};
        acceleration += centerSteer * Boid::primadonnaWeight;
        acceleration += fullSpeed * Boid::speedWeight;
        acceleration += separation * Boid::separationWeight;
        acceleration += alignment * Boid::alignmentWeight;
        acceleration += cohesion * Boid::cohesionWeight;

        acceleration = magnitude(acceleration, Boid::maxForce);

        currentBoid.velocity += acceleration;
        currentBoid.position += currentBoid.velocity * dt;
    }

    // Move array pointers
    auto temp = std::move(m_primaryFlock);
    m_primaryFlock = std::move(m_secondaryFlock);
    m_secondaryFlock = std::move(temp);
}

BoidArray &Flock::boids() {
    return m_primaryFlock;
}
