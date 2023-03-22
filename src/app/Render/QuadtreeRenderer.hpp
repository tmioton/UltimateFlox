// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once

#include "pch.hpp"
#include "Structures/Quadtree.hpp"
#include "Geometry/QuadtreeGeometry.hpp"
#include "Math/Camera.hpp"


glm::vec4 lch_to_lab(glm::vec4 color);
glm::vec4 lab_to_xyz(glm::vec4 color);
glm::vec4 xyz_to_rgb(glm::vec4 color);

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


class QuadtreeRenderer {
public:
    explicit QuadtreeRenderer(Projection &);

    template<class T>
    void update(structures::Quadtree<T> const &tree) {
        int nodes = static_cast<int>(tree.size());
        int vertex_count = nodes * static_cast<int>(QuadtreeNodeVertexCount);
        m_primitive_count = nodes * 2;
        m_vertex_data.resize(vertex_count);
        {
            QuadtreeGeometry<T> geometry {tree};
            geometry(m_vertex_data.data());
        }

        auto buffer_size = static_cast<GLsizeiptr>(vertex_count * sizeof(QuadtreeVertex));
        if (buffer_size > m_buffer_size) {
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

    void update_camera(const Camera &view);

    void draw(bool draw_colors = false, bool draw_lines = false) const;

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
