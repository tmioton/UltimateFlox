#include "pch.hpp"
#include "GLDebug.hpp"
#include "Common.hpp"
#include "Core/Window.hpp"
#include "World/Flock.hpp"

#include <chrono>
#include <fstream>

using namespace lwvl::debug;
using namespace std::chrono;


// Ideas:
//   . Background texture displaying the path of the boid?
//   . Compute shaders
//   . Spatial partitioning
//   . Dev Console


// Commands stored as function pointers in a map
// We pass this worldState to all commands
class WorldState {
    size_t flockSize;

    Flock p_flock;
    DataBufferUpdater p_dataBuffer;
    BoidRenderer p_boidRenderer;
    VisionRenderer p_visionRenderer;

public:
    const int width, height;
    bool consoleOpen = false;
    bool paused = false;
    bool debugVisual = false;
    bool renderBoids = true;
    bool renderVision = false;

    WorldState(size_t size, int width, int height) :
        flockSize(size), p_flock(size, width, height), p_dataBuffer(size),
        p_boidRenderer(size, width, height, p_dataBuffer.buffer()),
        p_visionRenderer(size, width, height, p_dataBuffer.buffer()),
        width(width), height(height) {

    }

    Flock& flock() {
        return p_flock;
    }

    DataBufferUpdater& dataBuffer() {
        return p_dataBuffer;
    }

    BoidRenderer& boidRenderer() {
        return p_boidRenderer;
    }

    VisionRenderer& visionRenderer() {
        return p_visionRenderer;
    }
};


class Application {
    Window m_window;

public:
    Application(int width, int height) : m_window(width, height, "UltimateFlox") {}

    static inline double delta(time_point<steady_clock> start) {
        return 0.000001 * static_cast<double>(duration_cast<microseconds>(
            high_resolution_clock::now() - start
        ).count());
    }

    int run() {
#ifndef NDEBUG
        GLEventListener listener(
            [](
                Source source, Type type,
                Severity severity, unsigned int id, int length,
                const char *message, const void *userState
            ) {
                if (type != Type::OTHER) {
                    std::cout << "[OpenGL] " << message << std::endl;
                }
            }
        );

        const auto setupStart = high_resolution_clock::now();
#endif
        size_t flockSize = 512;
        try {
            std::ifstream file("flox.txt");
            if (!file) {
                throw std::ios_base::failure("");
            }

            std::stringstream output_stream;
            std::string line;
            while (std::getline(file, line)) {
                if (line.length() == 0) {
                    continue;
                }

                flockSize = std::stoull(line);
                break;
            }
        } catch(std::ios_base::failure &fb) {
            //std::cout << fb.what() << ". Defaulting to 8 boids." << std::endl;
            std::cout << "flox.txt not found. Defaulting to " << flockSize << " boids." << std::endl;
        } catch(std::invalid_argument &ia) {
            std::cout << "Could not convert contents of flock.txt. Defaulting to 8 boids." << std::endl;
        }

        auto width = m_window.config.width;
        auto height = m_window.config.height;
        WorldState state(flockSize, width, height);

#ifndef NDEBUG
        std::cout << "Setup took " << delta(setupStart) << " seconds." << std::endl;
        auto secondStart = high_resolution_clock::now();
#endif
        auto frameStart = high_resolution_clock::now();

#ifndef NDEBUG
        double eventDurationAverage = 0.0;
        double updateDurationAverage = 0.0;
        double renderDurationAverage = 0.0;
#endif

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int frameCount = 0; !m_window.shouldClose(); frameCount++) {
            Flock& flock = state.flock();
            DataBufferUpdater& dataBuffer = state.dataBuffer();
            BoidRenderer& bRender = state.boidRenderer();
            VisionRenderer& vRender = state.visionRenderer();

            // Calculate the time since last frame
            const auto dt = static_cast<float>(delta(frameStart));
            frameStart = high_resolution_clock::now();

#ifndef NDEBUG
            auto averageStart = high_resolution_clock::now();
#endif

            // Fill event stack
            Window::update();

            // Handle incoming events
            while (std::optional<Event> possible = m_window.pollEvent()) {
                if (!possible.has_value()) {
                    continue;
                }

                Event &concrete = possible.value();

                // Handle window close.
                if (concrete.type == Event::Type::KeyRelease
                    && std::get<KeyboardEvent>(concrete.event).key == GLFW_KEY_ESCAPE
                ) {
                    m_window.shouldClose(true);
                }

                // Handle events differently if the console's open.
                if (state.consoleOpen) {
                    if (concrete.type == Event::Type::KeyRelease) {
                        KeyboardEvent &key_event = std::get<KeyboardEvent>(concrete.event);

                        if (key_event.key == GLFW_KEY_ENTER) {
                            // End the line, interpret input, start new input stream
                            std::cout << std::endl;
                        } else if (key_event.key == GLFW_KEY_GRAVE_ACCENT) {
                            state.consoleOpen = false;
                        }
                    } else if (concrete.type == Event::Type::TextInput) {
                        TextEvent &text_event = std::get<TextEvent>(concrete.event);
                        char key_name = static_cast<char>(text_event.codepoint);

                        // Ignore the ~ key. Also don't catch it here.
                        if (key_name == '`' || key_name == '~') { continue; }

                        std::cout << key_name;
                    }
                }
                else {
                    if (concrete.type == Event::Type::KeyRelease) {
                        KeyboardEvent &key_event = std::get<KeyboardEvent>(concrete.event);

                        // Open console.
                        if (key_event.key == GLFW_KEY_GRAVE_ACCENT) {
                            state.consoleOpen = true;
                        } else

                        // Boid keybinds.
                        if (key_event.key == GLFW_KEY_SPACE) {
                            state.paused ^= true;
                        } else if (key_event.key == GLFW_KEY_1) {
                            bRender.changeRenderMode(BoidRenderer::RenderMode::Filled);
                        } else if (key_event.key == GLFW_KEY_2) {
                            bRender.changeRenderMode(BoidRenderer::RenderMode::Classic);
                        } else if (key_event.key == GLFW_KEY_B) {
                            state.renderBoids ^= true;
                        } else if (key_event.key == GLFW_KEY_V) {
                            state.renderVision ^= true;
                        } else if (key_event.key == GLFW_KEY_S) {
                            state.debugVisual ^= true;
                            if (state.debugVisual) {
                                bRender.changeControlMode(BoidRenderer::ControlMode::SpeedDebug);
                            } else {
                                bRender.changeControlMode(BoidRenderer::ControlMode::Default);
                            }
                        }
                    }
                }
            }

#ifndef NDEBUG
            eventDurationAverage += delta(averageStart);
            averageStart = high_resolution_clock::now();
#endif

            // Update engine
            bool doUpdates = !state.paused && !state.consoleOpen;
            if (doUpdates) {
                flock.update(dt);
            }

#ifndef NDEBUG
            updateDurationAverage += delta(averageStart);
            averageStart = high_resolution_clock::now();
#endif

            // Rendering
            if (doUpdates) {
                dataBuffer.update(flock.boids());
            }

            lwvl::clear();

            if (state.renderVision) {
                vRender.draw();
            }

            if (state.renderBoids) {
                bRender.draw();
            }

            m_window.swapBuffers();

#ifndef NDEBUG
            renderDurationAverage += delta(averageStart);
#endif

            // Framerate display
            if ((frameCount & 63) == 0) {
#ifndef NDEBUG
                double fps = static_cast<double>(frameCount) / delta(secondStart);
                std::cout << "Average framerate for last " << frameCount << " frames: " << fps << " | " << 1.0 / fps << 's' << std::endl;

                const auto frameth = 1.0 / static_cast<double>(frameCount);
                std::cout << "Event updates: " << eventDurationAverage * frameth << "s, ";
                std::cout << "Flock updates: " << updateDurationAverage * frameth << "s, ";
                std::cout << "Rendering: " << renderDurationAverage * frameth << 's' << std::endl;
                std::cout << std::endl;

                // Reset time variables.
                secondStart = high_resolution_clock::now();
                eventDurationAverage = 0.0;
                updateDurationAverage = 0.0;
                renderDurationAverage = 0.0;
#endif
                frameCount = 0;
            }
        }
        return 0;
    }
};


int main() {
    try {
        Application app(800, 600);
        return app.run();
    } catch (const std::bad_alloc &e) {
        std::cout << "Unable to allocate memory for program. Exiting." << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
