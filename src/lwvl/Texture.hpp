#pragma once

#include "pch.hpp"
#include "Framebuffer.hpp"
#include "Buffer.hpp"
#include "Common.hpp"

namespace lwvl {
    class Framebuffer;

    enum class Filter {
        Linear = GL_LINEAR,
        Nearest = GL_NEAREST
    };

    enum class ChannelLayout {
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,

        //R8 = GL_R8,
        //R16 = GL_R16,
        //RG8 = GL_RG8,
        //RG16 = GL_RG16,
        //RGB8 = GL_RGB8,
        //RGBA2 = GL_RGBA2,
        //RGBA4 = GL_RGBA4,
        //RGBA8 = GL_RGBA8,
        //RGBA16 = GL_RGBA16,

        // Half Float Formats
        R16F = GL_R16F,
        RG16F = GL_RG16F,
        RGB16F = GL_RGB16F,
        RGBA16F = GL_RGBA16F,

        // Full Float Formats
        R32F = GL_R32F,
        RG32F = GL_RG32F,
        RGB32F = GL_RGB32F,
        RGBA32F = GL_RGBA32F,

        // Signed Integer Formats
        R8I = GL_R8I,
        RG8I = GL_RG8I,
        RGB8I = GL_RGB8I,
        RGBA8I = GL_RGBA8I,

        R16I = GL_R16I,
        RG16I = GL_RG16I,
        RGB16I = GL_RGB16I,
        RGBA16I = GL_RGBA16I,

        R32I = GL_R32I,
        RG32I = GL_RG32I,
        RGB32I = GL_RGB32I,
        RGBA32I = GL_RGBA32I,

        // Unsigned Integer Formats
        R8UI = GL_R8UI,
        RG8UI = GL_RG8UI,
        RGB8UI = GL_RGB8UI,
        RGBA8UI = GL_RGBA8UI,

        R16UI = GL_R16UI,
        RG16UI = GL_RG16UI,
        RGB16UI = GL_RGB16UI,
        RGBA16UI = GL_RGBA16UI,

        R32UI = GL_R32UI,
        RG32UI = GL_RG32UI,
        RGB32UI = GL_RGB32UI,
        RGBA32UI = GL_RGBA32UI,
    };

    enum class ChannelOrder {
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,
        StencilIndex = GL_STENCIL_INDEX,
        DepthComponent = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL
    };

    namespace detail {
        class TextureInfo {
        protected:
            static unsigned int reserve() {
                unsigned int tempID;
                glGenTextures(1, &tempID);
                return tempID;
            }
        public:
            ~TextureInfo() {
                glDeleteTextures(1, &id);
            }

            explicit operator GLuint() const {
                return id;
            }

            const GLuint id = reserve();
        };
    }

    class Texture {
        friend Framebuffer;
    public:
        enum class Target {
            Texture1D = GL_TEXTURE_1D,
            Texture1DArray = GL_TEXTURE_1D_ARRAY,
            Texture2D = GL_TEXTURE_2D,
            Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,
            Texture2DArray = GL_TEXTURE_2D_ARRAY,
            Texture2DArrayMultisample = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            TextureRectangle = GL_TEXTURE_RECTANGLE,
            TextureCubeMap = GL_TEXTURE_CUBE_MAP,
            TextureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
            Texture3D = GL_TEXTURE_3D,
            TextureBuffer = GL_TEXTURE_BUFFER
        };

        Texture();
        explicit Texture(Target t);

        [[nodiscard]] GLuint slot() const;
        void slot(GLuint value);
        void filter(Filter value);
        void bind();

        void construct(
            int width, int height,
            ChannelLayout layout, ChannelOrder order, ByteFormat type,
            const void *pixels
        );

        void construct(
            int width, int height, int depth,
            ChannelLayout layout, ChannelOrder order, ByteFormat type,
            const void *pixels
        );

        void construct(TextureBuffer &buffer, ChannelLayout layout);

    private:
        [[nodiscard]] inline GLenum target() const;

        // Offsite data for easy copies.
        std::shared_ptr<detail::TextureInfo> m_info;

        // Local data to avoid lookups.
        GLuint m_id = static_cast<GLuint>(*m_info);
        Target m_target;
        GLuint m_slot = 0;
    };
}
