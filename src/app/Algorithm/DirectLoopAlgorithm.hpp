#pragma once

#include "pch.hpp"
#include "Algorithm.hpp"


class DirectLoopAlgorithm final : public Algorithm {
public:
    explicit DirectLoopAlgorithm(Vector bounds);

    ~DirectLoopAlgorithm() override = default;

    void update(DoubleBuffer<Boid> &boids, float delta) override;

private:
    Vector m_bounds;
};
