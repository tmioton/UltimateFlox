module;
#include "pch.hpp"
export module Flock;

import Algorithm;
import DoubleBuffer;
import Boid;


export class Flock {
    // without any steering, this number can go above 500,000 before dipping below 60fps
    size_t m_count;
    DoubleBuffer<Boid> m_flock;

public:
    explicit Flock(const size_t flock_size) : m_count(flock_size), m_flock(flock_size) {
        // Set up boid starting locations
        Boid *writable = m_flock.write();

        float angle = 0.0f;
        for (ptrdiff_t i = 0; i < m_count; i++) {
            constexpr float d = 7.5f;
            auto & [position, velocity] = writable[i];
            //auto angle = static_cast<float>(i) * tauOverSize;
            const float radius = glm::sqrt(static_cast<float>(i + 1));
            angle += glm::asin(1.0f / radius);
            Vector offsets{glm::cos(angle) * radius * d, glm::sin(angle) * radius * d};
            position = offsets;
            velocity = magnitude(10.0f * offsets + angle, Boid::maxSpeed);
        }

        m_flock.flip();
    }

    void update(Algorithm *algorithm, const float dt) {
        // Run the given algorithm
        algorithm->update(m_flock, dt);

        // Push changes to flock
        m_flock.flip();
    }

    [[nodiscard]] Boid const *boids() const {
        return m_flock.read();
    }

    [[nodiscard]] std::size_t count() const {
        return m_count;
    }

    void resize(const size_t size) {
        if (size > m_count) {

            Boid *write = m_flock.write();
            const float tauOverSize = glm::two_pi<float>() / static_cast<float>(m_count);
            for (size_t i = m_count; i < size; i++) {
                auto &[position, velocity] = write[i];
                const auto angle = static_cast<float>(i) * tauOverSize;
                Vector offsets{cosf(angle), sinf(angle)};
                position = 50.0f * offsets;
                velocity = magnitude(10.0f * offsets + angle, Boid::maxSpeed);
            }
        }

        m_count = size;
    }
};

