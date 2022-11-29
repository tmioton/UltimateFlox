#include "pch.hpp"
#include "Core/Window.hpp"
#include "World/Flock.hpp"
//#include "Algorithm/DirectLoopAlgorithm.hpp"
#include "Algorithm/QuadtreeAlgorithm.hpp"
#include "Render/Boid/FlockRenderer.hpp"
#include "Render/QuadtreeRenderer.hpp"

#include <chrono>
#include <fstream>

using namespace lwvl::debug;
using namespace std::chrono;


// Ideas:
//   . Background texture displaying the path of the boid?
//   . Compute shaders
//   . Spatial partitioning
//   . Dev Console

static inline double delta(time_point<steady_clock> start) {
    return 0.000001 * static_cast<double>(duration_cast<microseconds>(
        high_resolution_clock::now() - start
    ).count());
}


int run(int width, int height) {
    Window window(width, height, "Ultimate Flox");

    lwvl::Program::clear();
#ifndef NDEBUG
    GLEventListener listener(
            [](
                Source source, Type type,
                Severity severity, unsigned int id, int length,
                const char *message, const void *userState
            ) {
                //if (type != Type::OTHER){
                    std::cout << "[OpenGL] " << message << std::endl;
                //}
            }
        );

        const auto setupStart = high_resolution_clock::now();
#endif

    size_t flockSize = 1024;
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
    } catch (std::ios_base::failure &fb) {
        //std::cout << fb.what() << ". Defaulting to 8 boids." << std::endl;
        std::cout << "flox.txt not found. Defaulting to " << flockSize << " boids." << std::endl;
    } catch (std::invalid_argument &ia) {
        std::cout << "Could not convert contents of flock.txt. Defaulting to 8 boids." << std::endl;
    }

    // Move all of this to Register type variables
    bool consoleOpen = false;
    bool paused = false;
    bool debugVisual = false;
    bool renderBoids = true;
    bool renderVision = false;
    bool renderQuadtree = false;

    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    const Vector bounds{calculateBounds(aspect)};
    Flock flock{flockSize};
    //DirectLoopAlgorithm directLoopAlgorithm{bounds};
    QuadtreeAlgorithm quadtreeAlgorithm{bounds};
    //Algorithm* algorithm = &directLoopAlgorithm;
    Algorithm *algorithm = &quadtreeAlgorithm;

    Projection projection{
        1.0f / bounds.x, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / bounds.y, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    FlockRenderer renderer{flockSize};
    QuadtreeRenderer qtRenderer{projection};

    Model classicModel{loadObject(
        boidShape.data(), boidShape.size(),
        classicIndices.data(), classicIndices.size(),
        lwvl::PrimitiveMode::Lines
    ), flockSize};
    Model filledModel{loadObject(
        boidShape.data(), boidShape.size(),
        filledIndices.data(), filledIndices.size(),
        lwvl::PrimitiveMode::Triangles
    ), flockSize};
    Model visionModel{loadObject(
        visionShape.data(), visionShape.size(),
        visionIndices.data(), visionIndices.size(),
        lwvl::PrimitiveMode::Lines
    ), flockSize};
    Model *activeModel = &filledModel;

    renderer.attachData(&classicModel);
    renderer.attachData(&filledModel);
    renderer.attachData(&visionModel);

    DefaultBoidShader defaultBoidShader{projection};
    SpeedDebugShader speedDebugShader{projection};
    VisionShader visionShader{projection};
    BoidShader *activeShader = &defaultBoidShader;

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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int frameCount = 0; !window.shouldClose(); frameCount++) {
        // Calculate the time since last frame
        const auto dt = static_cast<float>(delta(frameStart));
        frameStart = high_resolution_clock::now();

#ifndef NDEBUG
        auto averageStart = high_resolution_clock::now();
#endif

        // Fill event stack
        Window::update();

        // Handle incoming events
        while (std::optional<Event> possible = window.pollEvent()) {
            if (!possible.has_value()) {
                continue;
            }

            Event &concrete = possible.value();

            // Handle window close.
            if (concrete.type == Event::Type::KeyRelease
                && std::get<KeyboardEvent>(concrete.event).key == GLFW_KEY_ESCAPE
                ) {
                window.shouldClose(true);
            }

            // Handle events differently if the console's open.
            if (consoleOpen) {
                if (concrete.type == Event::Type::KeyRelease) {
                    auto &key_event = std::get<KeyboardEvent>(concrete.event);

                    if (key_event.key == GLFW_KEY_ENTER) {
                        // End the line, interpret input, start new input stream
                        std::cout << std::endl;
                    } else if (key_event.key == GLFW_KEY_GRAVE_ACCENT) {
                        consoleOpen = false;
                    }
                } else if (concrete.type == Event::Type::TextInput) {
                    auto &text_event = std::get<TextEvent>(concrete.event);
                    char key_name = static_cast<char>(text_event.codepoint);

                    // Ignore the ~ key. Also don't catch it here.
                    if (key_name == '`' || key_name == '~') { continue; }

                    std::cout << key_name;
                }
            } else {
                if (concrete.type == Event::Type::KeyRelease) {
                    auto &key_event = std::get<KeyboardEvent>(concrete.event);

                    // Open console.
                    if (key_event.key == GLFW_KEY_GRAVE_ACCENT) {
                        consoleOpen = true;
                    } else

                        // Boid keybinds.
                    if (key_event.key == GLFW_KEY_SPACE) {
                        paused ^= true;
                    } else if (key_event.key == GLFW_KEY_1) {
                        activeModel = &filledModel;
                    } else if (key_event.key == GLFW_KEY_2) {
                        activeModel = &classicModel;
                    } else if (key_event.key == GLFW_KEY_B) {
                        renderBoids ^= true;
                    } else if (key_event.key == GLFW_KEY_V) {
                        renderVision ^= true;
                    } else if (key_event.key == GLFW_KEY_Q) {
                        renderQuadtree ^= true;
                    } else if (key_event.key == GLFW_KEY_S) {
                        debugVisual ^= true;
                        if (debugVisual) {
                            activeShader = &speedDebugShader;
                        } else {
                            activeShader = &defaultBoidShader;
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
        bool doUpdates = !paused && !consoleOpen;
        if (doUpdates) {
            flock.update(algorithm, dt);
        }

#ifndef NDEBUG
        updateDurationAverage += delta(averageStart);
            averageStart = high_resolution_clock::now();
#endif

        // Rendering
        if (doUpdates) {
            if (renderBoids || renderVision) {
                renderer.update(flock.boids());
            }

            if (renderQuadtree) {
                qtRenderer.update(quadtreeAlgorithm.tree());
            }
        }

        lwvl::clear();

        if (renderQuadtree) {
            qtRenderer.draw();
        }

        if (renderVision) {
            visionShader.radius(Boid::cohesiveRadius);
            FlockRenderer::draw(&visionModel, &visionShader);
            visionShader.radius(Boid::disruptiveRadius);
            FlockRenderer::draw(&visionModel, &visionShader);
        }

        if (renderBoids) {
            FlockRenderer::draw(activeModel, activeShader);
        }

        window.swapBuffers();

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


int main() {
    try {
        return run(800, 450);
        //return run(1920, 1080);
    } catch (const std::bad_alloc &e) {
        std::cout << "Unable to allocate memory for program. Exiting." << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
