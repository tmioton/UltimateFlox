#pragma once

#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"
#include "Common.hpp"

using Vector = glm::vec2;

constexpr std::array<float, 8> defaultModel {
    1.0, 0.0,
    -0.7071067811865475f, 0.7071067811865476f,
    -0.5, 0.0,
    -0.7071067811865477f, -0.7071067811865475f,
};

constexpr std::array<float, 34> visionModel {
    1.0f, 0.0f,
    0.92388f, 0.38268f,
    0.70711f, 0.70711f,
    0.38268f, 0.92388f,
    0.0f, 1.0f,
    -0.38268f, 0.92388f,
    -0.70711f, 0.70711f,
    -0.92388f, 0.38268f,
    -1.0f, 0.0f,
    -0.92388f, -0.38268f,
    -0.70711f, -0.70711f,
    -0.38268f, -0.92388f,
    -0.0f, -1.0f,
    0.38268f, -0.92388f,
    0.70711f, -0.70711f,
    0.92388f, -0.38268f,
    1.0f, 0.0f,
};

// The maximum size of the vertex buffer.
constexpr size_t vertexBufferSize = defaultModel.size();


struct Boid {
    static constexpr float scale = 5.0f;

    static constexpr float maxSpeed = 100.0f;
    static constexpr float maxForce = 3.0f;

    static constexpr float disruptiveRadius = 1.5f * scale;
    static constexpr float cohesiveRadius = 2.0f * disruptiveRadius;

    // These weights get normalized, so they do not have to add to 1.
    static constexpr float primadonnaWeight = 0.8f;  // Desire to be on-screen. Must be stronger than alignment.
    static constexpr float alignmentWeight = 0.6f;   // Desire to move in the same direction as other boids.
    static constexpr float separationWeight = 1.0f;  // Desire to have personal space.
    static constexpr float cohesionWeight = 0.6f;    // Desire to compress the flock size.
    static constexpr float speedWeight = 0.2f;       // Desire to move at full speed. Helps with separation.

    // Non-static members
    Vector position, velocity, acceleration;

    // Methods
    [[nodiscard]] Vector steer(Vector const& vec) const;
};


class Flock {
public:
    enum class RenderMode : uint8_t {
        Classic,
        Filled
    };
private:

    static constexpr float worldBound = 200.0f;

    // without any steering, this number can go above 500,000 before dipping below 60fps
    size_t flockSize;

    std::unique_ptr<Boid[]> m_primaryFlock = std::make_unique<Boid[]>(flockSize);
    std::unique_ptr<Boid[]> m_secondaryFlock = std::make_unique<Boid[]>(flockSize);
    std::unique_ptr<float[]> offsetArray = std::make_unique<float[]>(flockSize * 4);

    // At the flock sizes I want to hit, flockSize ^ 2 would be > 4TB of RAM to store this matrix.
    //std::unique_ptr<float[]> distanceMatrix = std::make_unique<float[]>(flockSize * flockSize);

    // Boid Rendering
    lwvl::ShaderProgram boidShader;
    lwvl::VertexArray boidArrayBuffer;
    lwvl::ArrayBuffer boidVertexBuffer{lwvl::Usage::Static};
    lwvl::ElementBuffer boidIndexBuffer{lwvl::Usage::Static};

    // Boid Vision Rendering
    lwvl::ShaderProgram visionShader;
    lwvl::VertexArray visionArrayBuffer;
    lwvl::ArrayBuffer visionVertexBuffer{lwvl::Usage::Static};

    // Object Position Buffer
    lwvl::ArrayBuffer offsetBuffer{lwvl::Usage::Stream};

    //lwvl::PrimitiveMode renderMode = lwvl::PrimitiveMode::Lines;  // Classic Flox render mode
    lwvl::PrimitiveMode renderMode = lwvl::PrimitiveMode::TriangleFan;
    int32_t indexCount = 10;
    Vector bounds;
    bool renderVision = false;
    bool renderBoids = true;

public:

    explicit Flock(size_t flock_size, float aspect);
    void changeRenderMode(RenderMode mode);
    void toggleVisionRendering();
    void toggleBoidRendering();
    void update(float dt);
    void draw();

private:
    void configureRendering();
};
