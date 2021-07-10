#include "pch.hpp"
#include "Flock.hpp"


Flock::Flock(size_t flock_size, float aspect) :
    flockSize(flock_size), m_bounds {
            aspect >= 1.0f ? worldBound * aspect : worldBound,
            aspect < 1.0f ? worldBound * aspect : worldBound
        }, m_renderer(flock_size, m_bounds)
{
    // Set up boid starting locations
    const float tauOverSize = glm::two_pi<float>() / static_cast<float>(flockSize);
    for (ptrdiff_t i = 0; i < flockSize; i++) {
        Boid &boid = m_primaryFlock[i];
        auto angle = static_cast<float>(i) * tauOverSize;
        Vector offsets {cosf(angle), sinf(angle)};
        boid.position = 50.0f * offsets;
        boid.velocity = magnitude(10.0f * offsets + angle, Boid::maxSpeed);
        boid.acceleration.y = -1.0f;
    }
}

void Flock::update(float dt) {
    for (size_t i = 0; i < flockSize; i++) {
        m_secondaryFlock[i] = m_primaryFlock[i];

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

            const float d = glm::fastDistance(currentBoid.position, otherBoid.position);
            if (d < Boid::disruptiveRadius && d > 0.0f) {
                Vector diff = currentBoid.position - otherBoid.position;
                separation += diff / (d * d);
                disruptiveTotal++;
            }

            if (d < Boid::cohesiveRadius) {
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

        currentBoid.acceleration += centerSteer * Boid::primadonnaWeight;
        currentBoid.acceleration += fullSpeed * Boid::speedWeight;
        currentBoid.acceleration += separation * Boid::separationWeight;
        currentBoid.acceleration += alignment * Boid::alignmentWeight;
        currentBoid.acceleration += cohesion * Boid::cohesionWeight;

        currentBoid.acceleration = magnitude(currentBoid.acceleration, Boid::maxForce);

        currentBoid.velocity += currentBoid.acceleration;
        currentBoid.position += currentBoid.velocity * dt;
        currentBoid.acceleration *= 0.0f;
    }

    // Update boid rendering
    m_renderer.update(m_secondaryFlock);

    // Move array pointers
    auto temp = std::move(m_primaryFlock);
    m_primaryFlock = std::move(m_secondaryFlock);
    m_secondaryFlock = std::move(temp);

    //std::cout << "Boid 1 position: " << m_primaryFlock[0].position << std::endl;
}

void Flock::draw() {
    m_renderer.draw();
}

detail::BoidRenderer &Flock::renderer() {
    return m_renderer;
}
