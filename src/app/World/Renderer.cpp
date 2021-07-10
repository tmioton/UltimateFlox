#include "pch.hpp"
#include "Renderer.hpp"

BoidRenderer::BoidRenderer(size_t size, glm::vec2 bounds) : flockSize(size) {
    // Construct shader
    lwvl::VertexShader boidVertexModifier{lwvl::VertexShader::readFile("Data/Shaders/boid.vert")};
    lwvl::FragmentShader boidFragmentModifier{lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")};
    boidModifier.link(boidVertexModifier, boidFragmentModifier);

    boidModifier.bind();
    boidModifier.uniform("scale").set1f(Boid::scale);
    boidModifier.uniform("projection").set2DOrthographic(bounds.y, -bounds.y, bounds.x, -bounds.x);

    //boidShader.uniform("color").set3f(1.00000f, 0.00000f, 0.00000f);  // Red
    //boidShader.uniform("color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
    //boidShader.uniform("color").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
    //boidShader.uniform("color").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
    boidModifier.uniform("color").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
    //boidShader.uniform("color").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayon

    lwvl::VertexShader visionVertexModifier{lwvl::VertexShader::readFile("Data/Shaders/vision.vert")};
    lwvl::FragmentShader visionFragmentModifier{lwvl::FragmentShader::readFile("Data/Shaders/vision.frag")};
    visionModifier.link(visionVertexModifier, visionFragmentModifier);
    visionModifier.bind();
    visionModifier.uniform("projection").set2DOrthographic(bounds.y, -bounds.y, bounds.x, -bounds.x);

    boidController.bind();
    boidController.instances(flockSize);

    // Boid model vertex buffer
    boidVertices.bind();
    boidVertices.construct<float>(nullptr, defaultModel.size());  // Create a buffer of 10 floats
    boidVertices.construct(defaultModel.begin(), defaultModel.end());
    boidController.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    // Boid model indexing buffer
    boidIndices.bind();
    std::array<uint8_t, 10> indexData{
        0, 1, 1, 2, 2, 0, 2, 3, 3, 0
    };

    boidIndices.construct(indexData.begin(), indexData.end());

    visionController.bind();
    visionController.instances(flockSize);

    // Vision model vertex buffer
    visionVertices.bind();
    visionVertices.construct(visionModel.begin(), visionModel.end());
    visionController.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    // Per object offset buffer
    offsets.bind();
    visionController.attribute(2, GL_FLOAT, 4 * sizeof(float), 0, 1);

    boidController.bind();
    offsets.construct(offsetStore.get(), int32_t(4 * flockSize));
    boidController.attribute(2, GL_FLOAT, 4 * sizeof(float), 0, 1);
    boidController.attribute(2, GL_FLOAT, 4 * sizeof(float), 2 * sizeof(float), 1);
}

void BoidRenderer::changeRenderMode(BoidMode mode) {
    switch(mode) {
        case BoidMode::Classic: {
            renderMode = lwvl::PrimitiveMode::Lines;
            indexCount = 10;
            std::array<uint8_t, 10> indexData {
                0, 1, 1, 2, 2, 3, 2, 0, 3, 0
            };
            boidIndices.bind();
            boidIndices.update(indexData.begin(), indexData.end());
            boidVertices.bind();
            boidVertices.update(
                defaultModel.begin(),
                defaultModel.end()
            );
            return;
        }
        case BoidMode::Filled: {
            renderMode = lwvl::PrimitiveMode::TriangleFan;
            indexCount = 4;
            std::array<uint8_t, 4> indexData {
                0, 1, 2, 3
            };
            boidIndices.bind();
            boidIndices.update(indexData.begin(), indexData.end());
            boidVertices.bind();
            boidVertices.update(
                defaultModel.begin(),
                defaultModel.end()
            );
            return;
        }
        default:
            return;
    }
}

void BoidRenderer::toggleVisionRendering() {
    renderVision ^= true;
}

void BoidRenderer::toggleBoidRendering() {
    renderBoids ^= true;
}

void BoidRenderer::update(const BoidArray &boids) {
    for (size_t i = 0; i < flockSize; i++) {
        Boid &boid = boids[i];

        // Update the offsets
        offsetStore[i * 4 + 0] = boid.position.x;
        offsetStore[i * 4 + 1] = boid.position.y;

        // In terms of calculating a rotation,
        // sqrt seems to be faster than atan2
        // and saves cos and sin computations on the gpu
        const float l2 = glm::length2(boid.velocity);
        const Vector normal = l2 != 0.0f ? boid.velocity * glm::fastInverseSqrt(l2) : Vector{0.0f, 0.0f};
        offsetStore[i * 4 + 2] = normal.x;
        offsetStore[i * 4 + 3] = normal.y;
    }

    offsets.bind();
    offsets.update(offsetStore.get(), int32_t(4 * flockSize));
}

void BoidRenderer::draw() {
    if (renderVision) {
        visionModifier.bind();
        visionController.bind();

        visionModifier.uniform("scale").set1f(Boid::cohesiveRadius);
        visionController.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);

        visionModifier.uniform("scale").set1f(Boid::disruptiveRadius);
        visionController.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);
    }

    if (renderBoids) {
        boidModifier.bind();
        boidController.bind();
        boidController.drawElements(
            renderMode, indexCount,
            lwvl::ByteFormat::UnsignedByte
        );
    }
}
