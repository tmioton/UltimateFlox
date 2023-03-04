#pragma once
#include "pch.hpp"
#include "Math/Rectangle.hpp"
#include "Math/Camera.hpp"


class RectangleRenderer;


struct RectanglePrimitive {
    Rectangle rectangle {};
    Color color {1.0f, 1.0f, 1.0f, 1.0f};

    RectanglePrimitive() = default;
    explicit RectanglePrimitive(Rectangle r);
    RectanglePrimitive(Rectangle r, Color c);
};


class RectangleLink {
    friend RectangleRenderer;
    RectangleLink(RectangleRenderer *renderer, UnsignedInt id);
public:
    RectanglePrimitive &get();
private:
    RectangleRenderer *m_renderer;
    UnsignedInt m_id;
};


class RectangleRenderer {
public:
    struct RectangleInstance {
        Vector center;
        Vector size;
        Color color;
    };

    friend RectangleLink;

    RectangleRenderer(const Projection &projection, SignedInt initial_size);
    ~RectangleRenderer();

    RectangleLink push(Rectangle r, Color c);
    RectangleLink push(Rectangle r);
    RectangleLink push();

    void update();
    void update_camera(const Camera &view);

    void draw() const;
private:
    lwvl::Program m_control;
    lwvl::VertexArray m_layout;
    lwvl::Buffer m_instance_buffer;
    lwvl::Buffer m_model;
    lwvl::Uniform u_view;

    RectangleInstance* m_mapped_buffer;
    std::vector<RectanglePrimitive> m_data;

    SignedInt m_buffer_size;
};
