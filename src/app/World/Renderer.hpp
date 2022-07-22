#pragma once

#include "pch.hpp"

#include "Boid.hpp"
#include "World.hpp"

class DataBufferUpdater {
public:
    explicit DataBufferUpdater(size_t size);
    lwvl::Buffer& buffer();
    void update(BoidArray const &array);
    void resize(size_t size);

private:
    // Don't need the offset store because we want to upload the boid data as-is
    lwvl::Buffer dataBuffer;
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

    BoidRenderer(size_t size, int width, int height, lwvl::Buffer& offsetBuffer);
    void changeRenderMode(RenderMode mode);
    void changeControlMode(ControlMode mode);
    void resize(size_t size);
    void draw();

private:
    typedef enum { defaultControl, speedDebugControl } ControlSelect;

    lwvl::Program activeControl;
    lwvl::VertexArray layout;
    lwvl::Buffer vertices;
    lwvl::Buffer indices;

    lwvl::Program controls[2];

    lwvl::PrimitiveMode renderMode = lwvl::PrimitiveMode::TriangleFan;
    int32_t indexCount = 10;
};

class VisionRenderer {
public:
    VisionRenderer(size_t size, int width, int height, lwvl::Buffer& offsetBuffer);
    void resize(size_t size);
    void draw();

private:
    lwvl::Program control;
    lwvl::VertexArray layout;
    lwvl::Buffer vertices;
};
