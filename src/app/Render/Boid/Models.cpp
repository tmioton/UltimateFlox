// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "World/Boid.hpp"
#include "Models.hpp"

Model::Model(Object const& object, size_t instances): mode(object.drawMode()), count(object.indexCount()), instances(instances) {
    layout.instances = instances;
    vertices.store(object.vertices().begin(), object.vertices().end());
    layout.array(vertices, 0, 0, 3 * sizeof(float));
    layout.attribute(0, 0, 3, lwvl::ByteFormat::Float, 0);

    indices.store(object.indices().begin(), object.indices().end());
    layout.element(indices);
}

void Model::draw() const {
    layout.drawElements(mode, count, lwvl::ByteFormat::UnsignedInt);
}

void attachBoidData(Model *model, lwvl::Buffer& buffer) {
    model->layout.array(buffer, 1, 0, sizeof(Boid));
    model->layout.attribute(1, 1, Vector::length(), lwvl::ByteFormat::Float, offsetof(Boid, position));
    model->layout.attribute(1, 2, Vector::length(), lwvl::ByteFormat::Float, offsetof(Boid, velocity));
    model->layout.divisor(1, 1);
}
