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

//#define FLOX_DEBUG_TIMINGS

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

spdlog is just a less readable output to the console.
 When we have something special to output to, then we'll use spdlog.

How do we handle Lua hooks? I want to be able to hook into the code before and after each step of making a frame.
  . Global state object?
  . State object passed to important objects?

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


void run_startup_script(
    lua::VirtualMachine &L, size_t &flock_size, float &world_bound,
    app::WindowConfiguration &window
) {
    L.addBasicLibraries();

    // Create config table for Lua customization.
    lua::Table appConfig {L.table("flox")};
    appConfig.create(4, 0);
    appConfig.pushInteger("flock_size", static_cast<int>(flock_size));
    appConfig.pushNumber("world_bound", world_bound);
    appConfig.pushInteger("width", window.width);
    appConfig.pushInteger("height", window.height);
    L.pushGlobal(appConfig);

    const bool validLua = [](lua::VirtualMachine &L) {
        lua::CodeFile customStart {"Data/Scripts/flox.lua"};
        int r = L.run(customStart);
        if (r == LUA_OK) {
            return true;
        }

        // Pop error message.
        std::cout << "Error from external script file:\n    " << L.toString() << std::endl;

        lua::CodeBuffer defaultStart {(const char *) (FLOX_DEFAULT_LUA_SCRIPT), FLOX_DEFAULT_LUA_SCRIPT_LENGTH};
        r = L.run(defaultStart);
        if (r == LUA_OK) {
            return true;
        }

        // Pop error message.
        std::cout << "Error from default script:\n    " << L.toString() << std::endl;

        return false;
    }(L);

    if (validLua && appConfig.push()) {
        flock_size = appConfig.toInteger("flock_size", flock_size);
        world_bound = appConfig.toNumber("world_bound", world_bound);
        window.width = appConfig.toInteger("width", window.width);
        window.height = appConfig.toInteger("height", window.height);
        appConfig.pop();
    }
}


int run() {
    size_t flock_size = 1024;
    float world_bound = 450.0f;
    app::WindowConfiguration window_configuration {800, 450};

    auto &L {lua::VirtualMachine::get()};
    run_startup_script(L, flock_size, world_bound, window_configuration);

    lua::Function lua_on_frame_start {L.function("OnFrameStart", 1, 0)};

    Window &window {Window::get()};
    window.create("Ultimate Flox", {window_configuration.width, window_configuration.height, 1, false});
    if (!window.created()) { return -1; }

    lwvl::Program::clear();
#ifdef FLOX_SHOW_DEBUG_INFO
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

    const auto setup_start = high_resolution_clock::now();
#endif

    // Move all of this to Register type variables
    bool console_open = false;
    bool paused = false;
    bool debug_visual = false;
    bool render_boids = true;
    bool render_vision = false;
    bool render_quadtree_colored = false;
    bool render_quadtree_lines = false;

    const float aspect = static_cast<float>(window_configuration.width) / static_cast<float>(window_configuration.height);
    const Vector bounds {
        aspect >= 1.0f ? world_bound * aspect : world_bound,
        aspect < 1.0f ? world_bound * aspect : world_bound
    };
    Flock flock {flock_size};

    // Unused algorithms are dead code, but having them as components allows easier testing.
    //DirectLoopAlgorithm direct_loop_algorithm{bounds};
    //QuadtreeAlgorithm quadtree_algorithm{bounds};
    ThreadedAlgorithm threaded_algorithm {bounds};

    //Algorithm* algorithm = &direct_loop_algorithm;
    //QuadtreeAlgorithm *qt_algorithm = &quadtree_algorithm;
    ThreadedAlgorithm *qt_algorithm = &threaded_algorithm;
    Algorithm *algorithm = qt_algorithm;

    Projection projection {
        1.0f / bounds.x, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / bounds.y, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    FlockRenderer renderer {flock_size};
    QuadtreeRenderer quadtree_renderer {projection};

    Model classic_model {
        loadObject(
            boidShape.data(), boidShape.size(),
            classicIndices.data(), classicIndices.size(),
            lwvl::PrimitiveMode::Lines
        ), flock_size
    };
    Model filled_model {
        loadObject(
            boidShape.data(), boidShape.size(),
            filledIndices.data(), filledIndices.size(),
            lwvl::PrimitiveMode::Triangles
        ), flock_size
    };
    Model vision_model {
        loadObject(
            visionShape.data(), visionShape.size(),
            visionIndices.data(), visionIndices.size(),
            lwvl::PrimitiveMode::Lines
        ), flock_size
    };
    Model *active_model = &filled_model;

    renderer.attachData(&classic_model);
    renderer.attachData(&filled_model);
    renderer.attachData(&vision_model);

    DefaultBoidShader default_boid_shader {projection};
    SpeedDebugShader speed_debug_shader {projection};
    VisionShader vision_shader {projection};
    BoidShader *active_shader = &default_boid_shader;

#ifdef FLOX_SHOW_DEBUG_INFO
    std::cout << "Setup took " << delta(setup_start) << " seconds." << std::endl;
    auto second_start = high_resolution_clock::now();
#endif
    auto frame_start = high_resolution_clock::now();

#ifdef FLOX_SHOW_DEBUG_INFO
    double event_duration_average = 0.0;
    double update_duration_average = 0.0;
    double render_update_duration_average = 0.0;
    double render_duration_average = 0.0;
#endif

    //L.pushNumber(1.0 / 60.0);
    //L.setGlobal("fps");

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

#ifdef FLOX_DEBUG_TIMINGS
    std::ofstream file;
    file.open("UltimateFlox - Frame Times.txt");
    int total_frame_count = 0;
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int frame_count = 1; !window.should_close(); frame_count++) {
        // Calculate the time since last frame
        const auto dt = static_cast<float>(delta(frame_start));
        frame_start = high_resolution_clock::now();

#ifdef FLOX_DEBUG_TIMINGS
        if (total_frame_count >= 1800) {
            window.should_close(true);
            continue;
        }
#endif


        if (lua_on_frame_start.push()) {
            L.pushNumber(dt);
            //L.validate(onFrameStart.call());
            L.log(lua_on_frame_start.call());
            // Nothing left on stack after call.
        }

#ifdef FLOX_SHOW_DEBUG_INFO
        auto average_start = high_resolution_clock::now();
#endif

        // Fill event stack
        window.update();

        // Handle incoming events
        while (std::optional<Event> possible = window.poll_event()) {
            // This is where we need an event system with the ability to consume events
            // bool handle_event(...) { return true; }

            if (!possible.has_value()) {
                continue;
            }

            Event &concrete = possible.value();
            switch (concrete.type) {
                using
                enum Event::Type;
                case KeyPress:
                case KeyRelease:
                case KeyRepeat:
                    [&](KeyboardEvent &event, Event::Type type) {
                        if (type != KeyRelease) { return; }

                        //if (event.mods & GLFW_MOD_SHIFT && event.key == GLFW_KEY_ESCAPE) {
                        if (event.key == GLFW_KEY_ESCAPE) {
                            window.should_close(true);
                            return;
                        }

                        if (event.key == GLFW_KEY_GRAVE_ACCENT) {
                            console_open ^= true;
                            return;
                        }

                        if (!console_open) {
                            // Boid keybinds.
                            if (event.key == GLFW_KEY_SPACE) {
                                paused ^= true;
                            } else if (event.key == GLFW_KEY_1) {
                                if (active_model == &filled_model && active_shader == &default_boid_shader) {
                                    default_boid_shader.nextColor();
                                } else {
                                    active_model = &filled_model;
                                }
                            } else if (event.key == GLFW_KEY_2) {
                                if (active_model == &classic_model && active_shader == &default_boid_shader) {
                                    default_boid_shader.nextColor();
                                } else {
                                    active_model = &classic_model;
                                }
                            } else if (event.key == GLFW_KEY_B) {
                                render_boids ^= true;
                            } else if (event.key == GLFW_KEY_V) {
                                render_vision ^= true;
                            } else if (event.key == GLFW_KEY_Q) {
                                render_quadtree_lines ^= true;
                            } else if (event.key == GLFW_KEY_C) {
                                render_quadtree_colored ^= true;
                            } else if (event.key == GLFW_KEY_S) {
                                debug_visual ^= true;
                                if (debug_visual) {
                                    active_shader = &speed_debug_shader;
                                } else {
                                    active_shader = &default_boid_shader;
                                }
                            }
                        }
                    }(std::get<KeyboardEvent>(concrete.event), concrete.type);
                    break;
                case Event::Type::TextInput:

                case Event::Type::MouseMotion:

                case Event::Type::MouseDown:
                case Event::Type::MouseUp:break;
            }
        }

#ifdef FLOX_SHOW_DEBUG_INFO
        event_duration_average += delta(average_start);
        average_start = high_resolution_clock::now();
#else
#ifdef FLOX_DEBUG_TIMINGS
        auto averageStart = high_resolution_clock::now();
#endif
#endif

        // Update engine
        bool do_updates = !paused && !console_open;
        if (do_updates) {
            flock.update(algorithm, dt);
        }

#ifdef FLOX_DEBUG_TIMINGS
        auto update_delta = duration_cast<microseconds>(
            high_resolution_clock::now() - average_start
        ).count();
        if (total_frame_count > 1199) {
            if (total_frame_count == 1200) {
                std::cout << "Started timing capture." << std::endl;
                file << update_delta;
            } else {
                file << ',' << update_delta;
            }
        }
        ++total_frame_count;
#endif
#ifdef FLOX_SHOW_DEBUG_INFO
        update_duration_average += delta(average_start);
        average_start = high_resolution_clock::now();
#endif

        // Rendering
        if (render_boids || render_vision) {
            renderer.update(flock.boids());
        }

        if (render_quadtree_colored || render_quadtree_lines) {
            quadtree_renderer.update(qt_algorithm->tree());
        }

#ifdef FLOX_SHOW_DEBUG_INFO
        render_update_duration_average += delta(average_start);
        average_start = high_resolution_clock::now();
#endif

        lwvl::clear();

        if (render_quadtree_colored || render_quadtree_lines) {
            quadtree_renderer.draw(render_quadtree_colored, render_quadtree_lines);
        }

        if (render_vision) {
            vision_shader.radius(Boid::cohesiveRadius);
            FlockRenderer::draw(&vision_model, &vision_shader);
            vision_shader.radius(Boid::disruptiveRadius);
            FlockRenderer::draw(&vision_model, &vision_shader);
        }

        if (render_boids) {
            FlockRenderer::draw(active_model, active_shader);
        }

        window.swap_buffers();

#ifdef FLOX_SHOW_DEBUG_INFO
        render_duration_average += delta(average_start);
#endif

        if (delta(frame_start) <= 0.008) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Framerate display
        if ((frame_count & 0x3F) == 0) {
#ifdef FLOX_SHOW_DEBUG_INFO
            double fps = 64.0 / delta(second_start);
            second_start = high_resolution_clock::now();
            const auto frameth = 1.0 / static_cast<double>(frame_count);
            std::cout << "Average framerate for last " << frame_count << " frames: " << fps << " | " << 1.0 / fps << 's'
                      << '\n';
            std::cout << "Event updates: " << event_duration_average * frameth << "s, ";
            std::cout << "Flock updates: " << update_duration_average * frameth << "s, ";
            std::cout << "Rendering Updates: " << render_update_duration_average * frameth << "s, ";
            std::cout << "Rendering: " << render_duration_average * frameth << 's' << '\n';
            std::cout << '\n';

            // Reset time variables.
            event_duration_average = 0.0;
            update_duration_average = 0.0;
            render_update_duration_average = 0.0;
            render_duration_average = 0.0;
#endif
            frame_count = 0;
        }
    }

    lua::Function lua_on_exit {L.function("OnExit", 0, 0)};
    if (lua_on_exit.push()) {
        L.log(lua_on_exit.call());
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
