#pragma once

#include "pch.hpp"
#include "Algorithm.hpp"
#include "World/Boidtree.hpp"


class ThreadedAlgorithm : public Algorithm {
    struct ThreadWork {
        ThreadedAlgorithm *algorithm;
        int id;
        float delta;
        const Boid *read;
        Boid *write;
        ptrdiff_t count;
        ptrdiff_t start;

        ThreadWork(ThreadedAlgorithm *, int, float, const Boid *, Boid *, ptrdiff_t, ptrdiff_t);
        void operator()() const;
    };


    void populate_tree(const Boid *read, ptrdiff_t count);
    void distribute_work(const Boid *read, Boid *write, ptrdiff_t count, float delta);
    void recalculate_bounds(Boid *write, ptrdiff_t count);

public:
    explicit ThreadedAlgorithm(Vector bounds);
    ~ThreadedAlgorithm() override;

    void update(DoubleBuffer<Boid> &boids, float delta) override;
    [[nodiscard]] Boidtree const &tree() const;
private:
    static constexpr int ThreadCount = 8;
    using ThreadFutures = std::array<std::future<void>, ThreadCount - 1>;
    using QuadtreeResults = std::vector<Boid>;

    Rectangle m_bounds;
    Rectangle m_treeBounds;
    Boidtree m_tree;
    //std::mutex m_mutex;

    ThreadPool m_pool {ThreadCount - 1};
    QuadtreeResults m_results[ThreadCount];
};
