module;
#include "pch.hpp"
export module RectangleRenderer;

import Rectangle;
import Camera;


export class RectangleRenderer;


struct RectanglePrimitive {
    Rectangle rectangle {};
    Color color {1.0f, 1.0f, 1.0f, 1.0f};

    RectanglePrimitive() = default;
    explicit RectanglePrimitive(const Rectangle r) : rectangle(r) {}
    RectanglePrimitive(const Rectangle r, const Color c) : rectangle(r), color(c) {}
};


class RectangleLink {
    friend RectangleRenderer;
    RectangleLink(RectangleRenderer *renderer, const UnsignedInt id) : m_renderer(renderer), m_id(id) {}
public:
    [[nodiscard]] RectanglePrimitive &get() const;

private:
    RectangleRenderer *m_renderer;
    UnsignedInt m_id;
};


export class RectangleRenderer {
public:
    struct RectangleInstance {
        Vector center;
        Vector size;
        Color color;
    };

    friend RectangleLink;

    RectangleRenderer(const Projection &projection, const SignedInt initial_size) :
    m_buffer_size(initial_size * static_cast<SignedInt>(sizeof(RectangleInstance)))
    {
        m_data.reserve(initial_size);

        m_layout.instances = 0;
        m_layout.array(m_model, 0, 0, 2 * sizeof(float));
        m_layout.array(m_instance_buffer, 1, 0, sizeof(RectangleInstance));
        m_layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, 0);
        m_layout.attribute(1, 1, 2, lwvl::ByteFormat::Float, offsetof(RectangleInstance, center));
        m_layout.attribute(1, 2, 2, lwvl::ByteFormat::Float, offsetof(RectangleInstance, size));
        m_layout.attribute(1, 3, 4, lwvl::ByteFormat::Float, offsetof(RectangleInstance, color));
        m_layout.divisor(1, 1);

        constexpr float model_data[8] {
            -1.0f, -1.0f,
            +1.0f, -1.0f,
            +1.0f, +1.0f,
            //+1.0f, +1.0f,
            -1.0f, +1.0f,
            //-1.0f, -1.0f
        };

        m_model.store(model_data, sizeof(model_data));
        m_instance_buffer.store<RectangleInstance>(
            nullptr, m_buffer_size,
            GL_MAP_WRITE_BIT
            | GL_MAP_PERSISTENT_BIT
            | GL_CLIENT_STORAGE_BIT
        );

        m_mapped_buffer = reinterpret_cast<RectangleInstance*>(glMapNamedBufferRange(
            m_instance_buffer.id(), 0, m_buffer_size,
            GL_MAP_WRITE_BIT
            | GL_MAP_PERSISTENT_BIT
            | GL_MAP_FLUSH_EXPLICIT_BIT
            | GL_MAP_INVALIDATE_BUFFER_BIT
        ));

        lwvl::VertexShader vs {lwvl::VertexShader::fromFile("Data/Shaders/rectangle.vert")};
        lwvl::FragmentShader fs {lwvl::FragmentShader::fromFile("Data/Shaders/rectangle.frag")};

        m_control.link(vs, fs);
        m_control.bind();
        m_control.uniform("projection").matrix4F(&projection[0][0]);
        u_view = m_control.uniform("view");
    }

    ~RectangleRenderer() {
        glUnmapNamedBuffer(m_instance_buffer.id());
    }

    RectangleLink push(const Rectangle r, const Color c) {
        const UnsignedInt new_id = m_data.size();
        m_data.emplace_back(r, c);
        m_layout.instances++;
        return {this, new_id};
    }

    RectangleLink push(const Rectangle r) {
        const UnsignedInt new_id = m_data.size();
        m_data.emplace_back(r);
        m_layout.instances++;
        return {this, new_id};
    }

    RectangleLink push() {
        const UnsignedInt new_id = m_data.size();
        m_data.emplace_back();
        m_layout.instances++;
        return {this, new_id};
    }

    void update() const {
        const auto count = m_data.size();
        const auto buffer_size = static_cast<SignedInt>(count * sizeof(RectangleInstance));
        if (buffer_size > m_buffer_size) {
            // recreate buffer
        }

        for (UnsignedInt i = 0; i < count; ++i) {
            // for each rectangle primitive, extract its components and place them in the buffer.
            auto & [center, size, color] = m_mapped_buffer[i];
            const RectanglePrimitive &primitive = m_data[i];
            center = primitive.rectangle.center;
            size = primitive.rectangle.size;
            color = primitive.color;
        }

        glFlushMappedNamedBufferRange(m_instance_buffer.id(), 0, buffer_size);
        glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
    }

    void update_camera(const Camera &view) {
        if (u_view.exists()) {
            m_control.bind();
            u_view.matrix4F(view.data());
        }
    }

    void draw() const {
        m_control.bind();
        m_layout.drawArrays(lwvl::PrimitiveMode::LineLoop, 4);
        lwvl::Program::clear();
    }
private:
    lwvl::Program m_control;
    lwvl::VertexArray m_layout;
    lwvl::Buffer m_instance_buffer;
    lwvl::Buffer m_model;
    lwvl::Uniform u_view;

    RectangleInstance* m_mapped_buffer;
    std::vector<RectanglePrimitive> m_data;

    SignedInt m_buffer_size;
};


RectanglePrimitive& RectangleLink::get() const {
    return m_renderer->m_data[m_id];
}
