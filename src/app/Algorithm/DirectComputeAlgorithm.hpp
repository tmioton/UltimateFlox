#pragma once

#include "pch.hpp"
#include "Algorithm.hpp"
#include "Math/Rectangle.hpp"
#include "Compute/OpenGL/DirectComputeAgent.hpp"


// First naive compute algorithm
// In:
// . Previous boid position and velocity
// . Frame delta
// Out:
// . New boid position and velocity
// Should just be array copying.
// Copy the data out, but we should be able to just swap which point the arrays are bound to.


class DirectComputeAlgorithm final : public Algorithm {
public:
    explicit DirectComputeAlgorithm(Rectangle bounds);
    void update(DoubleBuffer<Boid> &boids, float dt) override;
private:
    Rectangle m_bounds;
    DirectComputeAgent m_agent;
};
