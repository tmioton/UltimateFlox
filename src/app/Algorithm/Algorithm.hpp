#pragma once

#include "pch.hpp"
#include "World/Boid.hpp"
#include "Structures/DoubleBuffer.hpp"


class Algorithm {
public:
    virtual ~Algorithm() = default;

    virtual void update(DoubleBuffer<Boid> &boids, float delta) = 0;
};
