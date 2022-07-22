#include "lwvl/lwvl.hpp"
#include <iostream>


GLuint lwvl::Texture::ID::reserve(GLenum target) {
    GLuint tempID;
    glCreateTextures(target, 1, &tempID);
    return tempID;
}

lwvl::Texture::ID::ID(Target target) : id(reserve(static_cast<GLenum>(target))) {}

lwvl::Texture::ID::~ID() {
    glDeleteTextures(1, &id);
}

lwvl::Region::Region(GLsizei l, GLint o) : length(l), offset(o) {}

GLuint lwvl::Texture::id() const {
    return m_offsite_id->id;
}

lwvl::Texture::Target lwvl::Texture::target() const {
    return m_target;
}


lwvl::Texture::Texture() :
    m_offsite_id(std::make_shared<const lwvl::Texture::ID>(lwvl::Texture::Target::Texture2D)),
    m_target(lwvl::Texture::Target::Texture2D) {}

lwvl::Texture::Texture(lwvl::Texture::Target t) :
    m_offsite_id(std::make_shared<const lwvl::Texture::ID>(t)),
    m_target(t) {}

void lwvl::Texture::format(GLsizei width, lwvl::ChannelLayout layout, GLsizei levels) {
    glTextureStorage1D(id(), levels, static_cast<GLenum>(layout), width);
}

void lwvl::Texture::format(GLsizei width, GLsizei height, lwvl::ChannelLayout layout, GLsizei levels) {
    glTextureStorage2D(id(), levels, static_cast<GLenum>(layout), width, height);
}

void lwvl::Texture::format(GLsizei width, GLsizei height, GLsizei depth, lwvl::ChannelLayout layout, GLsizei levels) {
    glTextureStorage3D(id(), levels, static_cast<GLenum>(layout), width, height, depth);
}

void lwvl::Texture::format(lwvl::Buffer const &buffer, lwvl::ChannelLayout layout) {
    glTextureBuffer(id(), static_cast<GLenum>(layout), buffer.id());
}

void lwvl::Texture::format(lwvl::Buffer const &buffer, lwvl::ChannelLayout layout, GLsizeiptr size, GLintptr offset) {
    glTextureBufferRange(id(), static_cast<GLenum>(layout), buffer.id(), offset, size);
}

void lwvl::Texture::filter(lwvl::Filter value) {
    const auto GLFilter = static_cast<GLint>(value);
    glTextureParameteri(id(), GL_TEXTURE_MIN_FILTER, GLFilter);
    glTextureParameteri(id(), GL_TEXTURE_MAG_FILTER, GLFilter);
}

void lwvl::Texture::bind(GLuint slot) {
    glBindTextureUnit(slot, id());
}

void lwvl::Texture::clear(GLuint slot) {
    glBindTextureUnit(slot, 0);
}

void lwvl::Texture::construct(
    lwvl::Region x, GLsizei level,
    lwvl::ChannelOrder order, lwvl::ByteFormat type,
    const void *pixels
) {
    glTextureSubImage1D(
        id(), level,
        x.offset,
        x.length,
        static_cast<GLenum>(order),
        static_cast<GLenum>(type),
        pixels
    );
}

void lwvl::Texture::construct(
    lwvl::Region x, lwvl::Region y, GLsizei level,
    lwvl::ChannelOrder order, lwvl::ByteFormat type,
    void const *pixels
) {
    glTextureSubImage2D(
        id(), level,
        x.offset, y.offset,
        x.length, y.length,
        static_cast<GLenum>(order),
        static_cast<GLenum>(type),
        pixels
    );
}

void lwvl::Texture::construct(
    lwvl::Region x, lwvl::Region y, lwvl::Region z,
    GLsizei level, lwvl::ChannelOrder order, lwvl::ByteFormat type,
    const void *pixels
) {
    glTextureSubImage3D(
        id(), level,
        x.offset, y.offset, z.offset,
        x.length, y.length, z.length,
        static_cast<GLenum>(order),
        static_cast<GLenum>(type),
        pixels
    );
}

void lwvl::Texture::construct(
    GLsizei width, GLsizei level,
    lwvl::ChannelOrder order, lwvl::ByteFormat type,
    void const *pixels
) {
    glTextureSubImage1D(
        id(), level, 0, width,
        static_cast<GLenum>(order),
        static_cast<GLenum>(type),
        pixels
    );
}

void lwvl::Texture::construct(
    GLsizei width, GLsizei height, GLsizei level,
    lwvl::ChannelOrder order, lwvl::ByteFormat type,
    const void *pixels
) {
    glTextureSubImage2D(
        id(), level, 0, 0, width, height,
        static_cast<GLenum>(order),
        static_cast<GLenum>(type),
        pixels
    );
}

void lwvl::Texture::construct(
    GLsizei width, GLsizei height, GLsizei depth, GLsizei level,
    lwvl::ChannelOrder order, lwvl::ByteFormat type,
    const void *pixels
) {
    glTextureSubImage3D(
        id(), level, 0, 0, 0,
        width, height, depth,
        static_cast<GLenum>(order),
        static_cast<GLenum>(type),
        pixels
    );
}
