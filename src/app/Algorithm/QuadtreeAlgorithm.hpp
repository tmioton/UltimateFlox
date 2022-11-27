#pragma once

#include "pch.hpp"
#include "Algorithm.hpp"
#include "Structures/Quadtree.hpp"

// Expose tree for rendering
class QuadtreeAlgorithm final : public Algorithm {
public:
    explicit QuadtreeAlgorithm(Vector bounds);

    ~QuadtreeAlgorithm() override = default;

    void update(DoubleBuffer<Boid> &boids, float delta) override;

    [[nodiscard]] Boidtree const &tree() const;

private:
    Rectangle m_bounds;
    Boidtree m_tree;

    std::vector<int> m_failed; // Boids that failed to get into the tree
};
