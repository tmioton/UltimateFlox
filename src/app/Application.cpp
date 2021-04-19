#include "pch.hpp"
#include "GLDebug.hpp"
#include "Common.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"
#include "Shader.hpp"
#include "Core/Window.hpp"

#include <chrono>
#include <fstream>

using namespace lwvl::debug;
using namespace std::chrono;

namespace constants {
    constexpr float tau = 6.283185307179586f;
    constexpr float pi = 3.141592653589793f;
}


struct Vector {
    float x = 0.0f;
    float y = 0.0f;
};


struct Boid {
    Vector position, velocity, acceleration;
};


class Flock {
    static constexpr size_t flockSize = 8;

    std::unique_ptr<Boid[]> m_primaryFlock = std::make_unique<Boid[]>(flockSize);
    std::unique_ptr<Boid[]> m_secondaryFlock = std::make_unique<Boid[]>(flockSize);

    lwvl::ShaderProgram boidShader;
    lwvl::VertexArray arrayBuffer;
    lwvl::ArrayBuffer vertexBuffer{lwvl::Usage::Static};
    lwvl::ElementBuffer indexBuffer{lwvl::Usage::Static};
    lwvl::ArrayBuffer offsetBuffer{lwvl::Usage::Static};

public:
    explicit Flock(float aspect) {
        // Setup boid starting locations
        constexpr float tauOverSize = constants::tau / static_cast<float>(flockSize);
        for (size_t i = 0; i < flockSize; i++) {
            Boid& boid = m_primaryFlock[i];
            auto slice = static_cast<float>(i);
            boid.position.x = 50.0f * std::cosf(slice * tauOverSize);
            boid.position.y = 50.0f * std::sinf(slice * tauOverSize);
        }

        // Construct shader
        lwvl::VertexShader vertexSource{lwvl::VertexShader::readFile("Data/Shaders/boid.vert")};
        lwvl::FragmentShader fragmentSource{lwvl::FragmentShader::readFile("Data/Shaders/boid.frag")};
        boidShader.link(vertexSource, fragmentSource);

        boidShader.bind();
        boidShader.uniform("scale").set1f(10.0f);
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
        std::array<float, 8> vertexData {
            std::cosf(0.0f), std::sinf(0.0f),
            std::cosf(3.0f * piOver4), std::sinf(3.0f * piOver4),
            0.5f * std::cosf(constants::pi), 0.5f * std::sinf(constants::pi),
            std::cosf(5.0f * piOver4), std::sinf(5.0f * piOver4)
        };
        vertexBuffer.construct(vertexData.begin(), vertexData.end());
        arrayBuffer.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

        // Boid model indexing buffer
        indexBuffer.bind();
        std::array<uint8_t, 10> indexData {
            0, 1, 1, 2, 2, 0, 2, 3, 3, 0
        };

        indexBuffer.construct(indexData.begin(), indexData.end());

        // Per-boid offset buffer
        offsetBuffer.bind();
        std::array<float, 2 * flockSize> offsetData {};
        for(size_t i = 0; i < flockSize; i++) {
            const Boid& boid = m_primaryFlock[i];
            offsetData[i * 2 + 0] = boid.position.x;
            offsetData[i * 2 + 1] = boid.position.y;
        }

        offsetBuffer.construct(offsetData.begin(), offsetData.end());
        arrayBuffer.attribute(2, GL_FLOAT, 2 * sizeof(float), 0, 1);
    }

    // Main meat of the program.
    void update(float dt) {

    }

    void draw() {
        boidShader.bind();
        arrayBuffer.bind();
        arrayBuffer.drawElements(
            lwvl::PrimitiveMode::Lines, 10,
            lwvl::ByteFormat::UnsignedByte
        );
    }
};


class Application {
    Window m_window;

public:
    Application(uint32_t width, uint32_t height) : m_window(width, height, "UltimateFlox") {}

    int run() {
        GLEventListener listener(
            [](
                Source source, Type type,
                Severity severity, unsigned int id, int length,
                const char *message, const void *userState
            ) {
                std::cout << "[OpenGL] " << message << std::endl;
            }
        );

        const auto width = static_cast<float>(m_window.config.width);
        const auto height = static_cast<float>(m_window.config.height);
        Flock flock(width / height);

        auto secondStart = high_resolution_clock::now();
        const auto getDuration = [&secondStart](){
            return 0.000001 * static_cast<double>(duration_cast<microseconds>(high_resolution_clock::now() - secondStart).count());
        };

        for (uint32_t frameCount = 0; !m_window.shouldClose(); frameCount++) {
            // Fill event stack
            Window::update();

            // Handle incoming events
            while (std::optional<Event> possible = m_window.pollEvent()) {
                if (!possible.has_value()) {
                    continue;
                }

                Event &concrete = possible.value();
                if (concrete.type == Event::Type::KeyRelease) {
                    KeyboardEvent &key_event = std::get<KeyboardEvent>(concrete.event);
                    if (key_event.key == GLFW_KEY_ESCAPE) {
                        m_window.shouldClose(true);
                    }
                }
            }

            // Update engine
            //const auto dt = static_cast<float>(getDuration());
            //flock.update(dt);

            // Rendering
            lwvl::clear();
            flock.draw();
            m_window.swapBuffers();

            // Framerate display
            if ((frameCount & 63) == 0) {
                double fps = static_cast<double>(frameCount) / getDuration();
                std::cout << "Average framerate for last " << frameCount << " frames : " << fps << std::endl;

                // Reset time variables.
                secondStart = high_resolution_clock::now();
                frameCount = 0;
            }
        }

        return 0;
    }
};


int main() {
    try {
        Application app(384, 384);
        return app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
