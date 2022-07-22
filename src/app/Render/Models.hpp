#pragma once
#include "pch.hpp"

constexpr std::array<float, 8> boidShape {
    1.0, 0.0,
    -0.7071067811865475f, 0.7071067811865476f,
    -0.5, 0.0,
    -0.7071067811865477f, -0.7071067811865475f,
};

constexpr std::array<float, 34> visionShape {
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


class BoidModel {
public:
    virtual ~BoidModel() = default;
    virtual void resize(size_t) = 0;
    virtual void draw() = 0;
};

class ClassicModel final : public BoidModel {
public:
    ClassicModel(size_t, lwvl::Buffer&);
    ~ClassicModel() override = default;
    void resize(size_t) override;
    void draw() override;
private:
    lwvl::VertexArray layout;
    lwvl::Buffer vertices;
    lwvl::Buffer indices;
};

class FilledModel final : public BoidModel {
public:
    FilledModel(size_t, lwvl::Buffer&);
    void resize(size_t) override;
    void draw() override;
private:
    lwvl::VertexArray layout;
    lwvl::Buffer vertices;
};

class VisionModel final : public BoidModel {
public:
    VisionModel(size_t, lwvl::Buffer&);
    void resize(size_t) override;
    void draw() override;
private:
    lwvl::VertexArray layout;
    lwvl::Buffer vertices;
};
