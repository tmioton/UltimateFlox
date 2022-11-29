#pragma once

#include "pch.hpp"
#include "World/Boid.hpp"
#include "Models.hpp"
#include "Controls.hpp"

// This handles the data buffer better,
// but it is still created before the first actual Vertex Array.
// Will fail in core profile mode.


class FlockRenderer {
public:
    explicit FlockRenderer(size_t);

    void update(Boid const array[]);

    void resize(size_t);

    void attachData(Model *);

    static void draw(Model *, BoidShader *);
private:
    lwvl::Buffer data;
    size_t flockSize;
};
