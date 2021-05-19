#include "pch.hpp"
#include "WorldBlock.hpp"

void lwvl::WorldBlock::attach(const lwvl::ShaderProgram& program) {
    uint32_t indices[2];
    const char* names[2] {
        "WorldBlock.projection", "WorldBlock.camera"
    };

    glGetUniformIndices(program.id(), 2, names, indices);
    auto [projectionIndex, cameraIndex] = indices;
}
