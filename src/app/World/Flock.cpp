#include "pch.hpp"
#include "Flock.hpp"


Flock::Flock(size_t flock_size) :
    m_count(flock_size), m_flock(flock_size) {
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

void Flock::update(Algorithm *algorithm, float dt) {
    // Run the given algorithm
    algorithm->update(m_flock, dt);

    // Push changes to flock
    m_flock.flip();
}

Boid const *Flock::boids() {
    return m_flock.read();
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
