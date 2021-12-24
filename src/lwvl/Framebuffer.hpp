#pragma once

#include "pch.hpp"
#include "Texture.hpp"

// Look into adding exceptions based on https://docs.gl/gl4/glFramebufferTexture

namespace lwvl {
    enum class Attachment {
        Color = GL_COLOR_ATTACHMENT0,
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT
    };

    class Texture;

    class Framebuffer {
        uint32_t m_id = 0;

    public:
        Framebuffer();

        ~Framebuffer();

        void attach(Attachment, Texture &, int level = 0);
        void attach1D(Attachment, Texture &, int level = 0);
        void attach2D(Attachment, Texture &, int level = 0);
        void attach3D(Attachment, Texture &, int layer, int level = 0);

        void bind();

        static void clear();
    };
}
