#include "pch.hpp"
#include "DirectComputeAgent.hpp"


void DirectComputeAgent::delete_buffer(const GLuint id) {
    if (id == 0) { return; }
    GLint mapped;
    glGetNamedBufferParameteriv(id, GL_BUFFER_MAPPED, &mapped);
    if (mapped) {
        glUnmapNamedBuffer(id);
    }
    glDeleteBuffers(1, &id);
}

GLuint DirectComputeAgent::get_current_program() {
    GLint prev;
    glGetIntegerv(GL_CURRENT_PROGRAM, &prev);
    return prev;
}

void DirectComputeAgent::validate_shader(GLuint id) {
    GLint success = -1;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE) { return; }
    else if (success != GL_FALSE) { unexpected(); }

    GLint length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    std::unique_ptr<char[]> log = std::make_unique<char[]>(length);
    glGetShaderInfoLog(id, length, &length, log.get());
    glDeleteShader(id);
    std::stringstream error;
    error << "Failed to compile compute shader:\n" << log.get() << '\n';
    throw std::invalid_argument(error.str().c_str());
}

void DirectComputeAgent::validate_program(const GLuint id, const GLenum stage) {
    GLint success = -1;
    glGetProgramiv(id, stage, &success);
    if (success == GL_TRUE) { return; }
    else if (success != GL_FALSE) { unexpected(); }

    GLint length;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    std::unique_ptr<char[]> log = std::make_unique<char[]>(length);
    glGetProgramInfoLog(id, length, &length, log.get());
    glDeleteProgram(id);
    std::stringstream error;
    error << "Failed to compile compute shader:\n" << log.get() << '\n';
    throw std::invalid_argument(error.str().c_str());
}

GLuint DirectComputeAgent::compile() {
    const std::string source {read_file("Data/Shaders/direct.compute")};
    const GLuint program = glCreateProgram();
    const GLuint shader {glCreateShader(GL_COMPUTE_SHADER)};
    const GLchar* src {source.c_str()};
    const auto length {static_cast<GLint>(source.length())};
    glShaderSource(shader, 1, &src, &length);
    glCompileShader(shader);
    validate_shader(shader);

    glAttachShader(program, shader);
    glLinkProgram(program);
    validate_program(program, GL_LINK_STATUS);

    glValidateProgram(program);
    validate_program(program, GL_VALIDATE_STATUS);

    glDetachShader(program, shader);
    glDeleteShader(shader);
    return program;
}

std::string DirectComputeAgent::read_file(const char* path) {
    // Handle errors on this.
    // Compilation acts as a sort of error handling.
    std::ifstream file {path};
    std::stringstream output_stream;

    std::string line;
    while (getline(file, line)) {
        output_stream << line << '\n';
    }

    return output_stream.str();
}

void DirectComputeAgent::resize_buffers(std::size_t count) {
    const size_t buffer_size = count * sizeof(Boid);
    const auto gl_buffer_size = static_cast<GLsizeiptr>(buffer_size);

    m_flock_size = count;
    const GLbitfield common_storage_flags {
        GL_MAP_PERSISTENT_BIT
        //| GL_MAP_COHERENT_BIT  // Automatic glMemoryBarrier call
        | GL_CLIENT_STORAGE_BIT  // Store data on CPU or GPU
        //| GL_DYNAMIC_STORAGE_BIT
    };

    const GLbitfield common_map_flags {
        GL_MAP_PERSISTENT_BIT
    };

    delete_buffer(m_write_buffer_id);
    glCreateBuffers(1, &m_write_buffer_id);
    glNamedBufferStorage(
        m_write_buffer_id, gl_buffer_size, nullptr,
        GL_MAP_READ_BIT | common_storage_flags
    );

    m_write = reinterpret_cast<const Boid*>(glMapNamedBufferRange(
        m_write_buffer_id, 0, gl_buffer_size,
        GL_MAP_READ_BIT | common_map_flags
    ));

    delete_buffer(m_read_buffer_id);
    glCreateBuffers(1, &m_read_buffer_id);
    glNamedBufferStorage(
        m_read_buffer_id, gl_buffer_size, nullptr,
        GL_MAP_WRITE_BIT | common_storage_flags
    );

    m_read = reinterpret_cast<Boid*>(glMapNamedBufferRange(
        m_read_buffer_id, 0, gl_buffer_size,
        GL_MAP_WRITE_BIT
        | GL_MAP_INVALIDATE_BUFFER_BIT
        | GL_MAP_FLUSH_EXPLICIT_BIT
        | common_map_flags
    ));
}

void DirectComputeAgent::write_uniforms(float delta) const {
    if (u_delta > -1) { glUniform1f(u_delta, delta); }
    if (u_bounds > -1) { glUniform2f(u_bounds, m_bounds.size.x, m_bounds.size.y); }
    if (u_max_speed > -1) { glUniform1f(u_max_speed, Boid::maxSpeed); }
    if (u_max_force > -1) { glUniform1f(u_max_force, Boid::maxForce); }
    if (u_cohesive_radius > -1) { glUniform1f(u_cohesive_radius, Boid::cohesiveRadius); }
    if (u_disruptive_radius > -1) { glUniform1f(u_disruptive_radius, Boid::disruptiveRadius); }
}

DirectComputeAgent::DirectComputeAgent(const Rectangle bounds) : m_program_id(compile()), m_bounds(bounds) {
    const GLuint previous_program_id {get_current_program()};
    glUseProgram(m_program_id);
    // Error handle these.
    u_delta = glGetUniformLocation(m_program_id, "u_delta");

    // Leave these around for when we can change these at runtime.
    u_bounds = glGetUniformLocation(m_program_id, "u_bounds");
    u_max_speed = glGetUniformLocation(m_program_id, "u_max_speed");
    u_max_force = glGetUniformLocation(m_program_id, "u_max_force");
    u_cohesive_radius = glGetUniformLocation(m_program_id, "u_cohesive_radius");
    u_disruptive_radius = glGetUniformLocation(m_program_id, "u_disruptive_radius");
    if (previous_program_id != m_program_id) {
        glUseProgram(previous_program_id);
    }
}

DirectComputeAgent::~DirectComputeAgent() {
    if (get_current_program() == m_program_id) {
        glUseProgram(0);
    }
    glDeleteProgram(m_program_id);

    delete_buffer(m_write_buffer_id);
    delete_buffer(m_read_buffer_id);

    m_write = nullptr;
    m_read = nullptr;
}

void DirectComputeAgent::update(DoubleBuffer<Boid> &boids, const float delta) {
    const size_t count = boids.count();
    const size_t buffer_size = count * sizeof(Boid);
    const auto gl_buffer_size = static_cast<GLsizeiptr>(buffer_size);
    const Boid* read = boids.read();
    Boid* write = boids.write();

    if (count > m_flock_size) {
        resize_buffers(count);
    }

    // Copy CPU read buffer into GPU read buffer
    std::memcpy(m_read, read, buffer_size);
    glFlushMappedNamedBufferRange(m_read_buffer_id, 0, gl_buffer_size);
    glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);

    const GLuint previous_program_id = get_current_program();
    glUseProgram(m_program_id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_write_buffer_id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_read_buffer_id);
    write_uniforms(delta);
    glDispatchCompute(count, 1, 1);
    glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    GLsync compute_sync {glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0)};

    // Calculate other boid acceleration components
    //const Rectangle bounds {m_bounds};
    //const Rectangle center_bound {bounds * 0.75f};
    //const Rectangle hard_bound {bounds * 0.90f};
    //Rectangle boid_bound {Vector {Boid::scale}};
    //Rectangle search_bound {Vector {Boid::cohesiveRadius}};
    //for (ptrdiff_t i = 0; i < 0 + count; ++i) {
    //    Boid &current = write[i];
    //    const Boid *previous = read + i;
    //}

    // Wait for GPU to finish
    // Convert this to perform different actions based on milestones of time spent waiting.
    const GLenum sync_status {glClientWaitSync(compute_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 8000000)};
    if (sync_status == GL_CONDITION_SATISFIED || sync_status == GL_ALREADY_SIGNALED) {
        // Copy GPU write buffer into CPU write buffer
        std::memcpy(write, m_write, buffer_size);
    }

    // Restore the previous program.
    glUseProgram(previous_program_id);

    //for (size_t i = 0; i < count; ++i) {
    //    std::cout << m_write[i].position << '\n';
    //}
    //std::cout << delta << std::endl;
}
