/* GEOMETRY CONCEPT
 * Use a subclass of Geometry to return the primitives used to visualize structures that are not inherently visual.
 */

#pragma once
#include "pch.hpp"

class Geometry {
public:
    virtual ~Geometry() = default;
    virtual void operator()(void*) = 0;
};
