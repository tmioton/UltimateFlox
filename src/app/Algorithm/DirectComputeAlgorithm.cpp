#include "pch.hpp"
#include "DirectComputeAlgorithm.hpp"


DirectComputeAlgorithm::DirectComputeAlgorithm(Rectangle bounds) : m_bounds(bounds), m_agent(bounds) {}

void DirectComputeAlgorithm::update(DoubleBuffer<Boid> &boids, float dt) {
    m_agent.update(boids, dt);
}
