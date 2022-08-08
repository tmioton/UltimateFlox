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

    static void draw(BoidModel *, BoidShader *);

    // Convert to concepts & requirements
    template<class T, std::enable_if_t<std::is_base_of<BoidModel, T>::value, bool> = true>
    T model() {
        return std::move(T{flockSize, data});
    }

    template<class T, std::enable_if_t<std::is_base_of<BoidShader, T>::value, bool> = true>
    T control(Projection &projection) {
        return std::move(T{projection});
    }

private:
    lwvl::Buffer data;
    size_t flockSize;
};
