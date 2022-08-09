#pragma once

#include "pch.hpp"
#include "Object.hpp"

constexpr std::array<float, 12> boidShape{
    1.0f, 0.0f, 0.0f,
    -0.707107f, 0.707107f, 0.0f,
    -0.5f, 0.0f, 0.0f,
    -0.707107f, -0.707107f, 0.0f
};

constexpr std::array<unsigned int, 10> classicIndices {
    0, 1, 1, 2, 2, 3, 2, 0, 3, 0
};

constexpr std::array<unsigned int, 6> filledIndices {
    0, 1, 2,
    2, 3, 0
};

constexpr std::array<float, 51> visionShape{
    1.0f, 0.0f, 0.0f,
    0.92388f, 0.38268f, 0.0f,
    0.70711f, 0.70711f, 0.0f,
    0.38268f, 0.92388f, 0.0f,
    0.0f, 1.0f, 0.0f,
    -0.38268f, 0.92388f, 0.0f,
    -0.70711f, 0.70711f, 0.0f,
    -0.92388f, 0.38268f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -0.92388f, -0.38268f, 0.0f,
    -0.70711f, -0.70711f, 0.0f,
    -0.38268f, -0.92388f, 0.0f,
    -0.0f, -1.0f, 0.0f,
    0.38268f, -0.92388f, 0.0f,
    0.70711f, -0.70711f, 0.0f,
    0.92388f, -0.38268f, 0.0f,
    1.0f, 0.0f, 0.0f
};

constexpr std::array<unsigned int, 34> visionIndices {
    0, 1, 1, 2, 2, 3,
    3, 4, 4, 5, 5, 6,
    6, 7, 7, 8, 8, 9,
    9, 10, 10, 11, 11, 12,
    12, 13, 13, 14, 14, 15,
    15, 16, 16, 0
};

constexpr std::array<float, 24> boxVertices {
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f
};

constexpr std::array<unsigned int, 24> boxIndices {
    0, 1, 1, 2, 2, 3, 3, 0,
    0, 4, 1, 5, 2, 6, 3, 7,
    4, 5, 5, 6, 6, 7, 7, 4
};


class Model {
public:
    explicit Model(Object const&, size_t);
    void draw() const;

    lwvl::VertexArray layout;
    lwvl::Buffer vertices;
    lwvl::Buffer indices;
    lwvl::PrimitiveMode mode;
    int32_t count;
    int32_t instances = 0;
    int16_t binding = 0;
    int16_t attribute = 0;
};

void attachBoidData(Model*, lwvl::Buffer&);
