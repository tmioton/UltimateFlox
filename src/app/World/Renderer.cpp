#include "pch.hpp"
#include "Renderer.hpp"

namespace detail {
    BoidRenderer::BoidRenderer(size_t size, int width, int height) :
            flockSize(size)
    {
        const glm::vec2 bounds{calculateBounds(static_cast<float>(width) / static_cast<float>(height))};

        // Construct shader
        lwvl::VertexShader boidVertexModifier{lwvl::VertexShader::readFile("Data/Shaders/boid.vert")};
        lwvl::FragmentShader boidFragmentModifier{lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")};
        boidControl.link(boidVertexModifier, boidFragmentModifier);

        boidControl.bind();
        boidControl.uniform("scale").set1f(Boid::scale);
        boidControl.uniform("projection").set2DOrthographic(bounds.y, -bounds.y, bounds.x, -bounds.x);

        //boidShader.uniform("color").set3f(1.00000f, 0.00000f, 0.00000f);  // Red
        //boidShader.uniform("color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
        //boidShader.uniform("color").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
        //boidShader.uniform("color").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
        boidControl.uniform("color").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
        //boidShader.uniform("color").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayon

        lwvl::VertexShader visionVertexModifier{lwvl::VertexShader::readFile("Data/Shaders/vision.vert")};
        lwvl::FragmentShader visionFragmentModifier{lwvl::FragmentShader::readFile("Data/Shaders/default.frag")};
        visionControl.link(visionVertexModifier, visionFragmentModifier);
        visionControl.bind();
        visionControl.uniform("projection").set2DOrthographic(bounds.y, -bounds.y, bounds.x, -bounds.x);

        boidLayout.bind();
        boidLayout.instances(flockSize);

        // Boid model vertex buffer
        boidVertices.bind();
        boidVertices.construct<float>(nullptr, defaultModel.size());  // Create a buffer of 10 floats
        boidVertices.construct(defaultModel.begin(), defaultModel.end());
        boidLayout.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

        // Boid model indexing buffer
        boidIndices.bind();
        std::array<uint8_t, 10> indexData{
            0, 1, 2, 3
        };

        boidIndices.construct(indexData.begin(), indexData.end());

        visionLayout.bind();
        visionLayout.instances(flockSize);

        // Vision model vertex buffer
        visionVertices.bind();
        visionVertices.construct(visionModel.begin(), visionModel.end());
        visionLayout.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

        // Per object offset buffer
        /* OffsetBufferItem {
         *  float oldPosition;
         *  float newPosition;
         *  float rotationComponents;
         * };
         */
        offsets.bind();
        visionLayout.attribute(2, GL_FLOAT, 4 * sizeof(float), 0, 1);

        boidLayout.bind();
        offsets.construct(offsetStore.get(), 4 * flockSize);
        boidLayout.attribute(2, GL_FLOAT, 4 * sizeof(float), 0, 1);
        boidLayout.attribute(2, GL_FLOAT, 4 * sizeof(float), 2 * sizeof(float), 1);
    }

    void BoidRenderer::changeRenderMode(BoidMode mode) {
        switch (mode) {
            case BoidMode::Classic: {
                renderMode = lwvl::PrimitiveMode::Lines;
                indexCount = 10;
                std::array<uint8_t, 10> indexData{
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
                std::array<uint8_t, 4> indexData{
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
            default:return;
        }
    }

    void BoidRenderer::toggleVisionRendering() {
        renderVision ^= true;
    }

    void BoidRenderer::toggleBoidRendering() {
        renderBoids ^= true;
    }

    void BoidRenderer::update(BoidArray const &current, BoidArray const &previous) {
        // Update offsets
        for (size_t i = 0; i < flockSize; i++) {
            Boid const &boid = current[i];

            const size_t offsetStart = i * 4;

            // Update the offsets
            offsetStore[offsetStart + 0] = boid.position.x;
            offsetStore[offsetStart + 1] = boid.position.y;

            // In terms of calculating a rotation,
            // sqrt seems to be faster than atan2
            // and saves cos and sin computations on the gpu
            const float l2 = glm::length2(boid.velocity);
            const Vector normal = l2 != 0.0f ? boid.velocity * glm::fastInverseSqrt(l2) : Vector{0.0f, 0.0f};
            offsetStore[offsetStart + 2] = normal.x;
            offsetStore[offsetStart + 3] = normal.y;
        }

        offsets.bind();
        offsets.update(offsetStore.get(), 4 * flockSize);
    }

    void BoidRenderer::draw() {
        if (renderVision) {
            visionControl.bind();
            visionLayout.bind();

            visionControl.uniform("scale").set1f(Boid::cohesiveRadius);
            visionLayout.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);

            visionControl.uniform("scale").set1f(Boid::disruptiveRadius);
            visionLayout.drawArrays(lwvl::PrimitiveMode::LineStrip, 17);
        }

        if (renderBoids) {
            boidControl.bind();
            boidLayout.bind();
            boidLayout.drawElements(
                renderMode, indexCount,
                lwvl::ByteFormat::UnsignedByte
            );
        }
    }
}
