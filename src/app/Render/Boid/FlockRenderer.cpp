#include "pch.hpp"
#include "FlockRenderer.hpp"

FlockRenderer::FlockRenderer(size_t size) : flockSize(size) {
    data.store<float>(nullptr, size * sizeof(Boid), lwvl::bits::Dynamic);
}

void FlockRenderer::update(Boid const array[]) {
    data.update(array, flockSize * sizeof(Boid));
}

void FlockRenderer::resize(size_t size) {
    data.store<float>(nullptr, size * sizeof(Boid), lwvl::bits::Dynamic);
    flockSize = size;
}

void FlockRenderer::draw(Model *model, BoidShader *shader) {
    shader->draw(model);
}

void FlockRenderer::attachData(Model *model) {
    attachBoidData(model, data);
}
