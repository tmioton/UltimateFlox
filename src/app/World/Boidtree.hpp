#pragma once
#include "pch.hpp"
#include "Boid.hpp"
#include "Structures/Quadtree.hpp"


template<int depth_max, size_t bucket_max>
class Quadtree<const Boid*, depth_max, bucket_max> {
public:
    using Type = const Boid*;
    using ResultVector = std::vector<Boid>;
private:
    friend QuadtreeGeometry<Type, depth_max, bucket_max>;

    Rectangle m_bounds;
    qt_details::QuadtreeData<Type, bucket_max> m_data;

public:
    explicit Quadtree(Rectangle bounds) : m_bounds(bounds) {}

    bool insert(Type data, Vector position) {
        int depth = 0;
        size_t node_index = 0;
        Rectangle current_bound{m_bounds};
        if (!current_bound.contains(position)) {
            return false;
        }

        while (true) {
            if (!m_data.node_has_children(node_index)) {
                const ptrdiff_t bucket = m_data.node_bucket(node_index);
                // No children. See if we can fit.
                if (m_data.bucket_size(bucket) < bucket_max) {
                    // Add to the current point list.
                    m_data.add(bucket, data, position);
                    return true;
                } else {
                    if (depth < depth_max) {
                        m_data.subdivide(node_index, bucket, current_bound);
                        continue; // Do another loop to see where the new point can fit.
                    } else {
                        // Do not create children. Fill and create PointLists as needed.
                        m_data.new_linked_bucket(node_index);
                        continue;
                    }
                }
            } else {
                // Figure out which child node we're in.
                int quadrant = current_bound.quadrant(position);
                node_index = m_data.node_child(node_index, quadrant);
                current_bound.size = current_bound.size * 0.5f;
                current_bound.center += current_bound.size * QuadrantOffsets[quadrant];
                ++depth;
                continue;
            }
        }
    }

    // Needs a self parameter to perform and identity check before dereference copy
    void search(const Boid *self, Rectangle area, ResultVector &search_results) const {
        if (m_bounds.intersects(area)) {
            int32_t indices[depth_max + 1];
            indices[0] = 0;
            indices[1] = m_data.node_child(0, 0);
            Rectangle bounds[depth_max + 1];  // TODO: test storing just centers
            bounds[0] = Rectangle{m_bounds};
            if (!indices[1]) { return; }
            uint64_t quadrantState = 0;
            int depth = 1;
            bool ascended = false;

            // Possible to abstract this loop?
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

                    if (m_data.node_has_children(nodeIndex) && newBound.intersects(area)) {
                        indices[++depth] = m_data.node_child(nodeIndex, 0);
                        // Shift left 2 bits to go down
                        quadrantState <<= 2;
                        continue;
                    } else if (newBound.intersects(area)) {
                        // Bottom. Add my contents to the search.
                        if (m_data.node_bucket(nodeIndex) > -1 && m_data.bucket_size(m_data.node_bucket(nodeIndex)) > 0) {
                            ptrdiff_t index = m_data.node_bucket(nodeIndex);
                            qt_details::BucketList const *list = &m_data.lists.at(index);
                            while(true) {
                                for (size_t i = 0; i < list->size; ++i) {
                                    // Any way to make this branch-less?
                                    if (area.contains(m_data.position(index, i)) && m_data.data(index, i) != self) {
                                        search_results.push_back(*m_data.data(index, i));
                                    }
                                }

                                if (list->next == 0) {
                                    break;
                                }

                                index += list->next;
                                list = &m_data.lists.at(index);
                            }
                        }
                    }
                }

                ++quadrant;

                if (quadrant >= ChildCount) {
                    // Shift right 2 bits to go up
                    quadrantState >>= 2;
                    ascended = true;
                    --depth;

                    if (!depth) {
                        break;
                    }
                } else {
                    indices[depth] = m_data.node_child(indices[depth - 1], quadrant);
                    quadrantState += 1;
                    ascended = false;
                }
            }
        }
    }

    void clear() {
        m_data.clear();
    }

    [[nodiscard]] size_t size() const {
        return m_data.size();
    }

    Rectangle bounds() {
        return m_bounds;
    }

    void bounds(Rectangle bound) {
        m_bounds.center = bound.center;
        m_bounds.size = bound.size;
    }
};


//using Boidtree = Quadtree<ptrdiff_t, MaxDepth, BucketSize>;
//using Boidtree = Quadtree<Boid, MaxDepth, BucketSize>;
using Boidtree = Quadtree<const Boid*, MaxDepth, BucketSize>;
