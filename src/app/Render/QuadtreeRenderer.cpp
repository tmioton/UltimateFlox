#include "pch.hpp"
#include "QuadtreeRenderer.hpp"

glm::vec4 lch_to_lab(glm::vec4 color) {
    float a = glm::cos(glm::radians(color.b)) * color.g;
    float b = glm::sin(glm::radians(color.b)) * color.g;

    return {color.r, a, b, color.a};
}

glm::vec4 lab_to_xyz(glm::vec4 color) {
    float var_Y = (color.r + 16.0f) / 116.0f;
    float var_X = color.g / 500.0f + var_Y;
    float var_Z = var_Y - color.b / 200.0f;

    if (glm::pow(var_Y, 3.0f) > 0.008856f) {
        var_Y = glm::pow(var_Y, 3.0f);
    } else {
        var_Y = (var_Y - 16.0f / 116.0f) / 7.787f;
    }
    if (glm::pow(var_X, 3.0f) > 0.008856f) {
        var_X = glm::pow(var_X, 3.0f);
    } else {
        var_X = (var_X - 16.0f / 116.0f) / 7.787f;
    }
    if (glm::pow(var_Z, 3.0f) > 0.008856f) {
        var_Z = glm::pow(var_Z, 3.0f);
    } else {
        var_Z = (var_Z - 16.0f / 116.0f) / 7.787f;
    }

    float ref_X = 95.047f; //Observer= 2.0 degrees, Illuminant= D65
    float ref_Y = 100.000f;
    float ref_Z = 108.883f;

    float X = ref_X * var_X;
    float Y = ref_Y * var_Y;
    float Z = ref_Z * var_Z;

    return {X, Y, Z, color.a};
}

glm::vec4 xyz_to_rgb(glm::vec4 color) {
    float var_X = color.r / 100.0f; //X from 0.0 to  95.047      (Observer = 2.0 degrees, Illuminant = D65);
    float var_Y = color.g / 100.0f; //Y from 0.0 to 100.000;
    float var_Z = color.b / 100.0f; //Z from 0.0 to 108.883;

    float var_R = var_X * 3.2406f + var_Y * -1.5372f + var_Z * -0.4986f;
    float var_G = var_X * -0.9689f + var_Y * 1.8758f + var_Z * 0.0415f;
    float var_B = var_X * 0.0557f + var_Y * -0.2040f + var_Z * 1.0570f;

    if (var_R > 0.0031308f) {
        var_R = 1.055f * pow(var_R, (1.0f / 2.4f)) - 0.055f;
    } else {
        var_R = 12.92f * var_R;
    }
    if (var_G > 0.0031308f) {
        var_G = 1.055f * pow(var_G, (1.0f / 2.4f)) - 0.055f;
    } else {
        var_G = 12.92f * var_G;
    }
    if (var_B > 0.0031308f) {
        var_B = 1.055f * pow(var_B, (1.0f / 2.4f)) - 0.055f;
    } else {
        var_B = 12.92f * var_B;
    }

    float R = var_R;
    float G = var_G;
    float B = var_B;

    return {R, G, B, color.a};
}

QuadtreeRenderer::QuadtreeRenderer(Projection &proj) {
    m_layout.instances = 1;
    m_layout.array(m_vertices, 0, 0, sizeof(QuadtreeVertex));
    m_layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, offsetof(QuadtreeVertex, position));
    m_layout.attribute(0, 1, 1, lwvl::ByteFormat::UnsignedInt, offsetof(QuadtreeVertex, depth));

    m_vertices.store<QuadtreeVertex>(nullptr, m_bufferSize, lwvl::bits::Dynamic);
    m_colors.store<glm::vec4>(DepthColors, sizeof(DepthColors));

    lwvl::VertexShader vs = lwvl::VertexShader::fromFile("Data/Shaders/quadtree.vert");
    lwvl::GeometryShader gs = lwvl::GeometryShader::fromFile("Data/Shaders/quadtree.geom");
    lwvl::FragmentShader fs = lwvl::FragmentShader::fromFile("Data/Shaders/default.frag");

    m_linesControl.attach(vs);
    m_linesControl.attach(gs);
    m_linesControl.attach(fs);
    m_linesControl.link();
    m_linesControl.detach(vs);
    m_linesControl.detach(gs);
    m_linesControl.detach(fs);

    m_colorControl.link(
        lwvl::VertexShader::fromFile("Data/Shaders/colorquadtree.vert"),
        lwvl::FragmentShader::fromFile("Data/Shaders/colorquadtree.frag")
    );

    m_linesControl.bind();
    m_linesControl.uniform("projection").matrix4F(&proj[0][0]);
    //m_linesControl.uniform("alpha").setF(0.1f);

    m_colorControl.bind();
    m_colorControl.uniform("projection").matrix4F(&proj[0][0]);
}

void QuadtreeRenderer::draw(bool drawColors, bool drawLines) const {
    //m_control.draw(this, [](const void* user_ptr){
    //    const auto* renderer = static_cast<const QuadtreeRenderer*>(user_ptr);
    //    renderer->m_layout.drawElements(lwvl::PrimitiveMode::Triangles, renderer->m_primitiveCount * 3, lwvl::ByteFormat::UnsignedInt);
    //});

    if (drawColors) {
        m_colors.bind(lwvl::Buffer::IndexedTarget::ShaderStorage, 0);
        m_colorControl.bind();
        m_layout.drawArrays(lwvl::PrimitiveMode::Triangles, m_primitiveCount * 3);
    }

    if (drawLines) {
        m_linesControl.bind();
        m_layout.drawArrays(lwvl::PrimitiveMode::Triangles, m_primitiveCount * 3);
    }
    lwvl::Program::clear();
}
