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

struct UIData {
    bool show_demo_window {false};
};

class App {
public:
    entt::dispatcher &dispatcher;
    core::InputDispatcher input_dispatcher {};
    std::unique_ptr<window::Window> window;
    std::unique_ptr<KeyHandler> key_handler;
    std::unique_ptr<UIData> ui_data;

    App() : dispatcher{entt::locator<entt::dispatcher>::emplace()} {
        const auto main_scale = window::get_primary_monitor_scale();

        const int     width = static_cast<int>(main_scale * 800);
        const int     height = static_cast<int>(main_scale * 600);
        window::Hints hints{"Ultimate Flox", width, height};
        hints.resizable = true;
        window = std::make_unique<window::Window>(&input_dispatcher, hints);

        key_handler = std::make_unique<KeyHandler>();
        input_dispatcher.push_layer(key_handler.get());

        // Set up Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
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
        ImGuiStyle &style      = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);
        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;
        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

        ImGui_ImplGlfw_InitForOpenGL(window->glfw_window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        ui_data = std::make_unique<UIData>();
    }

    void update() const {
        window->update();
        window->poll_events();
        // Start the Dear ImGui frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ui_data->show_demo_window) ImGui::ShowDemoWindow(&ui_data->show_demo_window);

        // Rendering
        ImGui::Render();
        const auto resolution = window->resolution();
        glViewport(0, 0, resolution.x, resolution.y);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window->swap_buffers();
    }

    ~App() {
        input_dispatcher.clear();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};

int run() {
    const App app {};
    while (!app.window->should_close()) {
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
