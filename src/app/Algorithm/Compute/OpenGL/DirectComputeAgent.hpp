#pragma once

#include "pch.hpp"
#include "Algorithm/Compute/ComputeAgent.hpp"
#include "Math/Rectangle.hpp"


// How do we parallelize bird calculations?
// We can do multiple compute shaders if needed.
// Totals have to be accumulated separately because of division but can be added to acceleration in any order.
// Separation, alignment, and cohesion are the only ones we need to do in parallel. Rest can be done in a for-loop.
// First, get just one of the behaviors parallelized.


class DirectComputeAgent final : public ComputeAgent {
    static void delete_buffer(GLuint id);
    static GLuint get_current_program();
    static void validate_shader(GLuint id);
    static void validate_program(GLuint id, GLenum stage);
    static GLuint compile();
    static std::string read_file(const char* path);

    void resize_buffers(std::size_t count);
    void write_uniforms(float delta) const;
public:
    explicit DirectComputeAgent(Rectangle bounds);
    ~DirectComputeAgent() override;
    void update(DoubleBuffer<Boid> &boids, float delta) override;
private:
    GLuint m_program_id;

    // Get created on first update.
    GLuint m_write_buffer_id {0};
    GLuint m_read_buffer_id {0};

    size_t m_flock_size {0};
    const Boid *m_write {nullptr};
    Boid *m_read {nullptr};
    Rectangle m_bounds;

    GLint u_delta;
    GLint u_bounds;
    GLint u_max_speed;
    GLint u_max_force;
    GLint u_cohesive_radius;
    GLint u_disruptive_radius;
};
