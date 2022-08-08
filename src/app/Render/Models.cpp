#include "pch.hpp"
#include "World/Boid.hpp"
#include "Models.hpp"


// ****** Classic Model ******
ClassicModel::ClassicModel(size_t size, lwvl::Buffer &boidData) {
    layout.instances = size;
    vertices.store(boidShape.begin(), boidShape.end());
    layout.array(vertices, 0, 0, 2 * sizeof(float));
    layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, 0);

    std::array<uint8_t, 10> indexData{
        0, 1, 1, 2, 2, 3, 2, 0, 3, 0
    };

    indices.store(indexData.begin(), indexData.end());
    layout.element(indices);

    layout.array(boidData, 1, 0, sizeof(Boid));
    layout.attribute(
        1, 1, Vector::length(),
        lwvl::ByteFormat::Float,
        offsetof(Boid, position)
    );
    layout.attribute(
        1, 2, Vector::length(),
        lwvl::ByteFormat::Float,
        offsetof(Boid, velocity)
    );
    layout.divisor(1, 1);
}

void ClassicModel::resize(size_t size) {
    layout.instances = size;
}

void ClassicModel::draw() {
    layout.drawElements(lwvl::PrimitiveMode::Lines, 10, lwvl::ByteFormat::UnsignedByte);
}


// ****** Filled Model ******
FilledModel::FilledModel(size_t size, lwvl::Buffer &boidData) {
    layout.instances = size;
    vertices.store(boidShape.begin(), boidShape.end());
    layout.array(vertices, 0, 0, 2 * sizeof(float));
    layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, 0);

    layout.array(boidData, 1, 0, sizeof(Boid));
    layout.attribute(
        1, 1, Vector::length(),
        lwvl::ByteFormat::Float,
        offsetof(Boid, position)
    );
    layout.attribute(
        1, 2, Vector::length(),
        lwvl::ByteFormat::Float,
        offsetof(Boid, velocity)
    );
    layout.divisor(1, 1);
}

void FilledModel::resize(size_t size) {
    layout.instances = size;
}

void FilledModel::draw() {
    layout.drawArrays(lwvl::PrimitiveMode::TriangleFan, 4);
}


// ****** Vision Model ******
VisionModel::VisionModel(size_t size, lwvl::Buffer &boidData) {
    layout.instances = size;
    vertices.store(visionShape.begin(), visionShape.end());
    layout.array(vertices, 0, 0, 2 * sizeof(float));
    layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, 0);

    layout.array(boidData, 1, 0, sizeof(Boid));
    layout.attribute(1, 1, Vector::length(), lwvl::ByteFormat::Float, offsetof(Boid, position));
    layout.attribute(1, 2, Vector::length(), lwvl::ByteFormat::Float, offsetof(Boid, velocity));
    layout.divisor(1, 1);
}

void VisionModel::resize(size_t size) {
    layout.instances = size;
}

void VisionModel::draw() {
    layout.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);
}
