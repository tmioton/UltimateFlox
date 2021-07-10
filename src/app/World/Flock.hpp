#pragma once

#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Common.hpp"
#include "Boid.hpp"
#include "Renderer.hpp"


class Flock {
    static constexpr float worldBound = 200.0f;

    // without any steering, this number can go above 500,000 before dipping below 60fps
    size_t flockSize;

    BoidArray m_primaryFlock = std::make_unique<Boid[]>(flockSize);
    BoidArray m_secondaryFlock = std::make_unique<Boid[]>(flockSize);

    Vector bounds;
    BoidRenderer renderer;

public:

    explicit Flock(size_t flock_size, float aspect);
    void changeRenderMode(BoidMode mode);
    void toggleVisionRendering();
    void toggleBoidRendering();
    void update(float dt);
    void draw();
};
