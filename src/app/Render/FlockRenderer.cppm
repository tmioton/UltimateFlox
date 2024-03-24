module;
#include "pch.hpp"
export module FlockRenderer;

import Boid;
import Camera;
import RawArray;


export class Object {
public:
    Object(size_t vertexCount, size_t indexCount, lwvl::PrimitiveMode mode) : m_vertexCount(vertexCount), m_indexCount(indexCount), m_drawMode(mode), m_vertices(vertexCount), m_indices(indexCount) {}
    Object(Object &&other)  noexcept : m_vertexCount(other.m_vertexCount), m_indexCount(other.m_indexCount), m_drawMode(other.m_drawMode), m_vertices(std::move(other.m_vertices)), m_indices(std::move(other.m_indices)) {}

    Object& operator=(Object &&rhs) noexcept {
        m_vertexCount = rhs.m_vertexCount;
        m_indexCount = rhs.m_indexCount;
        m_drawMode = rhs.m_drawMode;

        m_vertices = std::move(rhs.m_vertices);
        m_indices = std::move(rhs.m_indices);

        return *this;
    }

    [[nodiscard]] const std::vector<float> &vertices() const {
        return m_vertices;
    }

    [[nodiscard]] std::vector<float> &vertices() {
        return m_vertices;
    }

    [[nodiscard]] size_t vertexCount() const {
        return m_vertexCount;
    }

    [[nodiscard]] const std::vector<unsigned int> &indices() const {
        return m_indices;
    }

    [[nodiscard]] std::vector<unsigned int> &indices() {
        return m_indices;
    }

    [[nodiscard]] size_t indexCount() const {
        return m_indexCount;
    }

    [[nodiscard]] lwvl::PrimitiveMode drawMode() const {
        return m_drawMode;
    }
private:
    size_t m_vertexCount;
    size_t m_indexCount;

    lwvl::PrimitiveMode m_drawMode;

    // Tie this to a common Object allocator
    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
};

export Object loadObject(
    const float *_vertices, size_t vertexCount,
    const unsigned int *_indices, size_t indexCount,
    lwvl::PrimitiveMode mode
) {
    auto vertices = RawArray(_vertices, vertexCount);
    auto indices = RawArray(_indices, indexCount);
    Object obj{vertexCount, indexCount, mode};

    std::copy(vertices.begin(), vertices.end(), obj.vertices().data());
    std::copy(indices.begin(), indices.end(), obj.indices().data());

    return obj;
}

export void loadObject(std::unordered_map<std::string, Object> &objects, const std::string &objDescriptor) {

}


export constexpr std::array<float, 12> boidShape{
    1.0f, 0.0f, 0.0f,
    -0.707107f, 0.707107f, 0.0f,
    -0.5f, 0.0f, 0.0f,
    -0.707107f, -0.707107f, 0.0f
};

export constexpr std::array<unsigned int, 10> classicIndices {
    0, 1, 1, 2, 2, 3, 2, 0, 3, 0
};

export constexpr std::array<unsigned int, 6> filledIndices {
    0, 1, 2,
    2, 3, 0
};

export constexpr std::array<float, 51> visionShape{
    1.0f, 0.0f, 0.0f,
    0.92388f, 0.38268f, 0.0f,
    0.70711f, 0.70711f, 0.0f,
    0.38268f, 0.92388f, 0.0f,
    0.0f, 1.0f, 0.0f,
    -0.38268f, 0.92388f, 0.0f,
    -0.70711f, 0.70711f, 0.0f,
    -0.92388f, 0.38268f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -0.92388f, -0.38268f, 0.0f,
    -0.70711f, -0.70711f, 0.0f,
    -0.38268f, -0.92388f, 0.0f,
    -0.0f, -1.0f, 0.0f,
    0.38268f, -0.92388f, 0.0f,
    0.70711f, -0.70711f, 0.0f,
    0.92388f, -0.38268f, 0.0f,
    1.0f, 0.0f, 0.0f
};

export constexpr std::array<unsigned int, 34> visionIndices {
    0, 1, 1, 2, 2, 3,
    3, 4, 4, 5, 5, 6,
    6, 7, 7, 8, 8, 9,
    9, 10, 10, 11, 11, 12,
    12, 13, 13, 14, 14, 15,
    15, 16, 16, 0
};

export constexpr std::array<float, 24> boxVertices {
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f
};

export constexpr std::array<unsigned int, 24> boxIndices {
    0, 1, 1, 2, 2, 3, 3, 0,
    0, 4, 1, 5, 2, 6, 3, 7,
    4, 5, 5, 6, 6, 7, 7, 4
};


export class Model {
public:
    explicit Model(Object const& object, size_t instances): mode(object.drawMode()), count(object.indexCount()), instances(instances) {
        layout.instances = instances;
        vertices.store(object.vertices().begin(), object.vertices().end());
        layout.array(vertices, 0, 0, 3 * sizeof(float));
        layout.attribute(0, 0, 3, lwvl::ByteFormat::Float, 0);

        indices.store(object.indices().begin(), object.indices().end());
        layout.element(indices);
    }

    void draw() const {
        layout.drawElements(mode, count, lwvl::ByteFormat::UnsignedInt);
    }

    lwvl::VertexArray layout;
    lwvl::Buffer vertices;
    lwvl::Buffer indices;
    lwvl::PrimitiveMode mode;
    int32_t count;
    int32_t instances = 0;
    int16_t binding = 0;
    int16_t attribute = 0;
};

export void attachBoidData(Model *model, lwvl::Buffer& buffer) {
    model->layout.array(buffer, 1, 0, sizeof(Boid));
    model->layout.attribute(1, 1, Vector::length(), lwvl::ByteFormat::Float, offsetof(Boid, position));
    model->layout.attribute(1, 2, Vector::length(), lwvl::ByteFormat::Float, offsetof(Boid, velocity));
    model->layout.divisor(1, 1);
}


export class BoidShader {
public:
    virtual ~BoidShader() = default;

    void update_camera(const Camera &view) {
        control.bind();
        lwvl::Uniform u_view {control.uniform("view")};
        if (u_view.location() > -1) {
            u_view.matrix4F(view.data());
        }
    }

    void draw(Model const *model) const {
        control.bind();
        model->draw();
        lwvl::Program::clear();
    }

protected:
    lwvl::Program control;
};


export class DefaultBoidShader : public BoidShader {
public:
    explicit DefaultBoidShader(Projection &proj) {
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

    ~DefaultBoidShader() override = default;
    void nextColor() {
        m_color = (m_color + 1) % BoidColorCount;
        Color color = BoidColors[m_color];
        control.bind();
        control.uniform("color").setF(color.r, color.g, color.b);
        lwvl::Program::clear();
    }

protected:
    uint8_t m_color = 0;
};


export class SpeedDebugShader : public BoidShader {
public:
    explicit SpeedDebugShader(Projection &proj) {
        control.link(
            lwvl::VertexShader::readFile("Data/Shaders/speeddebug.vert"),
            lwvl::FragmentShader::readFile("Data/Shaders/speeddebug.frag")
        );

        control.bind();
        control.uniform("scale").setF(Boid::scale);
        control.uniform("projection").matrix4F(&proj[0][0]);
        control.uniform("maxSpeed").setF(Boid::maxSpeed);
    }

    ~SpeedDebugShader() override = default;
};


export class VisionShader : public BoidShader {
public:
    explicit VisionShader(Projection &proj) {
        control.link(
            lwvl::VertexShader::readFile("Data/Shaders/vision.vert"),
            lwvl::FragmentShader::readFile("Data/Shaders/default.frag")
        );

        control.bind();
        control.uniform("color").setF(1.0f, 1.0f, 1.0f);
        control.uniform("alpha").setF(0.30f);
        control.uniform("projection").matrix4F(&proj[0][0]);
    }

    ~VisionShader() override = default;

    void radius(const float radius) {
        control.bind();
        control.uniform("scale").setF(radius);
    }

private:
    lwvl::Uniform u_radius;
};


export class FlockRenderer {
public:
    explicit FlockRenderer(size_t size) : flockSize(size) {
        data.store<float>(nullptr, size * sizeof(Boid), lwvl::bits::Dynamic | lwvl::bits::Client);
    }

    void update(Boid const array[]) {
        data.update(array, flockSize * sizeof(Boid));
    }

    void resize(size_t size) {
        data.store<float>(nullptr, size * sizeof(Boid), lwvl::bits::Dynamic | lwvl::bits::Client);
        flockSize = size;
    }

    void attachData(Model *model) {
        attachBoidData(model, data);
    }

    static void draw(Model const *model, BoidShader const *shader) {
        shader->draw(model);
    }
private:
    lwvl::Buffer data;
    size_t flockSize;
};
