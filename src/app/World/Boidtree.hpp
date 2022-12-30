#pragma once
#include "pch.hpp"
#include "Boid.hpp"
#include "Structures/Quadtree.hpp"

using Boidtree = Quadtree<Boid, 8, 5>;
