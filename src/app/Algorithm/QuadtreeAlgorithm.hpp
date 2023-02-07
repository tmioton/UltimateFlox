#pragma once

#include "pch.hpp"
#include "Algorithm.hpp"
#include "World/Boidtree.hpp"

// Expose tree for rendering
class QuadtreeAlgorithm : public Algorithm {
public:
    explicit QuadtreeAlgorithm(Vector bounds);

    ~QuadtreeAlgorithm() override = default;

    void update(DoubleBuffer<Boid> &boids, float delta) override;

    [[nodiscard]] Boidtree const &tree() const;

protected:
    using QuadtreeResults = std::vector<Boid>;

    Rectangle m_bounds;
    Rectangle m_treeBounds;
    Boidtree m_tree;

    QuadtreeResults m_results;
};
