#pragma once

#include "pch.hpp"
#include "Algorithm.hpp"
#include "QuadtreeAlgorithm.hpp"

class ThreadedAlgorithm: public QuadtreeAlgorithm {
public:
    explicit ThreadedAlgorithm(Vector bounds);
    ~ThreadedAlgorithm() override = default;

    void update(DoubleBuffer<Boid> &boids, float delta) override;
private:
    struct ThreadState {
        Boidtree* tree;
        Rectangle bounds;
        Boid* write;
        Boidtree::ResultVector* search;
        int count;
        float delta;

        ThreadState(Boidtree*, Rectangle, Boid*, Boidtree::ResultVector*, int, float);
        ThreadState(ThreadState const&) = default;
        ThreadState(ThreadState &&) = default;

        ThreadState& operator=(ThreadState const&) = default;
        ThreadState& operator=(ThreadState&&) = default;
    };

    static void thread_update(ThreadState);

    static constexpr int ThreadCount = 4;
    Boidtree::ResultVector m_results[ThreadCount];
};
