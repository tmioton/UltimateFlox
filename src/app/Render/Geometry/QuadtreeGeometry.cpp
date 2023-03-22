// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "QuadtreeGeometry.hpp"


void addToArray(QuadtreeVertex *array, size_t depth, Rectangle const &bound) {
    int quadrants[QuadtreeNodeVertexCount] {2, 3, 0, 0, 1, 2};
    for (int i = 0; i < QuadtreeNodeVertexCount; i++) {
        array[i].position = bound.center + bound.size * structures::QuadrantOffsets[quadrants[i]];
        array[i].depth = static_cast<uint32_t>(depth);
    }
}
