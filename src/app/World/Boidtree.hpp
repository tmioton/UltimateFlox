#pragma once
#include "pch.hpp"
#include "Boid.hpp"
#include "Structures/Quadtree.hpp"


typedef structures::Quadtree<const Boid*> Boidtree;

// Needs a self parameter to perform an identity check before dereference copy
void search(const Boidtree &tree, const Boid *self, Rectangle area, std::vector<Boid> &search_results);
