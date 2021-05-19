#pragma once

#include "pch.hpp"
#include "Buffer.hpp"
#include "Shader.hpp"


namespace lwvl {
    class WorldBlock {
    public:
        WorldBlock() = default;

        void attach(const ShaderProgram& program);

    private:
        lwvl::UniformBuffer blockBuffer;
    };
}
