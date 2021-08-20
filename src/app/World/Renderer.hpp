#pragma once

#include "pch.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"

#include "Boid.hpp"
#include "World.hpp"

constexpr std::array<float, 8> defaultModel{
    1.0, 0.0,
    -0.7071067811865475f, 0.7071067811865476f,
    -0.5, 0.0,
    -0.7071067811865477f, -0.7071067811865475f,
};

constexpr std::array<float, 34> visionModel{
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

class DataBufferUpdater {
public:
    explicit DataBufferUpdater(size_t size);
    lwvl::ArrayBuffer& buffer();
    void update(BoidArray const &array);

private:
    // Don't need the offset store because we want to upload the boid data as-is
    lwvl::ArrayBuffer dataBuffer;
    size_t flockSize;
};

class BoidRenderer {
public:
    enum class RenderMode {
        Classic,
        Filled
    };

    enum class ControlMode {
        Default,
        SpeedDebug
    };

    BoidRenderer(size_t size, int width, int height, lwvl::ArrayBuffer& offsetBuffer);
    void changeRenderMode(RenderMode mode);
    void changeControlMode(ControlMode mode);
    void draw();

private:
    typedef enum { defaultControl, speedDebugControl } ControlSelect;

    lwvl::ShaderProgram activeControl;
    lwvl::VertexArray layout;
    lwvl::ArrayBuffer vertices{lwvl::Usage::Static};
    lwvl::ElementBuffer indices{lwvl::Usage::Static};

    lwvl::ShaderProgram controls[2];

    lwvl::PrimitiveMode renderMode = lwvl::PrimitiveMode::TriangleFan;
    int32_t indexCount = 10;
};

class VisionRenderer {
public:
    VisionRenderer(size_t size, int width, int height, lwvl::ArrayBuffer& offsetBuffer);
    void draw();

private:
    lwvl::ShaderProgram control;
    lwvl::VertexArray layout;
    lwvl::ArrayBuffer vertices{lwvl::Usage::Static};
};
