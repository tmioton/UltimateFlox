module;
#include "pch.hpp"
export module Camera;

import Rectangle;

export class Camera {
    using CameraMatrix = glm::mat<4, 4, float>;

    void apply_position() {
        // The scaling component of the camera matrix sends the vertex off into the distance. The translation component
        //   of the camera matrix is what needs to be added to the vertex to bring it back.
        // When we apply a scale to the camera matrix, we need to apply the scale to the translation as well.
        const Vector mat_position = -m_box.center * m_zoom;
        std::memcpy(&m_mat[3][0], &mat_position, sizeof(Vector));
    }
public:
    Camera() = default;
    explicit Camera(const Rectangle box, const Vector viewport) :
        m_box(box), m_viewport(viewport),
        m_zoom(box.size / viewport),
        m_aspect(viewport.x / box.size.x / (viewport.y / box.size.y + Epsilon))
    {
        m_mat[3][0] = box.center.x;
        m_mat[3][1] = box.center.y;
        m_mat[0][0] = m_zoom.x;
        m_mat[1][1] = m_zoom.y;
    }

    // Mutators
    void move(const Vector delta) {
        m_box.center += delta;
        apply_position();
    }

    void move_to(const Vector position) {
        m_box.center = position;
        apply_position();
    }

    void zoom(const float level) {
        m_zoom = m_aspect > 1.0f ? Vector {level * m_aspect, level} : Vector {level, level * m_aspect};
        m_box.size = m_viewport / level;
        m_mat[0][0] = m_zoom.x;
        m_mat[1][1] = m_zoom.y;
        apply_position();
    }

    void zoom(const Vector zoom_vector) {
        m_zoom = zoom_vector;
        m_box.size = m_viewport / zoom_vector;
        m_mat[0][0] = m_zoom.x;
        m_mat[1][1] = m_zoom.y;
        apply_position();
    }

    // Accessors
    [[nodiscard]] inline const float *data() const { return &m_mat[0][0]; }
    [[nodiscard]] float zoom() const { return m_aspect > 1.0f ? m_zoom.y : m_zoom.x; }
    [[nodiscard]] Vector zoom_vector() const { return m_zoom; }
    [[nodiscard]] Vector position() const { return m_box.center; }
    [[nodiscard]] Rectangle box() const { return m_box; }
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

