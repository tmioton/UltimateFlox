module;
#include "pch.hpp"
export module Boidtree;

import Quadtree;
import Rectangle;
import Boid;


export typedef Quadtree<const Boid*> Boidtree;

// Needs a self parameter to perform an identity check before dereference copy
export void search(const Boidtree &tree, const Boid *self, Rectangle area, std::vector<Boid> &search_results) {
    if (tree.bounds.intersects(area)) {
        size_t indices[Boidtree::MaxDepth + 1];
        indices[0] = 0;
        indices[1] = tree.node_child(0, 0);
        Rectangle terrace[Boidtree::MaxDepth + 1];  // TODO: test storing just centers
        terrace[0] = Rectangle{tree.bounds};
        if (!indices[1]) { return; }

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

                if (tree.node_has_children(node_index) && new_bound.intersects(area)) {
                    indices[++depth] = tree.node_child(node_index, 0);
                    // Shift left 2 bits to go down
                    quadrant_memory <<= 2;
                    continue;
                } else if (new_bound.intersects(area)) {
                    // Bottom. Add my contents to the search.
                    if (tree.node_bucket(node_index) > -1 && tree.bucket_size(tree.node_bucket(node_index)) > 0) {
                        ptrdiff_t index = tree.node_bucket(node_index);
                        Boidtree::BucketList const *list = &tree.lists.at(index);
                        while(true) {
                            for (size_t i = 0; i < list->size; ++i) {
                                // Any way to make this branch-less?
                                if (area.contains(tree.position(index, i)) && tree.data(index, i) != self) {
                                    search_results.push_back(*tree.data(index, i));
                                }
                            }

                            if (list->next == 0) {
                                break;
                            }

                            index += list->next;
                            list = &tree.lists.at(index);
                        }
                    }
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
                indices[depth] = tree.node_child(indices[depth - 1], quadrant);
                quadrant_memory += 1;
                ascended = false;
            }
        }
    }
}


