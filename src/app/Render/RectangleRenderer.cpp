#include "pch.hpp"
#include "RectangleRenderer.hpp"


RectanglePrimitive::RectanglePrimitive(Rectangle r) : rectangle(r) {}

RectanglePrimitive::RectanglePrimitive(Rectangle r, Color c) : rectangle(r), color(c) {}


RectangleLink::RectangleLink(RectangleRenderer *renderer, UnsignedInt id) :
    m_renderer(renderer), m_id(id) {}

RectanglePrimitive &RectangleLink::get() {
    return m_renderer->m_data[m_id];
}


RectangleRenderer::RectangleRenderer(const Projection &projection, SignedInt initial_size) :
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

RectangleRenderer::~RectangleRenderer() {
    glUnmapNamedBuffer(m_instance_buffer.id());
}

RectangleLink RectangleRenderer::push(Rectangle r, Color c) {
    const UnsignedInt new_id = m_data.size();
    m_data.emplace_back(r, c);
    m_layout.instances++;
    return {this, new_id};
}

RectangleLink RectangleRenderer::push(Rectangle r) {
    const UnsignedInt new_id = m_data.size();
    m_data.emplace_back(r);
    m_layout.instances++;
    return {this, new_id};
}

RectangleLink RectangleRenderer::push() {
    const UnsignedInt new_id = m_data.size();
    m_data.emplace_back();
    m_layout.instances++;
    return {this, new_id};
}

void RectangleRenderer::update() {
    const auto count = m_data.size();
    const auto buffer_size = static_cast<SignedInt>(count * sizeof(RectangleInstance));
    if (buffer_size > m_buffer_size) {
        // recreate buffer
    }

    for (UnsignedInt i = 0; i < count; ++i) {
        // for each rectangle primitive, extract its components and place them in the buffer.
        RectangleInstance &instance = m_mapped_buffer[i];
        const RectanglePrimitive &primitive = m_data[i];
        instance.center = primitive.rectangle.center;
        instance.size = primitive.rectangle.size;
        instance.color = primitive.color;
    }

    glFlushMappedNamedBufferRange(m_instance_buffer.id(), 0, buffer_size);
    glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
}

void RectangleRenderer::update_camera(const Camera &view) {
    if (u_view.exists()) {
        m_control.bind();
        u_view.matrix4F(view.data());
    }
}

void RectangleRenderer::draw() const {
    m_control.bind();
    m_layout.drawArrays(lwvl::PrimitiveMode::LineLoop, 4);
    lwvl::Program::clear();
}
