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
    using Bucket = std::array<T, bucket_size>;

    struct BucketList {
        ptrdiff_t next = 0; // offset to next bucket
        size_t size = 0;
    };

    struct Node {
        std::array<size_t, ChildCount> children{0, 0, 0, 0};  // Since node 0 can never be a child, 0 is safe to use as a sentry.
        ptrdiff_t bucket_index = -1;

        size_t &operator[](const size_t index) {
            return children[index];
        }

        [[nodiscard]] inline bool has_children() const {
            return children.at(0) > 0;
        }
    };

    template<class T, size_t bucket_max>
    struct QuadtreeData {
        using BucketList = qt_details::BucketList;
        using Bucket = qt_details::Bucket<T, bucket_max>;
        using Points = qt_details::Bucket<Vector, bucket_max>;
        using Node = qt_details::Node;
    private:
        inline void initialize() {
            nodes.emplace_back();
            create_bucket();
            nodes[0].bucket_index = 0;
        }

        inline void create_child(size_t node, size_t child, const ptrdiff_t bucket) {
            const size_t new_node_index = nodes.size();
            nodes.emplace_back();
            nodes[node][child] = new_node_index;
            nodes[new_node_index].bucket_index = bucket;
        }

        inline void create_bucket() {
            lists.emplace_back();
            buckets.emplace_back();
            points.emplace_back();
        }
    public:
        QuadtreeData() {
            initialize();
        }

        inline void add(int point, T data, Vector position) {
            const int point_count = lists[point].size++;
            buckets[point][point_count] = data;
            points[point][point_count] = position;
        }

        inline void clear() {
            lists.clear();
            buckets.clear();
            points.clear();
            nodes.clear();

            initialize();
        }

        inline void new_linked_bucket(ptrdiff_t node) {
            // Create a new bucket
            // Set that bucket as the node's bucket
            // Have the BucketList point backwards to the previous bucket
            const ptrdiff_t new_bucket_index = buckets.size();
            create_bucket();
            lists[new_bucket_index].next = nodes[node].bucket_index - new_bucket_index;
            nodes[node].bucket_index = new_bucket_index;
        }

        void subdivide(const size_t node, const ptrdiff_t bucket, Rectangle bound) {
            create_child(node, 0, bucket);
            for(size_t i = 1; i < ChildCount; ++i) {
                create_child(node, i, buckets.size());
                create_bucket();
            }

            Bucket current_bucket{buckets[bucket]};
            Points current_points{points[bucket]};
            lists[bucket].size = 0;

            for(size_t i = 0; i < bucket_max; ++i) {
                const Vector point_position = current_points[i];
                const int quadrant = bound.quadrant(point_position);
                add(node_bucket(node_child(node, quadrant)), current_bucket[i], point_position);
            }

            nodes[node].bucket_index = -1;
        }

        void push(const Rectangle area, const size_t node, std::vector<T> &search_results) const {
            if (node_bucket(node) > -1 && bucket_size(node_bucket(node)) > 0) {
                ptrdiff_t index = node_bucket(node);
                BucketList const *list = &lists.at(index);
                while(true) {
                    for (size_t i = 0; i < list->size; ++i) {
                        if (area.contains(position(index, i))) {
                            search_results.push_back(data(index, i));
                        }
                    }

                    if (list->next == 0) {
                        break;
                    }

                    index += list->next;
                    list = &lists.at(index);
                }
            }
        }

        [[nodiscard]] inline bool node_has_children(size_t node) const {
            return nodes.at(node).has_children();
        }

        [[nodiscard]] inline size_t node_child(size_t node, size_t child) const {
            return nodes.at(node).children.at(child);
        }

        [[nodiscard]] inline ptrdiff_t node_bucket(size_t node) const {
            return nodes.at(node).bucket_index;
        }

        [[nodiscard]] inline size_t bucket_size(size_t bucket) const {
            return lists.at(bucket).size;
        }

        [[nodiscard]] inline T data(size_t bucket, size_t index) const {
            return buckets.at(bucket).at(index);
        }

        [[nodiscard]] inline Vector position(size_t point_list, size_t index) const {
            return points.at(point_list).at(index);
        }

        [[nodiscard]] inline size_t size() const {
            return nodes.size();
        }

        Node &operator[](const size_t index) {
            return nodes[index];
        }

        std::vector<BucketList> lists;
        std::vector<Bucket> buckets;
        std::vector<Points> points;  // Potentially unnecessary
        std::vector<Node> nodes;
    };
}


template<class T, int depth_max, size_t bucket_max>
class QuadtreeGeometry;


template<class T, int depth_max, size_t bucket_max = 4>
class Quadtree {
public:
    using BucketList = qt_details::BucketList;
    using Bucket = qt_details::Bucket<T, bucket_max>;
    using Points = qt_details::Bucket<Vector, bucket_max>;
    using Node = qt_details::Node;
    using ResultVector = std::vector<T>;
private:
    friend QuadtreeGeometry<T, depth_max, bucket_max>;

    Rectangle m_bounds;
    qt_details::QuadtreeData<T, bucket_max> m_data;

public:
    explicit Quadtree(Rectangle bounds) : m_bounds(bounds) {}

    bool insert(T data, Vector position) {
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

    void search(Rectangle area, ResultVector &search_results) const {
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
                        m_data.push(area, nodeIndex, search_results);
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

    // TODO: Implement sorting algorithms and test speed
    //  bool sort();

    // TODO: Implement bfs and test speed for search
    //  SearchResult search_bfs(Rectangle area) const;
};
