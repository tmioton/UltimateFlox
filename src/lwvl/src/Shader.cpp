#include "lwvl/lwvl.hpp"
#ifndef _WIN32
#include <iostream>
#endif


/* ****** Uniform ****** */
lwvl::Uniform::Uniform(int location) : m_location(location) {}

void lwvl::Uniform::setI(GLint v0) { glUniform1i(m_location, v0); }

void lwvl::Uniform::setI(GLint v0, GLint v1) { glUniform2i(m_location, v0, v1); }

void lwvl::Uniform::setI(GLint v0, GLint v1, GLint v2) { glUniform3i(m_location, v0, v1, v2); }

void lwvl::Uniform::setI(GLint v0, GLint v1, GLint v2, GLint v3) { glUniform4i(m_location, v0, v1, v2, v3); }

void lwvl::Uniform::setF(GLfloat v0) { glUniform1f(m_location, v0); }

void lwvl::Uniform::setF(GLfloat v0, GLfloat v1) { glUniform2f(m_location, v0, v1); }

void lwvl::Uniform::setF(GLfloat v0, GLfloat v1, GLfloat v2) { glUniform3f(m_location, v0, v1, v2); }

void lwvl::Uniform::setF(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { glUniform4f(m_location, v0, v1, v2, v3); }

void lwvl::Uniform::setU(GLuint v0) { glUniform1ui(m_location, v0); }

void lwvl::Uniform::setU(GLuint v0, GLuint v1) { glUniform2ui(m_location, v0, v1); }

void lwvl::Uniform::setU(GLuint v0, GLuint v1, GLuint v2) { glUniform3ui(m_location, v0, v1, v2); }

void lwvl::Uniform::setU(GLuint v0, GLuint v1, GLuint v2, GLuint v3) { glUniform4ui(m_location, v0, v1, v2, v3); }

void lwvl::Uniform::matrix4F(const float *data) {
    glUniformMatrix4fv(m_location, 1, GL_FALSE, data);
}

void lwvl::Uniform::ortho(float top, float bottom, float right, float left, float far, float near) {
    float ortho[16] = {
        // top 3 rows
        2.0f / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
        0.0f, 0.0f, 2.0f / (far - near), 0.0f,

        // bottom row
        -(right + left) / (right - left),
        -(top + bottom) / (top - bottom),
        -(far + near) / (far - near),
        1.0f
    };

    glUniformMatrix4fv(m_location, 1, GL_FALSE, ortho);
}

void lwvl::Uniform::ortho2D(float top, float bottom, float right, float left) {
    float rlStein = 1.0f / (right - left);
    float tbStein = 1.0f / (top - bottom);

    float ortho[16] = {
        // top 3 rows
        2.0f * rlStein, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f * tbStein, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        -(right + left) * rlStein, -(top + bottom) * tbStein, 0.0f, 1.0f
    };

    glUniformMatrix4fv(m_location, 1, GL_FALSE, ortho);
}

void lwvl::Uniform::ortho2D(float aspect) {
    float w, h;
    float xOffset, yOffset;
    if (aspect >= 1.0f) {
        w = aspect;
        h = 1.0f;
        xOffset = -1.0f / aspect;
        yOffset = -1.0f;
    } else {
        w = 1.0f;
        h = 1.0f / aspect;
        xOffset = -1.0f;
        yOffset = -aspect;
    }

    float ortho[16]{
        2.0f / w, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / h, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        xOffset, yOffset, 0.0f, 1.0f
    };

    glUniformMatrix4fv(m_location, 1, GL_FALSE, ortho);
}

int lwvl::Uniform::location() const {
    return m_location;
}

bool lwvl::Uniform::exists() const {
    return m_location > -1;
}


/* ****** Shader ****** */
GLuint lwvl::Program::ID::reserve() {
    return glCreateProgram();
}

lwvl::Program::ID::~ID() {
    glDeleteProgram(id);
}

int lwvl::Program::uniformLocation(const char *name) const {
    return glGetUniformLocation(id(), name);
}

GLuint lwvl::Program::id() const {
    return m_offsite_id->id;
}

lwvl::Uniform lwvl::Program::uniform(const char *name) {
    int location = uniformLocation(name);
    return Uniform(location);
}

void lwvl::Program::link() {
    /* Links the program object.
    * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glLinkProgram.xhtml
    *
    * If any shader objects of type GL_VERTEX_SHADER are attached to program,
    * they will be used to create an executable that will run on the programmable vertex processor.
    *
    * If any shader objects of type GL_GEOMETRY_SHADER are attached to program,
    * they will be used to create an executable that will run on the programmable geometry processor.
    *
    * If any shader objects of type GL_FRAGMENT_SHADER are attached to program,
    * they will be used to create an executable that will run on the programmable fragment processor.
    */

    const GLuint pid = id();
    glLinkProgram(pid);
    // assert(glGetProgramiv(m_offsite_id, GL_LINK_STATUS) == GL_TRUE);
    // assertion because the user can't change enough with shader files to break this.

    GLint isLinked = 0;
    glGetProgramiv(pid, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        auto *infoLog = new GLchar[maxLength];
        glGetProgramInfoLog(pid, maxLength, &maxLength, &infoLog[0]);

        // The program is useless now. So delete it.
        glDeleteProgram(pid);

        // Provide the infolog in whatever manner you deem best.
        std::stringstream error;
        error << "Failed to link program:" << std::endl << infoLog << std::endl;
        delete[] infoLog;

        // Exit with failure.
        throw std::invalid_argument(error.str().c_str());
    }

    glValidateProgram(pid);
    // assert(glGetProgramiv(m_offsite_id, GL_VALIDATION_STATUS) == GL_TRUE);

    GLint isValid = 0;
    glGetProgramiv(pid, GL_VALIDATE_STATUS, &isValid);
    if (isValid == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        auto *infoLog = new GLchar[maxLength];
        glGetProgramInfoLog(pid, maxLength, &maxLength, &infoLog[0]);

        // The program is useless now. So delete it.
        glDeleteProgram(pid);

        // Provide the infolog in whatever manner you deem best.
        std::stringstream error;
        error << "Failed to validate program:" << std::endl << infoLog << std::endl;
        delete[] infoLog;

        // Exit with failure.
        throw std::invalid_argument(error.str().c_str());
    }

    /* Checks to see whether the executables contained in program can execute given the current OpenGL state.
    * https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glValidateProgram.xhtml
    *
    * The error GL_INVALID_OPERATION will be generated by any command that triggers the rendering of geometry if:
    * . Any two active samplers in the current program object are of different types, but refer to the same texture image unit
    * . The number of active samplers in the program exceeds the maximum number of texture image units allowed
    *
    * It may be difficult or cause a performance degradation for applications to catch these errors when rendering commands are issued.
    * Therefore, applications are advised to make calls to glValidateProgram to detect these issues during application development.
    */
}

void lwvl::Program::link(const VertexShader &vs, const FragmentShader &fs) {
    const GLuint pid = id();
    const GLuint vso = vs.id();
    const GLuint fso = fs.id();
    glAttachShader(pid, vso);
    glAttachShader(pid, fso);

    link();

    glDetachShader(pid, vso);
    glDetachShader(pid, fso);
}

void lwvl::Program::link(std::string const &vertexSource, std::string const &fragmentSource) {
    VertexShader vs(vertexSource);
    FragmentShader fs(fragmentSource);
    link(vs, fs);
}

void lwvl::Program::bind() const {
    glUseProgram(id());
}

void lwvl::Program::clear() {
    glUseProgram(0);
}

int lwvl::Program::active() {
    GLint temp;
    glGetIntegerv(GL_ACTIVE_PROGRAM, &temp);
    return temp;
}
