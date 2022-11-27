#pragma once

#include "pch.hpp"
#include "Controls.hpp"
#include "Structures/Quadtree.hpp"


class QuadtreeRenderer {
public:
    explicit QuadtreeRenderer(Projection &);
    void update(Boidtree const &);
    void draw() const;

private:
    lwvl::Program m_control;
    lwvl::VertexArray m_layout;
    lwvl::Buffer m_vertices;

    int m_primitiveCount = 0;
    ptrdiff_t m_bufferSize = 0;
};
