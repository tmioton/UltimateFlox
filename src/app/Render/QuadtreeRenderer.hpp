#pragma once

#include "pch.hpp"
#include "Structures/Quadtree.hpp"
#include "Geometry/QuadtreeGeometry.hpp"

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
get = lambda ccs: print(f"constexpr glm::vec4 DepthColors[{len(codes)}] {{",",\n".join("    {%s1.0f}"%('{:.5f}f, '*3).format(*(int(c[i:i+2],16)/255for(i)in(1,3,5)))for c in ccs),"};",sep="\n")
*/

//constexpr glm::vec4 DepthColors[9] {
//    {0.19216f, 0.91373f, 0.50196f, 1.0f},
//    {0.47451f, 0.91373f, 0.43137f, 1.0f},
//    {0.66667f, 0.90588f, 0.38039f, 1.0f},
//    {0.82745f, 0.89412f, 0.36471f, 1.0f},
//    {0.96471f, 0.87843f, 0.38824f, 1.0f},
//    {0.99608f, 0.73333f, 0.31373f, 1.0f},
//    {0.96078f, 0.53333f, 0.26275f, 1.0f},
//    {0.88627f, 0.32549f, 0.27059f, 1.0f},
//    {0.76471f, 0.04314f, 0.30196f, 1.0f}
//};

constexpr glm::vec4 DepthColors[12] {
    {0.19216f, 0.91373f, 0.50196f, 1.0f},
    {0.41176f, 0.91373f, 0.45098f, 1.0f},
    {0.56471f, 0.90980f, 0.40392f, 1.0f},
    {0.69412f, 0.90588f, 0.37255f, 1.0f},
    {0.81176f, 0.89412f, 0.36078f, 1.0f},
    {0.91765f, 0.88627f, 0.37255f, 1.0f},
    {1.00000f, 0.85490f, 0.38431f, 1.0f},
    {0.99216f, 0.71373f, 0.30588f, 1.0f},
    {0.96863f, 0.57255f, 0.26667f, 1.0f},
    {0.92549f, 0.42353f, 0.26275f, 1.0f},
    {0.85882f, 0.26275f, 0.27843f, 1.0f},
    {0.76471f, 0.04314f, 0.30196f, 1.0f}
};

//constexpr glm::vec4 DepthColors[12] {
//    {0.43137f, 0.64314f, 0.74902f, 1.0f},
//    {0.40392f, 0.60000f, 0.70588f, 1.0f},
//    {0.36863f, 0.55294f, 0.65490f, 1.0f},
//    {0.33725f, 0.50196f, 0.60000f, 1.0f},
//    {0.30196f, 0.45098f, 0.54510f, 1.0f},
//    {0.26667f, 0.40000f, 0.49020f, 1.0f},
//    {0.23137f, 0.34902f, 0.43529f, 1.0f},
//    {0.19216f, 0.29804f, 0.38039f, 1.0f},
//    {0.15686f, 0.24706f, 0.32549f, 1.0f},
//    {0.12157f, 0.20000f, 0.27059f, 1.0f},
//    {0.08627f, 0.15294f, 0.21569f, 1.0f},
//    {0.05098f, 0.10588f, 0.16471f, 1.0f}
//};

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

    template<typename T, int max_depth, size_t bucket_size>
    void update(Quadtree<T, max_depth, bucket_size> const &tree) {
        int nodes = static_cast<int>(tree.size());
        int vertexCount = nodes * static_cast<int>(QuadtreeNodeVertexCount);
        m_primitiveCount = nodes * 2;
        m_vertexData.resize(vertexCount); {
            QuadtreeGeometry<T, max_depth, bucket_size> geometry{tree};
            geometry(m_vertexData.data());
        }

        GLsizeiptr bufferSize = vertexCount * sizeof(QuadtreeVertex);
        m_vertices.update(m_vertexData.data(), bufferSize);
    }

    void draw(bool drawColors = false, bool drawLines = false) const;

private:
    lwvl::Program m_linesControl;
    lwvl::Program m_colorControl;
    lwvl::VertexArray m_layout;
    lwvl::Buffer m_vertices;
    lwvl::Buffer m_colors;

    std::vector<QuadtreeVertex> m_vertexData;

    int m_primitiveCount = 0;
    ptrdiff_t m_bufferSize = 0;
};
