// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "World/Boid.hpp"
#include "Controls.hpp"

void BoidShader::update_camera(const Camera &view) {
    control.bind();
    lwvl::Uniform u_view {control.uniform("view")};
    if (u_view.location() > -1) {
        u_view.matrix4F(view.data());
    }
}

void BoidShader::draw(Model const *model) const {
    control.bind();
    model->draw();
    lwvl::Program::clear();
}

DefaultBoidShader::DefaultBoidShader(Projection &proj) {
    control.link(
        lwvl::VertexShader::readFile("Data/Shaders/boid.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")
    );

    control.bind();
    control.uniform("scale").setF(Boid::scale);
    Color color = BoidColors[m_color];
    control.uniform("color").setF(color.r, color.g, color.b);
    control.uniform("projection").matrix4F(&proj[0][0]);
}

void DefaultBoidShader::nextColor() {
    m_color = (m_color + 1) % BoidColorCount;
    Color color = BoidColors[m_color];
    control.bind();
    control.uniform("color").setF(color.r, color.g, color.b);
    lwvl::Program::clear();
}

SpeedDebugShader::SpeedDebugShader(Projection &proj) {
    control.link(
        lwvl::VertexShader::readFile("Data/Shaders/speeddebug.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/speeddebug.frag")
    );

    control.bind();
    control.uniform("scale").setF(Boid::scale);
    control.uniform("projection").matrix4F(&proj[0][0]);
    control.uniform("maxSpeed").setF(Boid::maxSpeed);
}

VisionShader::VisionShader(Projection &proj) {
    control.link(
        lwvl::VertexShader::readFile("Data/Shaders/vision.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/default.frag")
    );

    control.bind();
    control.uniform("color").setF(1.0f, 1.0f, 1.0f);
    control.uniform("alpha").setF(0.30f);
    control.uniform("projection").matrix4F(&proj[0][0]);
}

void VisionShader::radius(float radius) {
    control.bind();
    control.uniform("scale").setF(radius);
}
