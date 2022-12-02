#include "pch.hpp"
#include "Core/Window.hpp"
#include "Core/Lua/VirtualMachine.hpp"
#include "World/Flock.hpp"
//#include "Algorithm/DirectLoopAlgorithm.hpp"
#include "Algorithm/QuadtreeAlgorithm.hpp"
#include "Render/Boid/FlockRenderer.hpp"
#include "Render/QuadtreeRenderer.hpp"

#include <chrono>
#include <fstream>

using namespace lwvl::debug;
using namespace std::chrono;


/* Ideas:
  . Background texture displaying the path of the boid?
  . Compute shaders
  . Shader uniform buffers
  . Dev console
  . Lua scripting integration

How do we handle Lua hooks?
Global state object?
State object passed to important objects?
I want to be able to hook into the code before and after each step of making a frame

VM::call("<name of function in lua>")

Every object puts data it wants changeable into a Data object derived from the base Data object?
Every object gets a reference to a central system object.
Every object can issue a call to the LuaHook(name) method of the system object.
  . name is the name of a function possibly defined inside a Lua script.
  . If this function is defined in a Lua script, that function gets called.
Use a script to allow the user to set most customization points without recompiling.
  . Window size
  . Flock size
  . Render controls
*/

static inline double delta(time_point<steady_clock> start) {
    return 0.000001 * static_cast<double>(duration_cast<microseconds>(
        high_resolution_clock::now() - start
    ).count());
}


int run() {
    size_t flockSize = 1024;
    float worldBound = 450.0f;
    int width = 800;
    int height = 450;

    //std::string cmd {"a = 7 + 11"};
    lua::VirtualMachine L;
    L.addCommonLibraries();
    lua_State* raw = lua::raw(L);  // Do things manually while working on the abstraction.

    // Create config table for Lua customization.
    lua::Table configTable{L.table("flox")};
    configTable.create(4, 0);
    configTable.setInteger("flock_size", static_cast<int>(flockSize));
    configTable.setNumber("world_bound", worldBound);
    configTable.setInteger("width", width);
    configTable.setInteger("height", height);
    L.setGlobal(configTable);  // Consumes the table off the stack

    //int r = L.runString(cmd);
    int r = L.runFile("Data/Scripts/flox.lua");
    if (L.validate(r)) {
        if (configTable.get()) {
            flockSize = configTable.getInteger("flock_size", flockSize);
            worldBound = static_cast<float>(configTable.getNumber("world_bound", worldBound));
            width = configTable.getInteger("width", width);
            height = configTable.getInteger("height", height);
            L.pop(1);  // Need to explicitly pop the table off the stack.
        }
    }

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

    // Move all of this to Register type variables
    bool consoleOpen = false;
    bool paused = false;
    bool debugVisual = false;
    bool renderBoids = true;
    bool renderVision = false;
    bool renderQuadtree = false;

    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    const Vector bounds {
        aspect >= 1.0f ? worldBound * aspect : worldBound,
        aspect < 1.0f ? worldBound * aspect : worldBound
    };
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
#endif
    auto secondStart = high_resolution_clock::now();
    auto frameStart = high_resolution_clock::now();

#ifndef NDEBUG
    double eventDurationAverage = 0.0;
    double updateDurationAverage = 0.0;
    double renderDurationAverage = 0.0;
#endif

    lua_pushnumber(raw, 1.0f / 60.0f);
    lua_setglobal(raw, "fps");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int frameCount = 0; !window.shouldClose(); frameCount++) {
        // Calculate the time since last frame
        const auto dt = static_cast<float>(delta(frameStart));
        frameStart = high_resolution_clock::now();

        lua_getglobal(raw, "OnFrameStart");
        if (lua_isfunction(raw, -1)) {
            lua_pushnumber(raw, dt);
            lua_setglobal(raw, "delta");
            L.validate(lua_pcall(raw, 0, 0, 0));
        } else {
            lua_pop(raw, 1);
        }

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
            double fps = static_cast<double>(frameCount) / delta(secondStart);
            lua_getglobal(raw, "OnFrameGroupEnd");
            if (lua_isfunction(raw, -1)) {
                lua_pushinteger(raw, frameCount);
                lua_setglobal(raw, "frame_count");
                lua_pushnumber(raw, fps);
                lua_setglobal(raw, "fps");
                L.validate(lua_pcall(raw, 0, 0, 0));
            } else {
                lua_pop(raw, 1);
            }

            secondStart = high_resolution_clock::now();
#ifndef NDEBUG
                std::cout << "Average framerate for last " << frameCount << " frames: " << fps << " | " << 1.0 / fps << 's' << std::endl;

                const auto frameth = 1.0 / static_cast<double>(frameCount);
                std::cout << "Event updates: " << eventDurationAverage * frameth << "s, ";
                std::cout << "Flock updates: " << updateDurationAverage * frameth << "s, ";
                std::cout << "Rendering: " << renderDurationAverage * frameth << 's' << std::endl;
                std::cout << std::endl;

                // Reset time variables.
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
        return run();
    } catch (const std::bad_alloc &e) {
        std::cout << "Unable to allocate memory for program. Exiting." << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
