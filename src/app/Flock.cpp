#include "pch.hpp"
#include "Flock.hpp"


static Vector magnitude(Vector const& vec, float mag) {
    const float x = vec.x;
    const float y = vec.y;
    const float l2 = x * x + y * y;
    if (l2 != 0.0f) {
        return vec * glm::fastInverseSqrt(l2) * mag;
    } else {
        return vec;
    }
}


Vector Boid::steer(Vector const& vec) const {
    return magnitude(magnitude(vec, Boid::maxSpeed) - velocity, Boid::maxForce);
}


Flock::Flock(size_t flock_size, float aspect) : flockSize(flock_size), bounds{aspect >= 1.0f ? worldBound * aspect : worldBound, aspect < 1.0f ? worldBound * aspect : worldBound} {
    // Set up boid starting locations
    const float tauOverSize = glm::two_pi<float>() / static_cast<float>(flockSize);
    for (size_t i = 0; i < flockSize; i++) {
        Boid &boid = m_primaryFlock[i];
        auto angle = static_cast<float>(i) * tauOverSize;
        Vector offsets {cosf(angle), sinf(angle)};
        boid.position = 50.0f * offsets;
        boid.velocity = magnitude(10.0f * offsets + angle, Boid::maxSpeed);
        boid.acceleration.y = -1.0f;
    }

    // Set up boid batch rendering
    configureRendering();
}

void Flock::configureRendering() {
    // Construct shader
    lwvl::VertexShader vertexSource{lwvl::VertexShader::readFile("Data/Shaders/boid.vert")};
    lwvl::FragmentShader fragmentSource{lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")};
    boidShader.link(vertexSource, fragmentSource);

    boidShader.bind();
    boidShader.uniform("scale").set1f(Boid::scale);
    boidShader.uniform("projection").set2DOrthographic(bounds.y, -bounds.y, bounds.x, -bounds.x);

    //boidShader.uniform("color").set3f(1.00000f, 0.00000f, 0.00000f);  // Red
    //boidShader.uniform("color").set3f(1.00000f, 1.00000f, 1.00000f);  // White
    //boidShader.uniform("color").set3f(0.05098f, 0.19608f, 0.30196f);  // Prussian Blue
    //boidShader.uniform("color").set3f(0.30980f, 0.00392f, 0.27843f);  // Tyrian Purple
    boidShader.uniform("color").set3f(0.71373f, 0.09020f, 0.29412f);  // Pictoral Carmine
    //boidShader.uniform("color").set3f(0.76471f, 0.92157f, 0.47059f);  // Yellow Green Crayon

    arrayBuffer.bind();
    arrayBuffer.instances(flockSize);

    // Boid model vertex buffer
    vertexBuffer.bind();
    vertexBuffer.construct<float>(nullptr, vertexBufferSize);  // Create a buffer of 10 floats
    vertexBuffer.construct(defaultModel.begin(), defaultModel.end());
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

    offsetBuffer.construct(offsetArray.get(), int32_t(4 * flockSize));
    arrayBuffer.attribute(2, GL_FLOAT, 4 * sizeof(float), 0, 1);
    arrayBuffer.attribute(2, GL_FLOAT, 4 * sizeof(float), 2 * sizeof(float), 1);
}

void Flock::update(float dt) {
    for (size_t i = 0; i < flockSize; i++) {
        m_secondaryFlock[i] = m_primaryFlock[i];
        Boid &currentBoid = m_secondaryFlock[i];

        Vector centerSteer {0.0f, 0.0f};
        // Precursor to Rectangle class
        if (currentBoid.position.x - Boid::scale <= -bounds.x
            || currentBoid.position.x + Boid::scale >= bounds.x
            || currentBoid.position.y - Boid::scale <= -bounds.y
            || currentBoid.position.y + Boid::scale >= bounds.y
        ) {
            centerSteer -= currentBoid.position;
            centerSteer = currentBoid.steer(centerSteer);
        }

        // Desire to move at full speed
        // This is be redundant when other forces are acting upon the boid, but that's not always the case.
        Vector fullSpeed = currentBoid.velocity;
        fullSpeed = currentBoid.steer(fullSpeed);

        Vector separation {0.0f, 0.0f};  // Desire to separate from flockmates
        Vector alignment {0.0f, 0.0f};  // Desire to align with the direction of other flockmates
        Vector cohesion {0.0f, 0.0f};  // Desire to shrink the distance between self and flockmates
        size_t cohesiveTotal = 0;
        size_t disruptiveTotal = 0;
        for(size_t j = 0; j < flockSize; j++) {
            if (i == j) {
                continue;
            }

            const Boid& otherBoid = m_primaryFlock[j];

            const float d = glm::fastDistance(currentBoid.position, otherBoid.position);
            if (d < Boid::disruptiveRadius && d > 0.0f) {
                Vector diff = currentBoid.position - otherBoid.position;
                separation += diff / (d * d);
                disruptiveTotal++;
            }

            if (d < Boid::cohesiveRadius) {
                alignment += otherBoid.velocity;
                cohesion += otherBoid.position;
                cohesiveTotal++;
            }
        }

        if (disruptiveTotal > 0) {
            separation /= static_cast<float>(cohesiveTotal);
            separation = currentBoid.steer(separation);
        }

        if (cohesiveTotal > 0) {
            const float countFactor = 1.0f / static_cast<float>(cohesiveTotal);
            alignment *= countFactor;
            alignment = currentBoid.steer(alignment);

            cohesion *= countFactor;
            cohesion -= currentBoid.position;
            cohesion = currentBoid.steer(cohesion);
        }

        currentBoid.acceleration += centerSteer * 0.8f;
        currentBoid.acceleration += fullSpeed * 0.00625f;
        currentBoid.acceleration += separation * 1.0f;
        currentBoid.acceleration += alignment * 0.6f;
        currentBoid.acceleration += cohesion * 0.4f;

        currentBoid.acceleration = magnitude(currentBoid.acceleration, Boid::maxForce);

        currentBoid.velocity += currentBoid.acceleration;
        currentBoid.position += currentBoid.velocity * dt;
        currentBoid.acceleration *= 0.0f;

        // Update the offsets
        offsetArray[i * 4 + 0] = currentBoid.position.x;
        offsetArray[i * 4 + 1] = currentBoid.position.y;

        // In terms of calculating a rotation,
        // sqrt seems to be faster than atan2
        // and saves cos and sin computations on the gpu
        const float magX = currentBoid.velocity.x;
        const float magY = currentBoid.velocity.y;
        const float l2 = magX * magX + magY * magY;
        const float magFactor = l2 != 0.0f ? glm::fastInverseSqrt(l2) : 0.0f;
        offsetArray[i * 4 + 2] = magX * magFactor;
        offsetArray[i * 4 + 3] = magY * magFactor;
    }

    offsetBuffer.bind();
    offsetBuffer.update(offsetArray.get(), int32_t(4 * flockSize));

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

void Flock::changeRenderMode(RenderMode mode) {
    switch(mode) {
        case RenderMode::Classic: {
            renderMode = lwvl::PrimitiveMode::Lines;
            indexCount = 10;
            std::array<uint8_t, 10> indexData {
                0, 1, 1, 2, 2, 3, 2, 0, 3, 0
            };
            indexBuffer.bind();
            indexBuffer.update(indexData.begin(), indexData.end());
            vertexBuffer.bind();
            vertexBuffer.update(
                defaultModel.begin(),
                defaultModel.end()
            );
            return;
        }
        case RenderMode::Filled: {
            renderMode = lwvl::PrimitiveMode::TriangleFan;
            indexCount = 4;
            std::array<uint8_t, 4> indexData {
                0, 1, 2, 3
            };
            indexBuffer.bind();
            indexBuffer.update(indexData.begin(), indexData.end());
            vertexBuffer.bind();
            vertexBuffer.update(
                defaultModel.begin(),
                defaultModel.end()
            );
            return;
        }
        default:
            return;
    }
}
