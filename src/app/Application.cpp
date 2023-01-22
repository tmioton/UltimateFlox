#include "pch.hpp"
#include "Core/Window/Window.hpp"
#include "Core/Lua/VirtualMachine.hpp"
#include "World/Flock.hpp"
//#include "Algorithm/DirectLoopAlgorithm.hpp"
//#include "Algorithm/QuadtreeAlgorithm.hpp"
#include "Algorithm/ThreadedAlgorithm.hpp"
#include "Render/Boid/FlockRenderer.hpp"
#include "Render/QuadtreeRenderer.hpp"

#include "binary_default_lua.cpp"

#ifndef NDEBUG
#define FLOX_SHOW_DEBUG_INFO
#endif

using namespace lwvl::debug;
using namespace std::chrono;
using namespace window;


/* Ideas:
  . Job system with local and global queues https://www.youtube.com/watch?v=1ZMasi_9g_A
  . Background texture displaying the path of the boid?
  . Compute shaders
  . Shader uniform buffers
  . Dev console
  . Lua scripting integration
  . Single large renderer class
    . Maintains small objects that represent a piece of the pipeline like a texture
    . Attach small objects to one "pipeline", can be easily swapped out
    . Helps with the "attach camera to every shader" issue.
    ? Objects like uniforms have "Static", "Dynamic", and "Stream" properties to mark how often they should be updated.

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

template<class Clock>
static inline double delta(time_point<Clock> start) {
    return 0.000001 * static_cast<double>(duration_cast<microseconds>(
        high_resolution_clock::now() - start
    ).count());
}

namespace app {
    struct WindowConfiguration {
        int width;
        int height;
    };
}


void runStartupScript(lua::VirtualMachine &L, size_t &flockSize, float &worldBound, app::WindowConfiguration &window) {
    L.addBasicLibraries();

    // Create config table for Lua customization.
    lua::Table appConfig{L.table("flox")};
    appConfig.create(4, 0);
    appConfig.pushInteger("flock_size", static_cast<int>(flockSize));
    appConfig.pushNumber("world_bound", worldBound);
    appConfig.pushInteger("width", window.width);
    appConfig.pushInteger("height", window.height);
    L.pushGlobal(appConfig);

    const bool validLua = [](lua::VirtualMachine& L) {
        lua::CodeFile customStart{"Data/Scripts/flox.lua"};
        int r = L.run(customStart);
        if (r == LUA_OK) {
            return true;
        }

        // Pop error message.
        std::cout << "Error from external script file:\n    " << L.toString() << std::endl;

        lua::CodeBuffer defaultStart{(const char *) (FLOX_DEFAULT_LUA_SCRIPT), FLOX_DEFAULT_LUA_SCRIPT_LENGTH};
        r = L.run(defaultStart);
        if (r == LUA_OK) {
            return true;
        }

        // Pop error message.
        std::cout << "Error from default script:\n    " << L.toString() << std::endl;

        return false;
    }(L);

    if (validLua && appConfig.push()) {
        flockSize = appConfig.toInteger("flock_size", flockSize);
        worldBound = appConfig.toNumber("world_bound", worldBound);
        window.width = appConfig.toInteger("width", window.width);
        window.height = appConfig.toInteger("height", window.height);
        appConfig.pop();
    }
}


int run() {
    size_t flockSize = 1024;
    float worldBound = 450.0f;
    app::WindowConfiguration wConfig{800, 450};

    //lua::OldVirtualMachine L;
    //runStartupScript(L, flockSize, worldBound, width, height);
    auto &L{lua::VirtualMachine::get()};
    runStartupScript(L, flockSize, worldBound, wConfig);

    //lua::Function onFrameStart{L.function("OnFrameStart", 0, 0)};
    //lua::Function onFrameGroupEnd{L.function("OnFrameGroupEnd", 0, 0)};

    //Window::init();
    //GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    //const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    //
    //glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    //glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    //glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    //glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    //
    //Window window(mode->width, mode->height, "Ultimate Flox", monitor);
    Window& window{Window::get()};
    window.create("Ultimate Flox", {wConfig.width, wConfig.height, 1, false});

    lwvl::Program::clear();
#ifdef FLOX_SHOW_DEBUG_INFO
    GLEventListener listener(
        [](
            Source source, Type type,
            Severity severity, unsigned int id, int length,
            const char *message, const void *userState
        ) {
            if (type != Type::OTHER){
                std::cout << "[OpenGL] " << message << std::endl;
            }
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
    bool renderQuadtreeColored = false;
    bool renderQuadtreeLines = false;

    const float aspect = static_cast<float>(wConfig.width) / static_cast<float>(wConfig.height);
    const Vector bounds {
        aspect >= 1.0f ? worldBound * aspect : worldBound,
        aspect < 1.0f ? worldBound * aspect : worldBound
    };
    Flock flock{flockSize};

    // Unused algorithms are dead code, but having them as components allows easier testing.
    //DirectLoopAlgorithm directLoopAlgorithm{bounds};
    //QuadtreeAlgorithm quadtreeAlgorithm{bounds};
    ThreadedAlgorithm threadedAlgorithm{bounds};

    //Algorithm* algorithm = &directLoopAlgorithm;
    //QuadtreeAlgorithm *qtAlgorithm = &quadtreeAlgorithm;
    QuadtreeAlgorithm *qtAlgorithm = &threadedAlgorithm;
    Algorithm *algorithm = qtAlgorithm;

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

#ifdef FLOX_SHOW_DEBUG_INFO
    std::cout << "Setup took " << delta(setupStart) << " seconds." << std::endl;
#endif
    auto secondStart = high_resolution_clock::now();
    auto frameStart = high_resolution_clock::now();

#ifdef FLOX_SHOW_DEBUG_INFO
    double eventDurationAverage = 0.0;
    double updateDurationAverage = 0.0;
    double renderUpdateDurationAverage = 0.0;
    double renderDurationAverage = 0.0;
#endif

    //L.pushNumber(1.0 / 60.0);
    //L.setGlobal("fps");

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int frameCount = 1; !window.should_close(); frameCount++) {
        // Calculate the time since last frame
        const auto dt = static_cast<float>(delta(frameStart));
        frameStart = high_resolution_clock::now();

        //if (onFrameStart.push()) {
        //    L.pushNumber(dt);
        //    L.setGlobal("delta");
        //    L.validate(onFrameStart.call());
        //}

#ifdef FLOX_SHOW_DEBUG_INFO
        auto averageStart = high_resolution_clock::now();
#endif

        // Fill event stack
        window.update();

        // Handle incoming events
        while (std::optional<Event> possible = window.poll_event()) {
            if (!possible.has_value()) {
                continue;
            }

            Event &concrete = possible.value();

            // Handle window close.
            if (concrete.type == Event::Type::KeyRelease
                && std::get<KeyboardEvent>(concrete.event).key == GLFW_KEY_ESCAPE
                ) {
                window.should_close(true);
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
                    }

                    // Boid keybinds.
                    if (key_event.key == GLFW_KEY_SPACE) {
                        paused ^= true;
                    } else if (key_event.key == GLFW_KEY_1) {
                        if (activeModel == &filledModel && activeShader == &defaultBoidShader) {
                            defaultBoidShader.nextColor();
                        } else {
                            activeModel = &filledModel;
                        }
                    } else if (key_event.key == GLFW_KEY_2) {
                        if (activeModel == &classicModel && activeShader == &defaultBoidShader) {
                            defaultBoidShader.nextColor();
                        } else {
                            activeModel = &classicModel;
                        }
                    } else if (key_event.key == GLFW_KEY_B) {
                        renderBoids ^= true;
                    } else if (key_event.key == GLFW_KEY_V) {
                        renderVision ^= true;
                    } else if (key_event.key == GLFW_KEY_Q) {
                        renderQuadtreeLines ^= true;
                    } else if (key_event.key == GLFW_KEY_C) {
                        renderQuadtreeColored ^= true;
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

#ifdef FLOX_SHOW_DEBUG_INFO
        eventDurationAverage += delta(averageStart);
        averageStart = high_resolution_clock::now();
#endif

        // Update engine
        bool doUpdates = !paused && !consoleOpen;
        if (doUpdates) {
            flock.update(algorithm, dt);
        }

#ifdef FLOX_SHOW_DEBUG_INFO
        updateDurationAverage += delta(averageStart);
        averageStart = high_resolution_clock::now();
#endif

        // Rendering
        if (renderBoids || renderVision) {
            renderer.update(flock.boids());
        }

        if (renderQuadtreeColored || renderQuadtreeLines) {
            qtRenderer.update(qtAlgorithm->tree());
        }

#ifdef FLOX_SHOW_DEBUG_INFO
        renderUpdateDurationAverage += delta(averageStart);
        averageStart = high_resolution_clock::now();
#endif

        lwvl::clear();

        if (renderQuadtreeColored || renderQuadtreeLines) {
            qtRenderer.draw(renderQuadtreeColored, renderQuadtreeLines);
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

        window.swap_buffers();

#ifdef FLOX_SHOW_DEBUG_INFO
        renderDurationAverage += delta(averageStart);
#endif

        if (delta(frameStart) <= 0.008) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Framerate display
        if ((frameCount & 0x3F) == 0) {
            double fps = 64.0 / delta(secondStart);
            //if (onFrameGroupEnd.push()) {
            //    L.pushInteger(frameCount);
            //    L.setGlobal("frame_count");
            //    L.pushNumber(fps);
            //    L.setGlobal("fps");
            //    L.validate(onFrameGroupEnd.call());
            //}

            secondStart = high_resolution_clock::now();
#ifdef FLOX_SHOW_DEBUG_INFO
            const auto frameth = 1.0 / static_cast<double>(frameCount);
            std::cout << "Average framerate for last " << frameCount << " frames: " << fps << " | " << 1.0 / fps << 's' << std::endl;
            std::cout << "Event updates: " << eventDurationAverage * frameth << "s, ";
            std::cout << "Flock updates: " << updateDurationAverage * frameth << "s, ";
            std::cout << "Rendering Updates: " << renderUpdateDurationAverage * frameth << "s, ";
            std::cout << "Rendering: " << renderDurationAverage * frameth << 's' << std::endl;
            std::cout << std::endl;

            // Reset time variables.
            eventDurationAverage = 0.0;
            updateDurationAverage = 0.0;
            renderUpdateDurationAverage = 0.0;
            renderDurationAverage = 0.0;
#endif
            frameCount = 0;
        }
    }
    return 0;
}

#ifdef WIN32
//#ifdef NDEBUG
//#include <windows.h>
//int WINAPI wWinMain(
//    HINSTANCE hInstance,     // HANDLE TO AN INSTANCE.
//    HINSTANCE hPrevInstance, // USELESS on modern windows (totally ignore hPrevInstance).
//    PWSTR szCmdLine,         // Command line arguments.
//    int iCmdShow             // Start window maximized, minimized, etc.
//)
//#else // NDEBUG
int wmain()
//#endif // NDEBUG
#else // WIN32
int main()
#endif // WIN32
{
    try {
        return run();
    } catch (const std::bad_alloc &e) {
        std::cerr << "Unable to allocate memory for program. Exiting." << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
