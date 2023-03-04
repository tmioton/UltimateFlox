#pragma once

#include "pch.hpp"
#include "World/Boid.hpp"
#include "Structures/DoubleBuffer.hpp"


class ComputeAgent {
public:
    virtual ~ComputeAgent() = default;
    virtual void update(DoubleBuffer<Boid> &boids, float delta) = 0;
};
