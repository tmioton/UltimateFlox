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
        class TextureID {
        protected:
            static unsigned int reserve() {
                unsigned int tempID;
                glGenTextures(1, &tempID);
                return tempID;
            }

        public:
            ~TextureID() {
                glDeleteTextures(1, &textureID);
            }

            explicit operator uint32_t() const {
                return textureID;
            }

            const uint32_t textureID = reserve();
        };

        enum class TextureTarget {
            Texture2D = GL_TEXTURE_2D,
            Texture3D = GL_TEXTURE_3D,
            TextureBuffer = GL_TEXTURE_BUFFER
        };

        template<TextureTarget target>
        class TextureBase {
        protected:
            // Offsite Data - to avoid copying buffers on the GPU for simple copies of this class.
            std::shared_ptr<TextureID> m_offsite_id = std::make_shared<TextureID>();

            // Local Data
            uint32_t m_id = static_cast<uint32_t>(*m_offsite_id);
            uint32_t m_slot = 0;

            friend Framebuffer;
        public:
            [[nodiscard]] uint32_t slot() const {
                return m_slot;
            }

            void slot(uint32_t value) {
                m_slot = value;
                //int32_t maxTextureUnits;
                //glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
                //
                //if (static_cast<uint32_t>(maxTextureUnits) < value) {
                //    m_slot = value;
                //} else {
                //    throw std::exception("Max Texture Units Exceeded.");
                //}
            }

            void filter(Filter value) {
                const auto GLTarget = static_cast<GLenum>(target);
                const auto GLFilter = static_cast<GLenum>(value);
                glTexParameteri(GLTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GLTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GLFilter);
                glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GLFilter);
            }

            void bind() {
                if (m_id != 0) {
                    glActiveTexture(GL_TEXTURE0 + m_slot);
                    glBindTexture(static_cast<GLenum>(target), m_id);
                }
            }
        };
    }

    class Texture2D : public detail::TextureBase<detail::TextureTarget::Texture2D> {
    public:
        void construct(
            uint32_t width, uint32_t height, const void *pixels,
            ChannelLayout internalFormat = ChannelLayout::R8UI, ChannelOrder format = ChannelOrder::Red,
            ByteFormat type = ByteFormat::Byte
        );
    };

    class Texture3D : public detail::TextureBase<detail::TextureTarget::Texture3D> {
    public:
        void construct(
            uint32_t width, uint32_t height, uint32_t depth, const void *pixels,
            ChannelLayout internalFormat = ChannelLayout::R8UI, ChannelOrder format = ChannelOrder::Red,
            ByteFormat type = ByteFormat::Byte
        );
    };

    class BufferTexture : public detail::TextureBase<detail::TextureTarget::TextureBuffer> {
    public:
        void construct(TextureBuffer &buffer, ChannelLayout internalFormat = ChannelLayout::R8UI);
    };
}
