module;
#include "pch.hpp"
export module QuadtreeGeometry;

import Rectangle;
import Quadtree;
import Geometry;

export constexpr size_t QuadtreeNodeVertexCount = 6;

export struct QuadtreeVertex {
    Vector position;
    uint32_t depth;
};

export void addToArray(QuadtreeVertex *array, size_t depth, Rectangle const &bound) {
    constexpr int quadrants[QuadtreeNodeVertexCount] {2, 3, 0, 0, 1, 2};
    for (int i = 0; i < QuadtreeNodeVertexCount; i++) {
        array[i].position = bound.center + bound.size * QuadrantOffsets[quadrants[i]];
        array[i].depth = static_cast<uint32_t>(depth);
    }
}


export template<typename T>
class QuadtreeGeometry final : public Geometry {
    using QuadtreeType = Quadtree<T>;
    QuadtreeType const &m_tree;
public:
    explicit QuadtreeGeometry(QuadtreeType const &tree) : m_tree(tree) {}

    ~QuadtreeGeometry() override = default;

    void operator()(void *in) override {
        // I can imagine dfs should work backwards from quadrants 4 to 1, but the ordering of the data probably doesn't matter here?
        auto array = static_cast<QuadtreeVertex *>(in);

        size_t indices[QuadtreeType::MaxDepth + 1];
        indices[0] = 0;
        indices[1] = m_tree.node_child(0, 0);
        Rectangle bounds[QuadtreeType::MaxDepth + 1];
        bounds[0] = Rectangle {m_tree.bounds};
        addToArray(array, 0, bounds[0]);
        if (!indices[1]) { return; }
        uint64_t quadrant_memory = 0;
        size_t depth = 1;
        size_t count = 1;
        bool ascended = false;

        while (true) {
            // The last 2 bits of the state are the current quadrant
            uint8_t quadrant = quadrant_memory & 0b11;
            const size_t node_index = indices[depth];

            // Critical operations happen when descending the tree
            if (!ascended) {
                Rectangle newBound {bounds[depth - 1]};
                newBound.size = newBound.size * 0.5f;
                newBound.center = newBound.center + newBound.size * QuadrantOffsets[quadrant];
                bounds[depth] = newBound;
                addToArray(array + count++ * QuadtreeNodeVertexCount, depth, newBound);

                if (m_tree.node_has_children(node_index)) {
                    indices[++depth] = m_tree.node_child(node_index, 0);
                    // Shift left 2 bits to go down
                    quadrant_memory <<= 2;
                    continue;
                }
            }

            ++quadrant;

            if (quadrant >= QuadtreeChildCount) {
                // Shift right 2 bits to go up
                quadrant_memory >>= 2;
                ascended = true;
                --depth;

                if (!depth) {
                    break;
                }
            } else {
                indices[depth] = m_tree.node_child(indices[depth - 1], quadrant);
                quadrant_memory += 1;
                ascended = false;
            }
        }
    }
};

