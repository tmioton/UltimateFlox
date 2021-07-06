#include "pch.hpp"
#include "GLDebug.hpp"
#include "Common.hpp"
#include "Core/Window.hpp"
#include "Flock.hpp"

#include <chrono>
#include <fstream>

using namespace lwvl::debug;
using namespace std::chrono;


// Ideas:
//   . Background texture displaying the path of the boid?
//   . Compute shaders
//   . Spatial partitioning


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
                std::cout << "[OpenGL] " << message << std::endl;
            }
        );

        const auto setupStart = high_resolution_clock::now();
#endif

        const auto width = static_cast<float>(m_window.config.width);
        const auto height = static_cast<float>(m_window.config.height);

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

        Flock flock(flockSize, width / height);

#ifndef NDEBUG
        std::cout << "Setup took " << delta(setupStart) << " seconds." << std::endl;
        auto secondStart = high_resolution_clock::now();
#endif
        auto frameStart = high_resolution_clock::now();

        bool paused = false;

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int frameCount = 0; !m_window.shouldClose(); frameCount++) {
            // Calculate the time since last frame
            const auto dt = static_cast<float>(delta(frameStart));
            frameStart = high_resolution_clock::now();

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
                    } else if (key_event.key == GLFW_KEY_SPACE) {
                        paused ^= true;
                    } else if (key_event.key == GLFW_KEY_1) {
                        flock.changeRenderMode(Flock::RenderMode::Filled);
                    } else if (key_event.key == GLFW_KEY_2) {
                        flock.changeRenderMode(Flock::RenderMode::Classic);
                    } else if (key_event.key == GLFW_KEY_B) {
                        flock.toggleBoidRendering();
                    } else if (key_event.key == GLFW_KEY_V) {
                        flock.toggleVisionRendering();
                    }
                }
            }

            // Update engine
            if (!paused) {
                flock.update(dt);
            }

            // Rendering
            lwvl::clear();
            flock.draw();
            m_window.swapBuffers();

            // Framerate display
            if ((frameCount & 63) == 0) {
#ifndef NDEBUG
                double fps = static_cast<double>(frameCount) / delta(secondStart);
                std::cout << "Average framerate for last " << frameCount << " frames: " << fps << std::endl;

                // Reset time variables.
                secondStart = high_resolution_clock::now();
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
