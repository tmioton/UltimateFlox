#pragma once

#include "pch.hpp"
#include "Common.hpp"


namespace lwvl {
    enum class PrimitiveMode {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineLoop = GL_LINE_LOOP,
        LineStrip = GL_LINE_STRIP,
        LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
        Triangles = GL_TRIANGLES,
        TriangleFan = GL_TRIANGLE_FAN,
        TriangleStrip = GL_TRIANGLE_STRIP,
        TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
        TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,
    };

    class VertexArray {
        // There are only < 256 attribute bind sites, so this could be a uint16_t
        //   if another 16 or 2x8 bytes can be used for something else.
        uint32_t m_attributes = 0;
        uint32_t m_instances = 1;
        uint32_t m_id = 0;
    public:

        VertexArray();

        ~VertexArray();

        void bind();

        static void clear();

        [[nodiscard]] uint32_t instances() const;

        void instances(uint32_t count);

        void attribute(uint8_t dimensions, GLenum type, int64_t stride, int64_t offset, uint32_t divisor = 0);

        void drawArrays(PrimitiveMode mode, int count) const;

        void drawElements(PrimitiveMode mode, int count, ByteFormat type) const;

        void multiDrawArrays(PrimitiveMode mode, const GLint *firsts, const GLsizei *counts, GLsizei drawCount);

        void multiDrawElements(
            PrimitiveMode mode, const GLsizei *counts, ByteFormat type, const void *const *indices, GLsizei drawCount
        );
    };
}
