#include "QuadtreeRenderer.hpp"

QuadtreeRenderer::QuadtreeRenderer(Projection &proj) {
    m_layout.instances = 1;
    m_layout.array(m_vertices, 0, 0, 2 * sizeof(float));
    m_layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, 0);

    lwvl::VertexShader vs = lwvl::VertexShader::fromFile("Data/Shaders/quadtree.vert");
    lwvl::GeometryShader gs = lwvl::GeometryShader::fromFile("Data/Shaders/quadtree.geom");
    lwvl::FragmentShader fs = lwvl::FragmentShader::fromFile("Data/Shaders/default.frag");

    m_control.attach(vs);
    m_control.attach(gs);
    m_control.attach(fs);
    m_control.link();
    m_control.detach(vs);
    m_control.detach(gs);
    m_control.detach(fs);

    m_control.bind();
    m_control.uniform("projection").matrix4F(&proj[0][0]);
    m_vertices.store<float>(nullptr, std::numeric_limits<uint16_t>::max() * 12 * sizeof(float), lwvl::bits::Dynamic);
}

void QuadtreeRenderer::update(Boidtree const &tree) {
    int nodes = static_cast<int>(tree.size());
    int vertexCount = nodes * 6;
    m_primitiveCount = nodes * 2;
    auto vertexData = new float[vertexCount * 2];
    tree.primitives(vertexData);

    ptrdiff_t bufferSize = vertexCount * 2 * sizeof(float);
    m_vertices.update(vertexData, bufferSize);
    delete[] vertexData;
}

void QuadtreeRenderer::draw() const {
    //m_control.draw(this, [](const void* user_ptr){
    //    const auto* renderer = static_cast<const QuadtreeRenderer*>(user_ptr);
    //    renderer->m_layout.drawElements(lwvl::PrimitiveMode::Triangles, renderer->m_primitiveCount * 3, lwvl::ByteFormat::UnsignedInt);
    //});

    m_control.bind();
    m_layout.drawArrays(lwvl::PrimitiveMode::Triangles, m_primitiveCount * 3);
    lwvl::Program::clear();
}
