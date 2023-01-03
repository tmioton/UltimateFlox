#pragma once

#include "pch.hpp"
#include "Common.hpp"
#include "Math/Rectangle.hpp"

// Generational algorithm to learn which bucket size works best for a number of birds.
// Sort boid arrays to have boids in the same cache line as the boids they access most often.
// Save the maximum and minimum boid positions every frame and resize the bounds of the tree to fit every bird?

// Just here to avoid seeing a hardcoded "4" and brainlessly changing it to "bucketSize"
static constexpr size_t ChildCount = 4;
static constexpr Vector QuadrantOffsets[ChildCount] {{1.0f, 1.0f}, {-1.0f, 1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f}};

namespace qt_details {
    template<class T, size_t bucket_size>
    class Bucket {
        T m_data[bucket_size]{};
    public:
        Bucket() = default;

        Bucket(Bucket const &lhs) {
            for (ptrdiff_t i = 0; i < static_cast<ptrdiff_t>(bucket_size); ++i) {
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
        ptrdiff_t next = 0; // offset to next bucket
        size_t size = 0;
    };

    struct Node {
        size_t children[ChildCount]{0, 0, 0, 0};  // Since node 0 can never be a child, 0 is safe to use as a sentry.
        ptrdiff_t bucketIndex = -1;
    };
}


template<class T, int max_depth, size_t bucket_size>
class QuadtreeGeometry;


template<class T, int max_depth, size_t bucket_size = 4>
class Quadtree {
    using BucketList = qt_details::BucketList;
    using Bucket = qt_details::Bucket<T, bucket_size>;
    using Points = qt_details::Bucket<Vector, bucket_size>;
    using Node = qt_details::Node;
public:
    using ResultVector = std::vector<T>;
    using Type = T;
    static constexpr int MaxDepth = max_depth;
    static constexpr int BucketSize = bucket_size;
private:
    friend QuadtreeGeometry<T, max_depth, bucket_size>;

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
        size_t nodeIndex = 0;
        Rectangle currentBound{m_bounds};
        if (!currentBound.contains(position)) {
            return false;
        }

        while (true) {
            if (!*m_nodes[nodeIndex].children) {
                const ptrdiff_t bucket = m_nodes[nodeIndex].bucketIndex;
                // No children. See if we can fit.
                if (m_lists[bucket].size < bucket_size) {
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

                        for (int i = 0; i < static_cast<int>(bucket_size); i++) {
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
                        ptrdiff_t newBucketIndex = m_buckets.size();
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
                            ptrdiff_t index = m_nodes[nodeIndex].bucketIndex;
                            BucketList const *list = &m_lists[index];
                            while (true) {
                                for (size_t i = 0; i < list->size; ++i) {
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

    void reserve(size_t size) {
        const double d_bucketsNeeded = glm::ceil((static_cast<double>(size) - bucket_size) / (bucket_size * ChildCount));
        if (d_bucketsNeeded < 0.0) {
            return;
        }

        const int bucketsNeeded = static_cast<int>(d_bucketsNeeded);
        m_lists.reserve(bucketsNeeded);
        m_buckets.reserve(bucketsNeeded);
        m_points.reserve(bucketsNeeded);

        if (max_depth < 2) {
            // Just reserve all possible nodes.
            m_nodes.reserve(5);
        } else {
            //m_nodes.reserve(bucketsNeeded);  // Works but doesn't take max_depth into account
            uint64_t sum = 1;
            uint64_t last = 1;
            for (int i = 0; i < max_depth; i++) {
                last <<= 2;
                sum += last;
            }
            if (bucketsNeeded < sum) {
                m_nodes.reserve(bucketsNeeded);
                std::cout << "reserved " << bucketsNeeded << " nodes." << std::endl;
            } else {
                m_nodes.reserve(sum);
                std::cout << "reserved " << sum << " nodes." << std::endl;
            }
        }
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

    // TODO: Implement bfs and test speed for search
    //  SearchResult search_bfs(Rectangle area) const;
};
