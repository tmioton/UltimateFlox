#pragma once

#include "pch.hpp"
#include "Math/Rectangle.hpp"

// Generational algorithm to learn which bucket size works best for a number of birds.
// Sort boid arrays to have boids in the same cache line as the boids they access most often.
// Save the maximum and minimum boid positions every frame and resize the bounds of the tree to fit every bird?

// Just here to avoid seeing a hardcoded "4" and brainlessly changing it to "bucketSize"
static constexpr size_t ChildCount = 4;

static constexpr size_t VertexCount = 6;
static constexpr Vector QuadrantOffsets[ChildCount] {{1.0f, 1.0f}, {-1.0f, 1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f}};

namespace qt_details {
    template<typename T, size_t bucketSize>
    class Bucket {
        T m_data[bucketSize]{};
    public:
        Bucket() = default;

        Bucket(Bucket const &lhs) {
            for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(bucketSize); ++i) {
                m_data[i] = lhs.m_data[i];
            }
        }

        T const &get(size_t index) const {
            return m_data[index];
        }

        T &operator[](size_t index) {
            return m_data[index];
        }
    };

    struct BucketList {
        int32_t next = 0; // offset to next bucket
        uint32_t size = 0;
    };

    struct Node {
        int32_t children[ChildCount]{0, 0, 0, 0};  // Since node 0 can never be a child, 0 is safe to use as a sentry.
        int32_t bucketIndex = -1;
    };
}

// How do we move rendering specifics out of this file?
static void addToArray(float *first, Rectangle const &bound) {
    auto actual = reinterpret_cast<Vector *>(first);
    int quadrants[VertexCount]{2, 3, 0, 0, 1, 2};
    for (int i = 0; i < VertexCount; i++) {
        actual[i] = bound.center + bound.size * QuadrantOffsets[quadrants[i]];
    }
}


template<typename T, int max_depth, size_t bucketSize = 4>
class Quadtree {
    using BucketList = qt_details::BucketList;
    using Bucket = qt_details::Bucket<T, bucketSize>;
    using Points = qt_details::Bucket<Vector, bucketSize>;
    using Node = qt_details::Node;
public:
    using ResultVector = std::vector<T>;
private:

    Rectangle m_bounds;
    std::vector<BucketList> m_lists;
    std::vector<Bucket> m_buckets;
    std::vector<Points> m_points;  // Potentially unnecessary
    std::vector<Node> m_nodes;

    inline void createBucket() {
        m_lists.emplace_back();
        m_buckets.emplace_back();
        m_points.emplace_back();
    }

    inline void addPoint(int point, T data, Vector position) {
        int pointCount = m_lists[point].size++;
        m_buckets[point][pointCount] = data;
        m_points[point][pointCount] = position;
    }

public:

    explicit Quadtree(Rectangle bounds) : m_bounds(bounds) {
        m_nodes.emplace_back();
        createBucket();
        m_nodes[0].bucketIndex = 0;
    }

    bool insert(T data, Vector position) {
        int depth = 0;
        int nodeIndex = 0;
        Rectangle currentBound{m_bounds};
        if (!currentBound.contains(position)) {
            return false;
        }

        while (true) {
            if (!*m_nodes[nodeIndex].children) {
                const int bucket = m_nodes[nodeIndex].bucketIndex;
                // No children. See if we can fit.
                if (m_lists[bucket].size < bucketSize) {
                    // Add to the current point list.
                    addPoint(bucket, data, position);
                    return true;
                } else {
                    if (depth < max_depth) {
                        // Create children.
                        for (int i = 0; i < ChildCount; i++) {
                            int newNodeIndex = m_nodes.size();
                            m_nodes.emplace_back();
                            m_nodes[nodeIndex].children[i] = newNodeIndex;
                            if (i) {
                                m_nodes[newNodeIndex].bucketIndex = m_buckets.size();
                                createBucket();
                            } else {
                                m_nodes[newNodeIndex].bucketIndex = bucket;
                            }
                        }

                        // Reassign points. When not at max depth we can assume number of points is always bucketSize;
                        Bucket currentBucket{m_buckets[bucket]};
                        Points currentPoints{m_points[bucket]};
                        m_lists[bucket].size = 0;

                        for (int i = 0; i < static_cast<int>(bucketSize); i++) {
                            Vector pointPosition = currentPoints[i];
                            int quadrant = currentBound.quadrant(pointPosition);
                            //addPoint(currentNode->children[quadrant], currentBucket[i], pointPosition);
                            addPoint(
                                m_nodes[m_nodes[nodeIndex].children[quadrant]].bucketIndex, currentBucket[i],
                                pointPosition
                            );
                        }

                        m_nodes[nodeIndex].bucketIndex = -1;
                        continue;

                        // Do another loop to see where the new point can fit.
                    } else {
                        // Do not create children. Fill and create PointLists as needed.
                        // Create a new bucket
                        // Set that bucket as the node's bucket
                        // Have the BucketList point backwards to the previous bucket
                        int32_t newBucketIndex = m_buckets.size();
                        createBucket();

                        m_lists[newBucketIndex].next = m_nodes[nodeIndex].bucketIndex - newBucketIndex;
                        m_nodes[nodeIndex].bucketIndex = newBucketIndex;
                        continue;
                    }
                }
            } else {
                // Figure out which child node we're in.
                int quadrant = currentBound.quadrant(position);
                nodeIndex = m_nodes[nodeIndex].children[quadrant];
                currentBound.size = currentBound.size * 0.5f;
                currentBound.center += currentBound.size * QuadrantOffsets[quadrant];
                ++depth;
                continue;
            }
        }
    }

    void search(Rectangle area, ResultVector &search_results) const {
        if (m_bounds.intersects(area)) {
            int32_t indices[max_depth + 1];
            indices[0] = 0;
            indices[1] = m_nodes[0].children[0];
            Rectangle bounds[max_depth + 1];  // TODO: test storing just centers
            bounds[0] = Rectangle{m_bounds};
            if (!indices[1]) { return; }
            uint64_t quadrantState = 0;
            int depth = 1;
            bool ascended = false;

            // Possible to abstract this loop?
            while (true) {
                // The last 2 bits of the state are the current quadrant
                uint8_t quadrant = quadrantState & 0b11;
                int nodeIndex = indices[depth];

                // Critical operations happen when descending the tree
                if (!ascended) {
                    Rectangle newBound{bounds[depth - 1]};
                    newBound.size = newBound.size * 0.5f;
                    newBound.center = newBound.center + newBound.size * QuadrantOffsets[quadrant];
                    bounds[depth] = newBound;

                    if (*m_nodes[nodeIndex].children && newBound.intersects(area)) {
                        indices[++depth] = *m_nodes[nodeIndex].children;
                        // Shift left 2 bits to go down
                        quadrantState <<= 2;
                        continue;
                    } else if (newBound.intersects(area)) {
                        // Bottom. Add my contents to the search.
                        if (m_nodes[nodeIndex].bucketIndex > -1 && m_lists[m_nodes[nodeIndex].bucketIndex].size > 0) {
                            int index = m_nodes[nodeIndex].bucketIndex;
                            BucketList const *list = &m_lists[index];
                            while (true) {
                                for (int i = 0; i < list->size; ++i) {
                                    if (area.contains(m_points[index].get(i))) {
                                        search_results.push_back(m_buckets[index].get(i));
                                    }
                                }

                                if (list->next == 0) {
                                    break;
                                }

                                index += list->next;
                                list = &m_lists[index];
                            }
                        }
                    }
                }

                ++quadrant;
                indices[depth] = m_nodes[indices[depth - 1]].children[quadrant];

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
    }

    void clear() {
        m_lists.clear();
        m_buckets.clear();
        m_points.clear();
        m_nodes.clear();

        m_nodes.emplace_back();
        createBucket();
        m_nodes[0].bucketIndex = 0;
    }

    [[nodiscard]] size_t size() const {
        return m_nodes.size();
    }

    Rectangle bounds() {
        return m_bounds;
    }

    void bounds(Rectangle bound) {
        m_bounds.center = bound.center;
        m_bounds.size = bound.size;
    }

    // TODO: Implement sorting algorithms and test speed
    //  bool sort();

    // TODO: Implement bfs and test speed for primitives and search
    //  SearchResult search_bfs(Rectangle area) const;
    //  void primitives_bfs(float* array) const;

    void primitives(float *array) const {
        // I can imagine dfs should work backwards from quadrants 4 to 1, but the ordering of the data probably doesn't matter here?
        // Maybe working backwards from insertion order has better cache performance.

        int indices[max_depth + 1];
        indices[0] = 0;
        indices[1] = m_nodes[0].children[0];
        Rectangle bounds[max_depth + 1];
        bounds[0] = Rectangle{m_bounds};
        addToArray(array, bounds[0]);
        if (!indices[1]) { return; }
        uint64_t quadrantState = 0;
        int depth = 1;
        int count = 1;
        bool ascended = false;

        while (true) {
            // The last 2 bits of the state are the current quadrant
            uint8_t quadrant = quadrantState & 0b11;
            const int nodeIndex = indices[depth];

            // Critical operations happen when descending the tree
            if (!ascended) {
                Rectangle newBound{bounds[depth - 1]};
                newBound.size = newBound.size * 0.5f;
                newBound.center = newBound.center + newBound.size * QuadrantOffsets[quadrant];
                bounds[depth] = newBound;
                addToArray(array + count++ * VertexCount * 2, newBound);

                if (*m_nodes[nodeIndex].children) {
                    indices[++depth] = *m_nodes[nodeIndex].children;
                    // Shift left 2 bits to go down
                    quadrantState <<= 2;
                    continue;
                }
            }

            ++quadrant;
            indices[depth] = m_nodes[indices[depth - 1]].children[quadrant];

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

using Boidtree = Quadtree<int, 16, 4>;
