module;
#include "pch.hpp"
export module Algorithm;

import DoubleBuffer;
import Boid;


export class Algorithm {
public:
    virtual ~Algorithm() = default;

    virtual void update(DoubleBuffer<Boid> &boids, float delta) = 0;
};
