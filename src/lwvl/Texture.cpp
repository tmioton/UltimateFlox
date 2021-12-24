#include "pch.hpp"
#include "Texture.hpp"


lwvl::Texture::Texture() : m_target(lwvl::Texture::Target::Texture2D) {}

lwvl::Texture::Texture(lwvl::Texture::Target t) : m_target(t) {}

GLuint lwvl::Texture::slot() const {
    return m_slot;
}

void lwvl::Texture::slot(GLuint value) {
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

void lwvl::Texture::filter(lwvl::Filter value) {
    const auto GLTarget = target();
    const auto GLFilter = static_cast<GLint>(value);
    glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GLFilter);
    glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GLFilter);
}

void lwvl::Texture::bind() {
    if (m_id != 0) {
        glActiveTexture(GL_TEXTURE0 + m_slot);
        glBindTexture(target(), m_id);
    }
}

void lwvl::Texture::construct(
    int width, int height, lwvl::ChannelLayout layout, lwvl::ChannelOrder order,
    lwvl::ByteFormat type, const void *pixels
) {
    glTexImage2D(
        target(), 0, static_cast<GLint>(layout), width, height, 0,
        static_cast<GLenum>(order), static_cast<GLenum>(type), pixels
    );
}

GLenum lwvl::Texture::target() const {
    return static_cast<GLenum>(m_target);
}

void lwvl::Texture::construct(
    int width, int height, int depth,
    lwvl::ChannelLayout layout,
    lwvl::ChannelOrder order,
    lwvl::ByteFormat type,
    const void *pixels
) {
    glTexImage3D(
        target(), 0, static_cast<GLint>(layout),
        width, height, depth, 0, static_cast<GLenum>(order),
        static_cast<GLenum>(type), pixels
    );
}

void lwvl::Texture::construct(lwvl::TextureBuffer &buffer, lwvl::ChannelLayout layout) {
    glTexBuffer(GL_TEXTURE_BUFFER, static_cast<GLenum>(layout), buffer.id());
}
