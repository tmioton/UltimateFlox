#pragma once

#include "World.hpp"
#include "Boid.hpp"


class Flock {
    // without any steering, this number can go above 500,000 before dipping below 60fps
    size_t flockSize;

    BoidArray m_primaryFlock = std::make_unique<Boid[]>(flockSize);
    BoidArray m_secondaryFlock = std::make_unique<Boid[]>(flockSize);

    Vector m_bounds;

public:
    explicit Flock(size_t flock_size, int width, int height);
    void update(float dt);

    BoidArray& boids();
    void resize(size_t size);
};
