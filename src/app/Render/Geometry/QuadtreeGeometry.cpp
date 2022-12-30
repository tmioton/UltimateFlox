#include "pch.hpp"
#include "QuadtreeGeometry.hpp"

void addToArray(QuadtreeVertex *array, int depth, Rectangle const& bound) {
    int quadrants[QuadtreeNodeVertexCount]{2, 3, 0, 0, 1, 2};
    for (int i = 0; i < QuadtreeNodeVertexCount; i++) {
        array[i].position = bound.center + bound.size * QuadrantOffsets[quadrants[i]];
        array[i].depth = depth;
    }
}
