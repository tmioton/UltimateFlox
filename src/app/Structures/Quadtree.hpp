// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once

#include "pch.hpp"
#include "Common.hpp"
#include "Math/Rectangle.hpp"

namespace structures {
    // ? Generational algorithm to learn which bucket size works best for a number of birds
    // ? Sort boid arrays to have boids in the same cache line as the boids they access most often

    // C-style struct and others can define their own interactions with the data.
    // . Makes it easier to implement classes that need only a small subset of the data.

    // Perhaps this would be better implemented as a 2-stage structure.
    //  Two classes, one for creating the tree and one for searching the resulting tree.
    // . Allows us to have to separate representations of the tree
    //   . Linked-list when creating the tree for ease of insertion
    //   . Harden the tree into a vector for speed of search

    // Just here to avoid seeing a hardcoded "4" and brainlessly changing it to "bucketSize"
    static constexpr size_t QuadtreeChildCount = 4;
    static constexpr Vector QuadrantOffsets[QuadtreeChildCount] {{1.0f, 1.0f}, {-1.0f, 1.0f}, {-1.0f, -1.0f}, {1.0f, -1.0f}};

    template<class T>
    struct Quadtree {
        static constexpr size_t BucketItemCount = 8;  // Some multiple that's cache-appropriate
        static constexpr size_t MaxDepth = 11;        // Max 32.

        typedef std::array<T, BucketItemCount> Bucket;
        typedef std::array<Vector, BucketItemCount> Points;
        struct BucketList {
            ptrdiff_t next = 0;
            size_t size = 0;
        };

        struct Node {
            std::array<size_t, QuadtreeChildCount> children {0, 0, 0, 0};
            ptrdiff_t bucket_index = -1;

            size_t &operator[](const size_t index) {
                return children[index];
            }

            [[nodiscard]] inline bool has_children() const {
                return children.at(0) > 0;  // Assumption: all children are allocated at the same time.
            }
        };

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

        inline void add(int point, T data, Vector position) {
            const int point_count = lists[point].size++;
            buckets[point][point_count] = data;
            points[point][point_count] = position;
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
            for(size_t i = 1; i < QuadtreeChildCount; ++i) {
                create_child(node, i, buckets.size());
                create_bucket();
            }

            Bucket current_bucket{buckets[bucket]};
            Points current_points{points[bucket]};
            lists[bucket].size = 0;

            for(size_t i = 0; i < BucketItemCount; ++i) {
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

        explicit Quadtree(Rectangle bounding_box) : bounds(bounding_box) {
            initialize();
        }

        void clear() {
            lists.clear();
            buckets.clear();
            points.clear();
            nodes.clear();

            initialize();
        }

        // Default insert for T
        bool insert(T data, Vector position) {
            int depth = 0;
            size_t node_index = 0;
            Rectangle current_bound{bounds};
            if (!current_bound.contains(position)) {
                return false;
            }

            while (true) {
                if (!node_has_children(node_index)) {
                    const ptrdiff_t bucket = node_bucket(node_index);
                    // No children. See if we can fit.
                    if (bucket_size(bucket) < BucketItemCount) {
                        // Add to the current point list.
                        add(bucket, data, position);
                        return true;
                    } else {
                        if (depth < MaxDepth) {
                            subdivide(node_index, bucket, current_bound);
                            continue; // Do another loop to see where the new point can fit.
                        } else {
                            // Do not create children. Fill and create PointLists as needed.
                            new_linked_bucket(node_index);
                            continue;
                        }
                    }
                } else {
                    // Figure out which child node we're in.
                    int quadrant = current_bound.quadrant(position);
                    node_index = node_child(node_index, quadrant);
                    current_bound.size = current_bound.size * 0.5f;
                    current_bound.center += current_bound.size * QuadrantOffsets[quadrant];
                    ++depth;
                    continue;
                }
            }
        }

        // Default search for T
        void search(Rectangle area, std::vector<T> &search_results) const {
            if (bounds.has_intersection_with(area)) {
                int32_t indices[MaxDepth + 1];
                indices[0] = 0;
                indices[1] = node_child(0, 0);

                // Array of rectangles with a very specific structure.
                Rectangle terrace[MaxDepth + 1];  // TODO: test storing just centers
                terrace[0] = Rectangle{bounds};
                if (indices[1] == 0) { return; }

                // Array of 32 2-bit numbers. Saves the search quadrant of the current level when descending.
                uint64_t quadrant_memory = 0;
                int depth = 1;
                bool ascended = false;

                // Possible to abstract this loop?
                while (true) {
                    // The last 2 bits of the state are the current quadrant
                    uint8_t quadrant = quadrant_memory & 0b11;
                    const size_t node_index = indices[depth];

                    // Critical operations happen when descending the tree
                    if (!ascended) {
                        Rectangle new_bound{terrace[depth - 1]};
                        new_bound.size = new_bound.size * 0.5f;
                        new_bound.center = new_bound.center + new_bound.size * QuadrantOffsets[quadrant];
                        terrace[depth] = new_bound;

                        if (node_has_children(node_index) && new_bound.has_intersection_with(area)) {
                            indices[++depth] = node_child(node_index, 0);
                            // Shift left 2 bits to go down
                            quadrant_memory <<= 2;
                            continue;
                        } else if (new_bound.has_intersection_with(area)) {
                            // Bottom. Add my contents to the search.
                            push(area, node_index, search_results);
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
                        indices[depth] = node_child(indices[depth - 1], quadrant);
                        quadrant_memory += 1;
                        ascended = false;
                    }
                }
            }
        }

        Rectangle bounds;
        std::vector<BucketList> lists;
        std::vector<Bucket> buckets;
        std::vector<Points> points;
        std::vector<Node> nodes;
    };
}
