module;
#include "pch.hpp"
export module Geometry;

export class Geometry {
public:
    virtual ~Geometry() = default;
    virtual void operator()(void*) = 0;
};
