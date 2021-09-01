#include "pch.hpp"
#include "Renderer.hpp"

DataBufferUpdater::DataBufferUpdater(size_t size) : flockSize(size) {
    dataBuffer.bind();
    dataBuffer.construct<float>(nullptr, size * 6);
}

lwvl::ArrayBuffer &DataBufferUpdater::buffer() {
    return dataBuffer;
}

void DataBufferUpdater::update(const BoidArray &array) {
    dataBuffer.bind();
    dataBuffer.update(array.get(), flockSize);
}

void DataBufferUpdater::resize(size_t size) {
    dataBuffer.bind();
    dataBuffer.construct<float>(nullptr, size * 6);
    flockSize = size;
}

BoidRenderer::BoidRenderer(size_t size, int width, int height, lwvl::ArrayBuffer& offsetBuffer) {
    const glm::vec2 bounds{calculateBounds(static_cast<float>(width) / static_cast<float>(height))};

    lwvl::ShaderProgram& boidDefaultControl = controls[defaultControl];
    activeControl = boidDefaultControl;
    boidDefaultControl.link(
        lwvl::VertexShader::readFile("Data/Shaders/boid.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")
    );

    lwvl::ShaderProgram& boidSpeedDebugControl = controls[speedDebugControl];
    boidSpeedDebugControl.link(
        lwvl::VertexShader::readFile("Data/Shaders/speeddebug.vert"),
        lwvl::FragmentShader::readFile("Data/Shaders/speeddebug.frag")
    );
    boidSpeedDebugControl.bind();
    boidSpeedDebugControl.uniform("maxSpeed").set1f(Boid::maxSpeed);

    for (auto &control : controls) {
        control.bind();
        control.uniform("scale").set1f(Boid::scale);
        control.uniform("projection").set2DOrthographic(bounds.y, -bounds.y, bounds.x, -bounds.x);

        //control.uniform("color").set3f(1.00000f, 0.00000f, 0.00000f);  // Red
        //control.uniform("color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
        //control.uniform("color").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
        //control.uniform("color").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
        control.uniform("color").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
        //control.uniform("color").set3f(0.17647f, 0.18824f, 0.27843f);  // Space Cadet
        //control.uniform("color").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayon
        //control.uniform("color").set3f(0.91765f, 0.88235f, 0.31765f);  // Hard Yellow
    }

    layout.bind();
    layout.instances(size);

    // Boid model vertex buffer
    vertices.bind();
    vertices.construct<float>(nullptr, defaultModel.size());  // Create a buffer of 10 floats
    vertices.construct(defaultModel.begin(), defaultModel.end());
    layout.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    // Boid model indexing buffer
    indices.bind();
    std::array<uint8_t, 10> indexData{
        0, 1, 2, 3
    };

    indices.construct(indexData.begin(), indexData.end());

    offsetBuffer.bind();
    layout.attribute(2, GL_FLOAT, sizeof(Boid), 0, 1);
    layout.attribute(2, GL_FLOAT, sizeof(Boid), 2 * sizeof(float), 1);
}

void BoidRenderer::changeRenderMode(RenderMode mode) {
    switch (mode) {
        case RenderMode::Classic: {
            renderMode = lwvl::PrimitiveMode::Lines;
            indexCount = 10;
            std::array<uint8_t, 10> indexData{
                0, 1, 1, 2, 2, 3, 2, 0, 3, 0
            };
            indices.bind();
            indices.update(indexData.begin(), indexData.end());
            vertices.bind();
            vertices.update(
                defaultModel.begin(),
                defaultModel.end()
            );
            return;
        }
        case RenderMode::Filled: {
            renderMode = lwvl::PrimitiveMode::TriangleFan;
            indexCount = 4;
            std::array<uint8_t, 4> indexData{
                0, 1, 2, 3
            };
            indices.bind();
            indices.update(indexData.begin(), indexData.end());
            vertices.bind();
            vertices.update(
                defaultModel.begin(),
                defaultModel.end()
            );
            return;
        }
        default:return;
    }
}

void BoidRenderer::changeControlMode(BoidRenderer::ControlMode mode) {
    switch(mode) {
        case ControlMode::Default:
            activeControl = controls[defaultControl];
            break;
        case ControlMode::SpeedDebug:
            activeControl = controls[speedDebugControl];
            break;
    }
}

void BoidRenderer::draw() {
    activeControl.bind();
    layout.bind();
    layout.drawElements(
        renderMode, indexCount,
        lwvl::ByteFormat::UnsignedByte
    );
}

void BoidRenderer::resize(size_t size) {
    layout.bind();
    layout.instances(size);
}

VisionRenderer::VisionRenderer(size_t size, int width, int height, lwvl::ArrayBuffer &offsetBuffer) {
    const glm::vec2 bounds{calculateBounds(static_cast<float>(width) / static_cast<float>(height))};

    // Construct Shader
    lwvl::VertexShader visionVertexModifier{lwvl::VertexShader::readFile("Data/Shaders/vision.vert")};
    lwvl::FragmentShader visionFragmentModifier{lwvl::FragmentShader::readFile("Data/Shaders/default.frag")};
    control.link(visionVertexModifier, visionFragmentModifier);
    control.bind();
    control.uniform("projection").set2DOrthographic(bounds.y, -bounds.y, bounds.x, -bounds.x);

    layout.bind();
    layout.instances(size);

    vertices.bind();
    vertices.construct(visionModel.begin(), visionModel.end());
    layout.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    offsetBuffer.bind();
    layout.attribute(2, GL_FLOAT, sizeof(Boid), 0, 1);
}

void VisionRenderer::draw() {
    control.bind();
    layout.bind();

    control.uniform("scale").set1f(Boid::cohesiveRadius);
    layout.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);

    control.uniform("scale").set1f(Boid::disruptiveRadius);
    layout.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);
}

void VisionRenderer::resize(size_t size) {
    layout.bind();
    layout.instances(size);
}
