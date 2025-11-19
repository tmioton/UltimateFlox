#include "pch.hpp"

import Input;
import Event;
import InputDispatcher;
import Window;

class KeyHandler final : public core::InputLayer {
public:
    bool handle_event(const input::Event &event) override {
        using enum input::Event::Type;
        switch (event.type) {
            case Key: {
                std::cout << input::to_string(event.key.key);
                if (event.key.action == input::Activation::Press) {
                    std::cout << " pressed.\n";
                } else {
                    std::cout << " released.\n";
                }
            }
            default:
                return false;
        }
    }
};


class ImGUI {
    ImGuiContext* ctx;
    const window::Window* window;

    static ImGuiContext* setup() {
        IMGUI_CHECKVERSION();
        return ImGui::CreateContext();
    }

    class ContextGuard {
        ImGuiContext* prev {nullptr};
    public:
        explicit ContextGuard(ImGuiContext* ctx) : prev(ImGui::GetCurrentContext()) {
            ImGui::SetCurrentContext(ctx);
        }

        ContextGuard(ContextGuard&&) = delete;
        ContextGuard& operator=(ContextGuard&&) = delete;
        ContextGuard(const ContextGuard&) = delete;
        ContextGuard& operator=(const ContextGuard&) = delete;

        ~ContextGuard() {
            if (prev != nullptr) {
                ImGui::SetCurrentContext(prev);
            }
        }
    };
public:
    explicit ImGUI(const window::Window* const window) : ctx(setup()), window(window) {
        const auto cg {activate()};
        auto& io {ImGui::GetIO()};
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

        // Prevent automatic settings saving.
        io.IniFilename = nullptr;

        // Set up Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // try {
        //     io.Fonts->AddFontFromFileTTF("Data/Fonts/ClearSans.ttf", 18.0f);
        // } catch (const std::exception& e) {
        //     std::cerr << "Unable to load custom font." << '\n';
        // }

        // Set up scaling
        const auto scale {window->scale().x};
        ImGuiStyle &style      = ImGui::GetStyle();
        style.ScaleAllSizes(scale);
        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style and calling this again)
        style.FontScaleDpi = scale;
        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

        ImGui_ImplGlfw_InitForOpenGL(window->glfw_window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    ContextGuard activate() const {
        return ContextGuard(ctx);
    }

    void new_frame() const {
        const auto cg {activate()};

        // Start the Dear ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void construct() const {
        const auto cg {activate()};
        ImGui::Render();
    }

    void render() const {
        const auto cg {activate()};
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    ~ImGUI() {
        const auto cg {activate()};
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};


class Engine {
public:
    entt::dispatcher &dispatcher;
    core::InputDispatcher input_dispatcher {};
    std::unique_ptr<window::Window> window;
    std::unique_ptr<ImGUI> im_gui;
    std::unique_ptr<KeyHandler> key_handler;

    explicit Engine(window::Hints &&hints) : dispatcher{entt::locator<entt::dispatcher>::emplace()} {
        window = std::make_unique<window::Window>(&input_dispatcher, hints);
        key_handler = std::make_unique<KeyHandler>();
        input_dispatcher.push_layer(key_handler.get());
        im_gui = std::make_unique<ImGUI>(window.get());
    }

    ~Engine() {
        input_dispatcher.clear();
    }
};

struct UIData {
    bool show_demo_window {true};
};

class App {
public:
    std::unique_ptr<Engine> engine;
    std::unique_ptr<UIData> ui_data;

    static window::Hints config() {
        const auto main_scale = window::get_primary_monitor_scale();

        const int     width = static_cast<int>(main_scale * 800);
        const int     height = static_cast<int>(main_scale * 600);
        window::Hints hints{"Ultimate Flox", width, height};
        hints.resizable = true;
        return std::move(hints);
    }

    App() : engine{std::make_unique<Engine>(config())} {
        ui_data = std::make_unique<UIData>();
    }

    void update() const {
        engine->window->update();
        engine->window->poll_events();

        const auto cg {engine->im_gui->activate()};
        engine->im_gui->new_frame();

        if (ui_data->show_demo_window) ImGui::ShowDemoWindow(&ui_data->show_demo_window);

        // Rendering
        engine->im_gui->construct();
        const auto resolution = engine->window->resolution();
        glViewport(0, 0, resolution.x, resolution.y);
        glClear(GL_COLOR_BUFFER_BIT);

        engine->im_gui->render();
        engine->window->swap_buffers();
    }
};

int run() {
    const App app {};
    while (!app.engine->window->should_close()) {
        app.update();
    }
    return 0;
}

int handle_run() {
    try {
        return run();
    } catch (const std::bad_alloc &) {
        std::cerr << "Unable to allocate memory for program. Exiting." << std::endl;
        return 1;
    } catch (const std::exception &e) {
#ifdef WIN32
        std::cerr << e.what() << std::endl;
#endif
        return 1;
    }
}

#ifdef WIN32
    #include <windows.h>
int WINAPI wWinMain(
    HINSTANCE   hInstance,     // HANDLE TO AN INSTANCE.
    HINSTANCE   hPrevInstance, // USELESS on modern windows (totally ignore hPrevInstance).
    const PWSTR lpCmdLine,     // Command line arguments.
    const int   nShowCmd       // Start window maximized, minimized, etc.
) {
    (void) hInstance;
    (void) hPrevInstance;
    (void) lpCmdLine;
    (void) nShowCmd;
    return handle_run();
}

int wmain() { return handle_run(); }
#else  // WIN32
int main() { return handle_run(); }
#endif // WIN32
