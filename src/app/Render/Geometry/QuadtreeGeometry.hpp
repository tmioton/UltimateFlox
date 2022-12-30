#pragma once
#include "pch.hpp"
#include "Geometry.hpp"
#include "Structures/Quadtree.hpp"

static constexpr size_t QuadtreeNodeVertexCount = 6;

struct QuadtreeVertex {
    Vector position;
    int depth;
};

void addToArray(QuadtreeVertex *array, int depth, Rectangle const &bound);

template<typename T, int max_depth, size_t bucket_size = 4>
class QuadtreeGeometry : public Geometry {
    Quadtree<T, max_depth, bucket_size> const& m_tree;
public:
    explicit QuadtreeGeometry(Quadtree<T, max_depth, bucket_size> const &tree) : m_tree(tree) {}
    ~QuadtreeGeometry() override = default;
    void operator()(void* in) override {
        // I can imagine dfs should work backwards from quadrants 4 to 1, but the ordering of the data probably doesn't matter here?
        // Maybe working backwards from insertion order has better cache performance.
        auto array = static_cast<QuadtreeVertex*>(in);

        size_t indices[max_depth + 1];
        indices[0] = 0;
        indices[1] = m_tree.m_nodes[0].children[0];
        Rectangle bounds[max_depth + 1];
        bounds[0] = Rectangle{m_tree.m_bounds};
        addToArray(array, 0, bounds[0]);
        if (!indices[1]) { return; }
        uint64_t quadrantState = 0;
        int depth = 1;
        int count = 1;
        bool ascended = false;

        while (true) {
            // The last 2 bits of the state are the current quadrant
            uint8_t quadrant = quadrantState & 0b11;
            const size_t nodeIndex = indices[depth];

            // Critical operations happen when descending the tree
            if (!ascended) {
                Rectangle newBound{bounds[depth - 1]};
                newBound.size = newBound.size * 0.5f;
                newBound.center = newBound.center + newBound.size * QuadrantOffsets[quadrant];
                bounds[depth] = newBound;
                addToArray(array + count++ * QuadtreeNodeVertexCount, depth, newBound);

                if (*m_tree.m_nodes[nodeIndex].children) {
                    indices[++depth] = *m_tree.m_nodes[nodeIndex].children;
                    // Shift left 2 bits to go down
                    quadrantState <<= 2;
                    continue;
                }
            }

            ++quadrant;
            indices[depth] = m_tree.m_nodes[indices[depth - 1]].children[quadrant];

            if (quadrant >= ChildCount) {
                // Shift right 2 bits to go up
                quadrantState >>= 2;
                ascended = true;
                --depth;

                if (!depth) {
                    break;
                }
            } else {
                quadrantState += 1;
                ascended = false;
            }
        }
    }
};
