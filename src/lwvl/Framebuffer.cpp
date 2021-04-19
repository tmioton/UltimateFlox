#include "pch.hpp"
#include "Framebuffer.hpp"

lwvl::Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &m_id);
}

lwvl::Framebuffer::~Framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &m_id);
    m_id = 0;
}

void lwvl::Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void lwvl::Framebuffer::attach(lwvl::Attachment point, lwvl::Texture2D &texture) {
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, static_cast<GLenum>(point),
        GL_TEXTURE_2D, texture.m_id, 0
    );
}

void lwvl::Framebuffer::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
