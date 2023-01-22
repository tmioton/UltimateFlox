#pragma once

#include "pch.hpp"
#include "Algorithm.hpp"
#include "World/Boidtree.hpp"


class ThreadedAlgorithm: public Algorithm {
    struct ThreadWork {
        ThreadedAlgorithm* algorithm;
        int id;
        float delta;
        const Boid* read;
        Boid* write;
        ptrdiff_t count;
        ptrdiff_t start;

        ThreadWork(ThreadedAlgorithm*, int, float, const Boid*, Boid*, ptrdiff_t, ptrdiff_t);
        //ThreadWork(ThreadedAlgorithm*, int, float, Boid*, ptrdiff_t);
        void operator()() const;
    };

public:
    explicit ThreadedAlgorithm(Vector bounds);
    ~ThreadedAlgorithm() override;

    void update(DoubleBuffer<Boid> &thread_work, float delta) override;
    [[nodiscard]] Boidtree const &tree() const;
private:
    static constexpr int ThreadCount = 4;

    Rectangle m_bounds;
    Rectangle m_treeBounds;
    Boidtree m_tree;

    ThreadPool m_pool{ThreadCount - 1};
    Boidtree::ResultVector m_results[ThreadCount];
};
