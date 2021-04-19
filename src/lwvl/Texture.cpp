#include "pch.hpp"
#include "Texture.hpp"


void lwvl::Texture2D::construct(
    uint32_t width, uint32_t height, const void *pixels,
    lwvl::ChannelLayout internalFormat, lwvl::ChannelOrder format,
    lwvl::ByteFormat type
) {
    glTexImage2D(
        GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat), width, height, 0,
        static_cast<GLenum>(format), static_cast<GLenum>(type), pixels
    );
}

void lwvl::Texture3D::construct(
    uint32_t width, uint32_t height, uint32_t depth, const void *pixels,
    lwvl::ChannelLayout internalFormat, lwvl::ChannelOrder format,
    lwvl::ByteFormat type
) {
    glTexImage3D(
        GL_TEXTURE_3D, 0, static_cast<GLenum>(internalFormat),
        width, height, depth, 0, static_cast<GLenum>(format),
        static_cast<GLenum>(type), pixels
    );
}

void lwvl::BufferTexture::construct(lwvl::TextureBuffer &buffer, lwvl::ChannelLayout internalFormat) {
    glTexBuffer(GL_TEXTURE_BUFFER, static_cast<GLenum>(internalFormat), buffer.id());
}
