#pragma once

#include "pch.hpp"

namespace lwvl {
    enum class ByteFormat {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        HalfFloat = GL_HALF_FLOAT,
        Float = GL_FLOAT,
    };

    void clear();

    struct Viewport {
        int x, y, width, height;
    };

    void viewport(Viewport);
    Viewport viewport();
}
