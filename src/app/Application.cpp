#include "pch.hpp"
#include "Core/Window/Window.hpp"
#include "Core/UserInterface/UserInterface.hpp"
#include "Core/UserInterface/UIDemoWindow.hpp"
#include "Core/UserInterface/UIConsole.hpp"
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

//#define FLOX_DEBUG_TIMINGS

using namespace lwvl::debug;
using namespace std::chrono;
using namespace window;
using namespace ui;


/* Ideas:
  . Job system with local and global queues https://www.youtube.com/watch?v=1ZMasi_9g_A
  . Background texture displaying the path of the boid?
  . Compute shaders
  . Shader uniform buffers
  . Dev console
  . Lua scripting integration
    . What does Lua offer us that a simple console command interface cannot?
      . User-customized handling of events.
        . We need an event system.
      . Ability to customize the boids on-the-fly. Color, name, etc.
        . Categories have to be supported by application, but user can define strategy.
      . Abstraction to allow custom steering strategies? Difficult.
    . Maybe we don't need a full Lua abstraction, just one that does exactly what we need here.
      . Look at a Lua implementation differently, like Data-Oriented vs Object-Oriented.
      . What does this look like?
      . Has to mean more than just "this one thing is programmed in raw Lua bindings."
        That kind of thinking leads to many difficult-to-change functions.
  . Renderer Resource Class
    . Maintains small objects that represent a piece of the pipeline like a texture
    . Attach small objects to one "pipeline", can be easily swapped out
    . Helps with the "attach camera to every shader" issue.
    ? Objects like uniforms have "Static", "Dynamic", and "Stream" properties to mark how often they should be updated.
  . Allow selecting boids with a rect.
    . Show options for individual boids.
      . Color picker
      ? Add a name
  . State machine to move between frame stages
    . Hazel's "Layers" are just states.

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


void
run_startup_script(lua::VirtualMachine &L, size_t &flockSize, float &worldBound, app::WindowConfiguration &window) {
    L.addBasicLibraries();

    // Create config table for Lua customization.
    lua::Table appConfig {L.table("flox")};
    appConfig.create(4, 0);
    appConfig.pushInteger("flock_size", static_cast<int>(flockSize));
    appConfig.pushNumber("world_bound", worldBound);
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
        spdlog::error("[Lua] Error from external script file:");
        spdlog::error("[Lua] {0}", L.toString());
        //std::cout << "Error from external script file:\n    " << L.toString() << std::endl;

        lua::CodeBuffer defaultStart {(const char *) (FLOX_DEFAULT_LUA_SCRIPT), FLOX_DEFAULT_LUA_SCRIPT_LENGTH};
        r = L.run(defaultStart);
        if (r == LUA_OK) {
            return true;
        }

        // Pop error message.
        spdlog::error("[Lua] Error from default script:");
        spdlog::error("[Lua] {0}", L.toString());

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
    size_t flock_size = 1024;
    float world_bound = 450.0f;
    app::WindowConfiguration w_config {800, 450};

    //lua::OldVirtualMachine L;
    //runStartupScript(L, flockSize, worldBound, width, height);
    auto &L {lua::VirtualMachine::get()};
    run_startup_script(L, flock_size, world_bound, w_config);

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
    Window &window {Window::get()};
    window.create("Ultimate Flox", {w_config.width, w_config.height, 1, false});
    UserInterface::get();
    window.extend(UserInterface::register_glfw);
    //UserInterface &user_interface {UserInterface::get()};

    UIDemoWindow demo_window;
    UIConsole console_window;

    lwvl::Program::clear();
#ifdef FLOX_SHOW_DEBUG_INFO
    GLEventListener listener(
        [](
            Source source, Type type,
            Severity severity, unsigned int id, int length,
            const char *message, const void *userState
        ) {
            if (type == Type::OTHER) {
                spdlog::debug("[OpenGL] {0}", message);
            } else if (type == Type::ERROR) {
                spdlog::error("[OpenGL] {0}", message);
            } else {
                spdlog::info("[OpenGL] {0}", message);
                //std::cout << "[OpenGL] " << message << std::endl;
            }
        }
    );

    const auto setupStart = high_resolution_clock::now();
#endif

    // Move all of this to Register type variables
    bool console_open = false;
    bool paused = false;
    bool debug_visual = false;
    bool render_boids = true;
    bool render_vision = false;
    bool render_quadtree_colored = false;
    bool render_quadtree_lines = false;
    bool show_demo_window = false;

    const float aspect = static_cast<float>(w_config.width) / static_cast<float>(w_config.height);
    const Vector bounds {
        aspect >= 1.0f ? world_bound * aspect : world_bound,
        aspect < 1.0f ? world_bound * aspect : world_bound
    };
    Flock flock {flock_size};

    // Unused algorithms are dead code, but having them as components allows easier testing.
    //DirectLoopAlgorithm directLoopAlgorithm{bounds};
    //QuadtreeAlgorithm quadtreeAlgorithm{bounds};
    ThreadedAlgorithm threaded_algorithm {bounds};

    //Algorithm* algorithm = &directLoopAlgorithm;
    //QuadtreeAlgorithm *qtAlgorithm = &quadtreeAlgorithm;
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
    //std::cout << "Setup took " << delta(setupStart) << " seconds." << std::endl;
    spdlog::debug("Setup took {0} seconds.", delta(setupStart));
#endif
    auto second_start = high_resolution_clock::now();
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

    //ImVec4 clear_color = ImVec4(0.76471f, 0.04314f, 0.30588f, 1.0f);
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

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
                        if (UserInterface::capture_keyboard()) { return; }
                        if (type != KeyRelease) { return; }

                        if (event.mods & GLFW_MOD_SHIFT && event.key == GLFW_KEY_ESCAPE) {
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
                            } else if (event.key == GLFW_KEY_U) {
                                show_demo_window ^= true;
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
        event_duration_average += delta(averageStart);
        averageStart = high_resolution_clock::now();
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
            high_resolution_clock::now() - averageStart
        ).count();
        if (total_frame_count > 1199) {
            if (total_frame_count == 1200) {
                spdlog::info("Started timing capture.");
                //std::cout << "Started timing capture." << std::endl;
                file << update_delta;
            } else {
                file << ',' << update_delta;
            }
        }
        ++total_frame_count;
#endif
#ifdef FLOX_SHOW_DEBUG_INFO
        update_duration_average += delta(averageStart);
#endif

#ifdef FLOX_SHOW_DEBUG_INFO
        averageStart = high_resolution_clock::now();
#endif

        // Rendering
        if (render_boids || render_vision) {
            renderer.update(flock.boids());
        }

        if (render_quadtree_colored || render_quadtree_lines) {
            quadtree_renderer.update(qt_algorithm->tree());
        }

#ifdef FLOX_SHOW_DEBUG_INFO
        render_update_duration_average += delta(averageStart);
#endif
        // GUI Update
        UserInterface::new_frame();
        if (show_demo_window) { UserInterface::update(&demo_window, &show_demo_window); }
        if (console_open) { UserInterface::update(&console_window, &console_open); }
        UserInterface::compile();

        //glViewport(0, 0, w_config.width, w_config.height);
        glClearColor(
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w
        );
        glClear(GL_COLOR_BUFFER_BIT);
#ifdef FLOX_SHOW_DEBUG_INFO
        averageStart = high_resolution_clock::now();
#endif

        //lwvl::clear();

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

        // GUI Render
        UserInterface::render();

        window.swap_buffers();

#ifdef FLOX_SHOW_DEBUG_INFO
        render_duration_average += delta(averageStart);
#endif

        if (delta(frame_start) <= 0.008) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        // Framerate display
        if ((frame_count & 0x3F) == 0) {
            double fps = 64.0 / delta(second_start);
            //if (onFrameGroupEnd.push()) {
            //    L.pushInteger(frameCount);
            //    L.setGlobal("frame_count");
            //    L.pushNumber(fps);
            //    L.setGlobal("fps");
            //    L.validate(onFrameGroupEnd.call());
            //}

            second_start = high_resolution_clock::now();
#ifdef FLOX_SHOW_DEBUG_INFO
            const auto frameth = 1.0 / static_cast<double>(frame_count);
            spdlog::debug("****** New Frame Group ******");
            spdlog::debug("Average over {0} frames: {1} fps | {2}s", frame_count, fps, 1.0 / fps);
            //std::cout << "Average framerate for last " << frame_count << " frames: " << fps << " | " << 1.0 / fps << 's'
            //          << '\n';
            spdlog::debug("Event updates: {0}s", event_duration_average * frameth);
            //std::cout << "Event updates: " << eventDurationAverage * frameth << "s, ";
            spdlog::debug("Flock updates: {0}s", update_duration_average * frameth);
            //std::cout << "Flock updates: " << updateDurationAverage * frameth << "s, ";
            spdlog::debug("Rendering updates: {0}s", render_update_duration_average * frameth);
            //std::cout << "Rendering Updates: " << renderUpdateDurationAverage * frameth << "s, ";
            spdlog::debug("Rendering: {0}s", render_duration_average * frameth);
            //std::cout << "Rendering: " << renderDurationAverage * frameth << 's' << '\n';
            //std::cout << '\n';

            // Reset time variables.
            event_duration_average = 0.0;
            update_duration_average = 0.0;
            render_update_duration_average = 0.0;
            render_duration_average = 0.0;
#endif
            frame_count = 0;
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
#ifdef FLOX_SHOW_DEBUG_INFO
    spdlog::set_level(spdlog::level::trace);
#endif
    try {
        return run();
    } catch (const std::bad_alloc &e) {
        spdlog::error("Unable to allocate memory for program. Exiting.");
        //std::cerr << "Unable to allocate memory for program. Exiting." << std::endl;
        return -1;
    } catch (const std::exception &e) {
        spdlog::error("{0}", e.what());
        //std::cerr << e.what() << std::endl;
        return -1;
    }
}
