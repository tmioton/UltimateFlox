module;
#include "pch.hpp"
export module QuadtreeRenderer;

import Quadtree;
import QuadtreeGeometry;
import Camera;

glm::vec4 lch_to_lab(glm::vec4 color) {
    const float a = glm::cos(glm::radians(color.b)) * color.g;
    const float b = glm::sin(glm::radians(color.b)) * color.g;

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

    constexpr float ref_X = 95.047f; //Observer= 2.0 degrees, Illuminant= D65
    constexpr float ref_Y = 100.000f;
    constexpr float ref_Z = 108.883f;

    float X = ref_X * var_X;
    float Y = ref_Y * var_Y;
    float Z = ref_Z * var_Z;

    return {X, Y, Z, color.a};
}

glm::vec4 xyz_to_rgb(glm::vec4 color) {
    const float var_X = color.r / 100.0f; //X from 0.0 to  95.047      (Observer = 2.0 degrees, Illuminant = D65);
    const float var_Y = color.g / 100.0f; //Y from 0.0 to 100.000;
    const float var_Z = color.b / 100.0f; //Z from 0.0 to 108.883;

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

/*
# Lch gradient
# https://davidjohnstone.net/lch-lab-colour-gradient-picker
# stops: 3
# rgb(49,233,128)
# rgb(255, 223, 102)
# rgb(195, 11, 77)
# interpolation power: 1.1

codes = ['#31e980', '#79e96e', '#aae761', '#d3e45d', '#f6e063', '#febb50', '#f58843', '#e25345', '#c30b4d']
get = lambda ccs: print(f"constexpr glm::vec4 DepthColors[{len(ccs)}] {{",",\n".join("    {%s1.0f}"%('{:.5f}f, '*3).format(*(int(c[i:i+2],16)/255for(i)in(1,3,5)))for c in ccs),"};",sep="\n")
*/

// Green to red 12
//constexpr glm::vec4 DepthColors[12] {
//    {0.91373f, 0.19216f, 0.61176f, 1.0f},
//    {0.96078f, 0.20000f, 0.49412f, 1.0f},
//    {0.97255f, 0.26275f, 0.38039f, 1.0f},
//    {0.95294f, 0.34118f, 0.27451f, 1.0f},
//    {0.90588f, 0.42353f, 0.18039f, 1.0f},
//    {0.84314f, 0.49412f, 0.09020f, 1.0f},
//    {0.76471f, 0.56078f, 0.01569f, 1.0f},
//    {0.67451f, 0.61569f, 0.05098f, 1.0f},
//    {0.57255f, 0.66275f, 0.15294f, 1.0f},
//    {0.46275f, 0.70588f, 0.26667f, 1.0f},
//    {0.32549f, 0.73725f, 0.38431f, 1.0f},
//    {0.04314f, 0.76471f, 0.50980f, 1.0f}
//};

// Blue 12
constexpr glm::vec4 DEPTH_COLORS[12] {
    {0.43137f, 0.64314f, 0.74902f, 1.0f},
    {0.40392f, 0.60000f, 0.70588f, 1.0f},
    {0.36863f, 0.55294f, 0.65490f, 1.0f},
    {0.33725f, 0.50196f, 0.60000f, 1.0f},
    {0.30196f, 0.45098f, 0.54510f, 1.0f},
    {0.26667f, 0.40000f, 0.49020f, 1.0f},
    {0.23137f, 0.34902f, 0.43529f, 1.0f},
    {0.19216f, 0.29804f, 0.38039f, 1.0f},
    {0.15686f, 0.24706f, 0.32549f, 1.0f},
    {0.12157f, 0.20000f, 0.27059f, 1.0f},
    {0.08627f, 0.15294f, 0.21569f, 1.0f},
    {0.05098f, 0.10588f, 0.16471f, 1.0f}
};

//Mesa 12
//constexpr glm::vec4 DepthColors[12] {
//    {0.94118f, 0.63529f, 0.00784f, 1.0f},
//    {0.95294f, 0.54902f, 0.14118f, 1.0f},
//    {0.94118f, 0.45490f, 0.23137f, 1.0f},
//    {0.89804f, 0.37255f, 0.30196f, 1.0f},
//    {0.82353f, 0.30588f, 0.36078f, 1.0f},
//    {0.72157f, 0.26275f, 0.40392f, 1.0f},
//    {0.60392f, 0.23922f, 0.42353f, 1.0f},
//    {0.47059f, 0.22745f, 0.41961f, 1.0f},
//    {0.33725f, 0.20784f, 0.38824f, 1.0f},
//    {0.21176f, 0.18431f, 0.32941f, 1.0f},
//    {0.11373f, 0.14902f, 0.25098f, 1.0f},
//    {0.05098f, 0.10588f, 0.16471f, 1.0f}
//};


export class QuadtreeRenderer {
public:
    explicit QuadtreeRenderer(Projection &proj) {
        m_layout.instances = 1;
        m_layout.array(m_vertices, 0, 0, sizeof(QuadtreeVertex));
        m_layout.attribute(0, 0, 2, lwvl::ByteFormat::Float, offsetof(QuadtreeVertex, position));
        m_layout.attribute(0, 1, 1, lwvl::ByteFormat::UnsignedInt, offsetof(QuadtreeVertex, depth));

        m_vertices.store<QuadtreeVertex>(nullptr, m_buffer_size, lwvl::bits::Dynamic);
        m_colors.store<glm::vec4>(DEPTH_COLORS, sizeof(DEPTH_COLORS));

        const lwvl::VertexShader vs = lwvl::VertexShader::fromFile("Data/Shaders/quadtree.vert");
        const lwvl::GeometryShader gs = lwvl::GeometryShader::fromFile("Data/Shaders/quadtree.geom");
        const lwvl::FragmentShader fs = lwvl::FragmentShader::fromFile("Data/Shaders/default.frag");

        m_lines_control.attach(vs);
        m_lines_control.attach(gs);
        m_lines_control.attach(fs);
        m_lines_control.link();
        m_lines_control.detach(vs);
        m_lines_control.detach(gs);
        m_lines_control.detach(fs);

        m_color_control.link(
            lwvl::VertexShader::fromFile("Data/Shaders/colorquadtree.vert"),
            lwvl::FragmentShader::fromFile("Data/Shaders/colorquadtree.frag")
        );

        m_lines_control.bind();
        u_lines_view = m_lines_control.uniform("view");
        m_lines_control.uniform("projection").matrix4F(&proj[0][0]);
        //m_linesControl.uniform("alpha").setF(0.1f);

        m_color_control.bind();
        u_color_view = m_color_control.uniform("view");
        m_color_control.uniform("projection").matrix4F(&proj[0][0]);
    }

    template<class T>
    void update(Quadtree<T> const &tree) {
        const int nodes = static_cast<int>(tree.size());
        const int vertex_count = nodes * static_cast<int>(QuadtreeNodeVertexCount);
        m_primitive_count = nodes * 2;
        m_vertex_data.resize(vertex_count);
        {
            QuadtreeGeometry<T> geometry {tree};
            geometry(m_vertex_data.data());
        }

        if (const auto buffer_size = static_cast<GLsizeiptr>(vertex_count * sizeof(QuadtreeVertex)); buffer_size > m_buffer_size) {
            // Create a new fixed-length buffer and set it as the array buffer of the layout.
            // This is probably how glCreateBuffer supports variable-length buffers behind the scenes.
            m_vertices = lwvl::Buffer();
            m_buffer_size = buffer_size + 1024; // 1024 bytes of headroom before growing again.
            m_vertices.store<QuadtreeVertex>(
                nullptr, static_cast<GLsizeiptr>(m_buffer_size), lwvl::bits::Dynamic
            );
            m_layout.array(m_vertices, 0, 0, sizeof(QuadtreeVertex));
        }

        m_vertices.update(m_vertex_data.begin(), m_vertex_data.end());
    }

    void update_camera(const Camera &view) {
        if (u_lines_view.location() > -1) {
            m_lines_control.bind();
            u_lines_view.matrix4F(view.data());
        }

        if (u_color_view.location() > -1) {
            m_color_control.bind();
            u_color_view.matrix4F(view.data());
        }
    }

    void draw(const bool draw_colors, const bool draw_lines) const {
        //m_control.draw(this, [](const void* user_ptr){
        //    const auto* renderer = static_cast<const QuadtreeRenderer*>(user_ptr);
        //    renderer->m_layout.drawElements(lwvl::PrimitiveMode::Triangles, renderer->m_primitiveCount * 3, lwvl::ByteFormat::UnsignedInt);
        //});

        if (draw_colors) {
            m_colors.bind(lwvl::Buffer::IndexedTarget::ShaderStorage, 0);
            m_color_control.bind();
            m_layout.drawArrays(lwvl::PrimitiveMode::Triangles, m_primitive_count * 3);
        }

        if (draw_lines) {
            m_lines_control.bind();
            m_layout.drawArrays(lwvl::PrimitiveMode::Triangles, m_primitive_count * 3);
        }
        lwvl::Program::clear();
    }

private:
    lwvl::Program m_lines_control;
    lwvl::Program m_color_control;
    lwvl::VertexArray m_layout;
    lwvl::Buffer m_vertices;
    lwvl::Buffer m_colors;
    lwvl::Uniform u_lines_view;
    lwvl::Uniform u_color_view;

    std::vector<QuadtreeVertex> m_vertex_data;

    int m_primitive_count = 0;
    GLsizeiptr m_buffer_size = 1024 * QuadtreeNodeVertexCount * sizeof(QuadtreeVertex);
};
