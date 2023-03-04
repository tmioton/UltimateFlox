#pragma once

#include "pch.hpp"
#include "Models.hpp"
#include "Math/Camera.hpp"

// Determine whether these should have to be passed shader strings

class BoidShader {
public:
    virtual ~BoidShader() = default;

    void update_camera(const Camera &view);
    void draw(Model const *) const;

protected:
    lwvl::Program control;
};


class DefaultBoidShader : public BoidShader {
public:
    explicit DefaultBoidShader(Projection &proj);

    ~DefaultBoidShader() override = default;
    void nextColor();

protected:
    uint8_t m_color = 0;
};


class SpeedDebugShader : public BoidShader {
public:
    explicit SpeedDebugShader(Projection &proj);

    ~SpeedDebugShader() override = default;
};


class VisionShader : public BoidShader {
public:
    explicit VisionShader(Projection &proj);

    ~VisionShader() override = default;

    void radius(float);

private:
    lwvl::Uniform u_radius;
};
