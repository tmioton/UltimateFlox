// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "lwvl/lwvl.hpp"

GLuint lwvl::Framebuffer::ID::reserve() {
    GLuint temp;
    glCreateFramebuffers(1, &temp);
    return temp;
}

lwvl::Framebuffer::ID::ID(int id) : id(id), safe(false) {}

lwvl::Framebuffer::ID::~ID() {
    if (safe) glDeleteFramebuffers(1, &id);
}


lwvl::Framebuffer::Framebuffer(int id) : m_offsite_id(std::make_shared<const lwvl::Framebuffer::ID>(id)) {}


void lwvl::Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id());
}

void lwvl::Framebuffer::attach(Attachment point, const Texture &texture, GLint level) {
    glNamedFramebufferTexture(id(), static_cast<GLenum>(point), texture.id(), level);
}

void lwvl::Framebuffer::attachLayer(Attachment point, const Texture &texture, GLint level, GLint layer) {
    glNamedFramebufferTextureLayer(id(), static_cast<GLenum>(point), texture.id(), level, layer);
}

void lwvl::Framebuffer::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint lwvl::Framebuffer::id() const {
    return m_offsite_id->id;
}

lwvl::Framebuffer lwvl::Framebuffer::activeDrawFramebuffer() {
    GLint draw = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw);
    return lwvl::Framebuffer(draw);
}

lwvl::Framebuffer lwvl::Framebuffer::activeReadFramebuffer() {
    GLint read = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read);
    return lwvl::Framebuffer(read);
}

bool lwvl::Framebuffer::safe() {
    return m_offsite_id->safe;
}
