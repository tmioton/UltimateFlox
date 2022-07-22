#include "lwvl/lwvl.hpp"

GLuint lwvl::Buffer::ID::reserve() {
    GLuint temp;
    glCreateBuffers(1, &temp);
    return temp;
}

lwvl::Buffer::ID::~ID() {
    glDeleteBuffers(1, &id);
}

GLuint lwvl::Buffer::id() const {
    return m_offsite_id->id;
}

void lwvl::Buffer::bind(lwvl::Buffer::Target t) {
    glBindBuffer(static_cast<GLenum>(t), id());
}

void lwvl::Buffer::bind(lwvl::Buffer::IndexedTarget t, GLuint index) {
    glBindBufferBase(static_cast<GLenum>(t), index, id());
}

void lwvl::Buffer::clear(lwvl::Buffer::Target t) {
    glBindBuffer(static_cast<GLenum>(t), 0);
}
