#include "pch.hpp"
#include "Flock.hpp"


Flock::Flock(float aspect) {
    // Setup boid starting locations
    constexpr float tauOverSize = constants::tau / static_cast<float>(flockSize);
    for (size_t i = 0; i < flockSize; i++) {
        Boid &boid = m_primaryFlock[i];
        auto angle = static_cast<float>(i) * tauOverSize;
        Vector offsets {cosf(angle), sinf(angle)};
        boid.position = 50.0f * offsets;
        boid.velocity = 10.0f * offsets + angle;
        boid.velocity.magnitude(10.0f);
    }

    configureRendering(aspect);
}

void Flock::configureRendering(float aspect) {
    // Construct shader
    lwvl::VertexShader vertexSource{lwvl::VertexShader::readFile("Data/Shaders/boid.vert")};
    lwvl::FragmentShader fragmentSource{lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")};
    boidShader.link(vertexSource, fragmentSource);

    boidShader.bind();
    boidShader.uniform("scale").set1f(10.0f);
    //boidShader.uniform("color").set3f(1.00000f, 0.00000f, 0.00000f);  // Red
    //boidShader.uniform("color").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
    //boidShader.uniform("color").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
    boidShader.uniform("color").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
    //boidShader.uniform("color").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayon

    if (aspect > 1.0f) {
        boidShader.uniform("projection").set2DOrthographic(
            100.0f, -100.0f, 100.0f * aspect, -100.0f * aspect
        );
    } else {
        boidShader.uniform("projection").set2DOrthographic(
            100.0f / aspect, -100.0f / aspect, 100.0f, -100.0f
        );
    }

    arrayBuffer.bind();
    arrayBuffer.instances(flockSize);

    // Boid model vertex buffer
    vertexBuffer.bind();
    const float piOver4 = constants::pi * 0.25f;
    std::array<float, 8> vertexData{
        cosf(0.0f), sinf(0.0f),
        cosf(3.0f * piOver4), sinf(3.0f * piOver4),
        0.5f * cosf(constants::pi), 0.5f * sinf(constants::pi),
        cosf(5.0f * piOver4), sinf(5.0f * piOver4)
    };
    vertexBuffer.construct(vertexData.begin(), vertexData.end());
    arrayBuffer.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

    // Boid model indexing buffer
    indexBuffer.bind();
    std::array<uint8_t, 10> indexData{
        0, 1, 1, 2, 2, 0, 2, 3, 3, 0
    };

    indexBuffer.construct(indexData.begin(), indexData.end());

    // Per-boid offset buffer
    offsetBuffer.bind();
    for (size_t i = 0; i < flockSize; i++) {
        const Boid &boid = m_primaryFlock[i];
        offsetArray[i * 4 + 0] = boid.position.x;
        offsetArray[i * 4 + 1] = boid.position.y;
        offsetArray[i * 4 + 2] = boid.velocity.x;
        offsetArray[i * 4 + 3] = boid.velocity.y;
    }

    offsetBuffer.construct(offsetArray.get(), 4 * flockSize);
    arrayBuffer.attribute(2, GL_FLOAT, 4 * sizeof(float), 0, 1);
    arrayBuffer.attribute(2, GL_FLOAT, 4 * sizeof(float), 2 * sizeof(float), 1);
}

void Flock::update(float dt) {
    for (size_t i = 0; i < flockSize; i++) {
        m_secondaryFlock[i] = m_primaryFlock[i];
        Boid &currentBoid = m_secondaryFlock[i];

        currentBoid.velocity += currentBoid.acceleration;
        currentBoid.position += currentBoid.velocity * dt;
        currentBoid.acceleration *= 0.0f;

        // Update the offsets
        offsetArray[i * 4 + 0] = currentBoid.position.x;
        offsetArray[i * 4 + 1] = currentBoid.position.y;

        // In terms of calculating a rotation,
        // sqrt seems to be faster than atan2
        // and saves cos and sin computations on the gpu
        float mag = currentBoid.velocity.magnitude();
        offsetArray[i * 4 + 2] = currentBoid.velocity.x / mag;
        offsetArray[i * 4 + 3] = currentBoid.velocity.y / mag;
    }

    offsetBuffer.bind();
    offsetBuffer.update(offsetArray.get(), flockSize * 4);

    // Move array pointers
    auto temp = std::move(m_primaryFlock);
    m_primaryFlock = std::move(m_secondaryFlock);
    m_secondaryFlock = std::move(temp);

    //std::cout << "Boid 1 position: " << m_primaryFlock[0].position << std::endl;
}

void Flock::draw() {
    boidShader.bind();
    arrayBuffer.bind();
    arrayBuffer.drawElements(
        renderMode, indexCount,
        lwvl::ByteFormat::UnsignedByte
    );
}

void Flock::changeRenderMode(lwvl::PrimitiveMode mode) {
    switch(mode) {
        case lwvl::PrimitiveMode::Lines: {
            renderMode = mode;
            indexCount = 10;
            std::array<uint8_t, 10> indexData {
                0, 1, 1, 2, 2, 3, 2, 0, 3, 0
            };
            indexBuffer.bind();
            indexBuffer.update(indexData.begin(), indexData.end());
            return;
        }
        case lwvl::PrimitiveMode::TriangleFan: {
            renderMode = mode;
            indexCount = 4;
            std::array<uint8_t, 4> indexData {
                0, 1, 2, 3
            };
            indexBuffer.bind();
            indexBuffer.update(indexData.begin(), indexData.end());
            return;
        }
        default:
            return;
    }
}

