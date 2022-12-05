#pragma once

#include "Structures/DoubleBuffer.hpp"
#include "Structures/Quadtree.hpp"
#include "Math/Rectangle.hpp"
#include "Boid.hpp"


class Flock {
public:
    explicit Flock(size_t flock_size, Vector bounds);

    void update(float dt);

    Boid const *boids();
    [[nodiscard]] Boidtree const &tree() const;

    void resize(size_t new_count);
private:
    struct ThreadState {
        Boidtree* tree;
        Rectangle bounds;
        Boid* write;
        Boid const* read;
        Boidtree::ResultVector* search;
        int start;
        int count;
        float delta;

        ThreadState(Boidtree*, Rectangle, Boid*, Boid const*, Boidtree::ResultVector*, int, int, float);
        ThreadState(ThreadState const&) = default;
        ThreadState(ThreadState &&) = default;

        ThreadState& operator=(ThreadState const&) = default;
        ThreadState& operator=(ThreadState&&) = default;
    };

    static void thread_update(ThreadState);

    size_t m_count;
    DoubleBuffer<Boid> m_flock;
    Boidtree::ResultVector m_results[4] {};

    Rectangle m_bounds;
    Rectangle m_treeBounds;
    Boidtree m_tree;
};
