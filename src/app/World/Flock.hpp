#pragma once

#include "Algorithm/Algorithm.hpp"
#include "Structures/DoubleBuffer.hpp"
#include "World.hpp"
#include "Boid.hpp"


class Flock {
    // without any steering, this number can go above 500,000 before dipping below 60fps
    size_t m_count;
    DoubleBuffer<Boid> m_flock;

public:
    explicit Flock(size_t flock_size);

    void update(Algorithm *, float dt);

    Boid const *boids();

    void resize(size_t new_count);
};
