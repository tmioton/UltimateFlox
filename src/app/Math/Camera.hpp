#pragma once

#include "pch.hpp"
#include "Math/Rectangle.hpp"


class Camera {
    using CameraMatrix = glm::mat<4, 4, float, glm::defaultp>;
    void apply_position();
public:
    Camera() = default;
    explicit Camera(Rectangle box, Vector viewport);

    // Mutators
    void move(Vector delta);
    void move_to(Vector position);
    void zoom(float level);
    void zoom(Vector zoom_vector);

    // Accessors
    [[nodiscard]] const float *data() const;
    [[nodiscard]] float zoom() const;
    [[nodiscard]] Vector zoom_vector() const;
    [[nodiscard]] Vector position() const;
    [[nodiscard]] Rectangle box() const;
private:
    CameraMatrix m_mat {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    Rectangle m_box {Vector {1.0f, 1.0f}};
    Vector m_viewport {1.0f, 1.0f};
    Vector m_zoom {1.0f, 1.0f};
    float m_aspect = 1.0f;
};
