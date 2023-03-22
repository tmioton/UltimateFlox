// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "Camera.hpp"


void Camera::apply_position() {
    // The scaling component of the camera matrix sends the vertex off into the distance. The translation component
    //   of the camera matrix is what needs to be added to the vertex to bring it back.
    // When we apply a scale to the camera matrix, we need to apply the scale to the translation as well.
    const Vector mat_position = -m_box.center * m_zoom;
    std::memcpy(&m_mat[3][0], &mat_position, sizeof(Vector));
}


Camera::Camera(Rectangle box, Vector viewport) :
    m_box(box), m_viewport(viewport),
    m_zoom(box.size / viewport),
    m_aspect((viewport.x / box.size.x) / ((viewport.y / box.size.y) + Epsilon))
{
    m_mat[3][0] = box.center.x;
    m_mat[3][1] = box.center.y;
    m_mat[0][0] = m_zoom.x;
    m_mat[1][1] = m_zoom.y;
}

void Camera::move(Vector delta) {
    m_box.center += delta;
    apply_position();
}

void Camera::move_to(Vector position) {
    m_box.center = position;
    apply_position();
}

void Camera::zoom(float level) {
    m_zoom = m_aspect > 1.0f ? Vector {level * m_aspect, level} : Vector {level, level * m_aspect};
    m_box.size = m_viewport / level;
    m_mat[0][0] = m_zoom.x;
    m_mat[1][1] = m_zoom.y;
    apply_position();
}

void Camera::zoom(Vector zoom_vector) {
    m_zoom = zoom_vector;
    m_box.size = m_viewport / zoom_vector;
    m_mat[0][0] = m_zoom.x;
    m_mat[1][1] = m_zoom.y;
    apply_position();
}

const float *Camera::data() const {
    return &m_mat[0][0];
}

float Camera::zoom() const {
    return m_aspect > 1.0f ? m_zoom.y : m_zoom.x;
}

Vector Camera::zoom_vector() const {
    return m_zoom;
}

Vector Camera::position() const {
    return m_box.center;
}

Rectangle Camera::box() const {
    return m_box;
}
