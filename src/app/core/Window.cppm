module;
#include "pch.hpp"
export module Window;

import Input;
import Event;
import InputDispatcher;

namespace window {
    export class Window;
    int from_key(input::Key);
    int from_mouse_button(input::MouseButton);

    input::Key         to_key(int);
    input::MouseButton to_mouse_button(int);

    export class context_creation_failure final : public std::exception {};
    export class initialization_failure   final : public std::exception {};
    export class creation_failure         final : public std::exception {};

    class GLFW {
        GLFW() {
            if (glfwInit() == GLFW_FALSE) {
                throw initialization_failure();
            }
        }

    public:
        static GLFW &get() {
            static GLFW inst;
            return inst;
        }

        ~GLFW() { glfwTerminate(); }
    };

    export struct Hints {
        const char  *title{"Ultimate Flox"};
        const int    width{800};
        const int    height{640};
        GLFWmonitor *monitor{nullptr};
        GLFWwindow  *share{nullptr};

        // Watch for bit-fields to be fixed in MSVC
        // https://developercommunity.visualstudio.com/t/Default-member-initializers-for-bit-fiel/10030064?q=export+bit+field
        // bool stereo : 1 {GLFW_FALSE};
        // bool double_buffered : 1 {GLFW_TRUE};

        bool stereo{GLFW_FALSE};
        bool double_buffered{GLFW_TRUE};
        bool resizable{GLFW_FALSE};
        bool visible{GLFW_TRUE};
        bool decorated{GLFW_TRUE};
        bool take_focus{GLFW_TRUE};
        bool iconify{GLFW_TRUE};
        bool floating{GLFW_FALSE};

        bool maximized{GLFW_FALSE};
        bool center_cursor{GLFW_TRUE};
        bool transparent{GLFW_FALSE};
        bool focus_on_show{GLFW_TRUE};
        bool scale_to_monitor{GLFW_TRUE};
        bool srgb_capable{GLFW_FALSE};
        // bool  : 2;

        bool opengl_forward_compatibility{GLFW_TRUE};
        bool opengl_generate_errors{GLFW_FALSE};
        bool opengl_debug_context{GLFW_TRUE};
        // bool  : 5;

        int client_api{GLFW_OPENGL_API};
        int context_creation_api{GLFW_NATIVE_CONTEXT_API};
        int opengl_profile{GLFW_OPENGL_CORE_PROFILE};
        int opengl_version_major{4};
        int opengl_version_minor{6};

        int multisample_samples{0};

        int refresh_rate{120};
        int swap_interval{0};

        Hints() = default;

        Hints(const char *title, const int width, const int height) : title(title), width(width), height(height) {}
    };

    class Window {

    public:
        GLFWwindow *glfw_window;

    private:
        core::InputDispatcher* m_dispatcher;
        glm::dvec2        m_cursor{0.0, 0.0};

    public:
        [[nodiscard]] bool should_close() const noexcept { return glfwWindowShouldClose(glfw_window) == GLFW_TRUE; }
        [[nodiscard]] glm::ivec2 resolution() const noexcept {
            int width, height;
            glfwGetFramebufferSize(glfw_window, &width, &height);
            return {width, height};
        }

        void swap_buffers() const noexcept { glfwSwapBuffers(glfw_window); }

        void update() noexcept {
            double x_pos, y_pos;
            glfwGetCursorPos(glfw_window, &x_pos, &y_pos);
            m_cursor = glm::dvec2{x_pos, y_pos};
        }

        static void poll_events() noexcept { glfwPollEvents(); }

    private:
        static GLFWwindow *create(const Hints &hints) {
            GLFW::get();

            // Set GLFW window hints.
            // ****** Hard Constraints ******
            // * Must match available capabilities exactly.

            // Specifies whether to use OpenGL stereoscopic rendering.
            glfwWindowHint(GLFW_STEREO, hints.stereo);

            // Specifies whether the framebuffer should be double-buffered. You nearly always want to use double
            // buffering.
            glfwWindowHint(GLFW_DOUBLEBUFFER, hints.double_buffered);

            // Specifies which context creation API to use to create the context.
            glfwWindowHint(GLFW_CLIENT_API, hints.client_api);
            // glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            // An extension loader library that assumes it knows which API was used to create the current context may
            // fail if you change this hint.
            //   This can be resolved by having it load functions via glfwGetProcAddress.
            // Maybe have to change for linux.
            glfwWindowHint(GLFW_CONTEXT_CREATION_API, hints.context_creation_api);
            // glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_OSMESA_CONTEXT_API);
            // glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

#ifndef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, hints.opengl_forward_compatibility);
#else
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#endif
            // Disable deprecated OpenGL operations.

            // Specifies which OpenGL profile to create the context for.
            glfwWindowHint(GLFW_OPENGL_PROFILE, hints.opengl_profile);
            // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
            // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

            // ****** Soft Constraints ******
            // **** OpenGL ****
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, hints.opengl_version_major);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, hints.opengl_version_minor);

            // Specifies the robustness strategy to be used by the context.
            // glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_RESET_NOTIFICATION);
            // glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
            // glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_ROBUSTNESS);

            // Specifies the release behavior to be used by the context.
            // https://registry.khronos.org/OpenGL/extensions/KHR/KHR_context_flush_control.txt
            // glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_ANY_RELEASE_BEHAVIOR);
            // glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
            // glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_NONE);

            glfwWindowHint(GLFW_CONTEXT_NO_ERROR, hints.opengl_generate_errors);
#ifndef NDEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, hints.opengl_debug_context);
#endif // NDEBUG

            // **** GLFW ****
            glfwWindowHint(GLFW_RESIZABLE, hints.resizable);
            glfwWindowHint(GLFW_VISIBLE, hints.visible);
            glfwWindowHint(GLFW_DECORATED, hints.decorated);
            glfwWindowHint(GLFW_FOCUSED, hints.take_focus);
            glfwWindowHint(GLFW_AUTO_ICONIFY, hints.iconify);
            glfwWindowHint(GLFW_FLOATING, hints.floating);
            glfwWindowHint(GLFW_MAXIMIZED, hints.maximized);
            glfwWindowHint(GLFW_CENTER_CURSOR, hints.center_cursor);
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, hints.transparent);
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, hints.focus_on_show);
            glfwWindowHint(GLFW_SCALE_TO_MONITOR, hints.scale_to_monitor);
            glfwWindowHint(GLFW_SAMPLES, hints.multisample_samples);
            glfwWindowHint(GLFW_SRGB_CAPABLE, hints.srgb_capable);
            glfwWindowHint(GLFW_REFRESH_RATE, hints.refresh_rate);

            // TODO: Select primary monitor.

            GLFWwindow *p_window = glfwCreateWindow(hints.width, hints.height, hints.title, hints.monitor, hints.share);
            glfwMakeContextCurrent(p_window);
            if (p_window == nullptr) {
                glfwSetWindowUserPointer(p_window, nullptr);
                glfwDestroyWindow(p_window);
                glfwPollEvents();
                throw creation_failure();
            }

            if (!gladLoadGL(glfwGetProcAddress)) {
                glfwSetWindowUserPointer(p_window, nullptr);
                glfwDestroyWindow(p_window);
                glfwPollEvents();
                throw context_creation_failure();
            }

            if (hints.multisample_samples > 0) {
                glEnable(GL_MULTISAMPLE);
            }

            glfwSwapInterval(hints.swap_interval);

            // Output the current GLFW version.
            std::cout << "GLFW " << glfwGetVersionString() << std::endl;

            // Output the current OpenGL version.
            std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

            glfwSetMouseButtonCallback(p_window, make_callback<&Window::on_mouse>());
            glfwSetCursorPosCallback(p_window, make_callback<&Window::on_cursor>());
            glfwSetScrollCallback(p_window, make_callback<&Window::on_scroll>());
            glfwSetKeyCallback(p_window, make_callback<&Window::on_key>());
            glfwSetCharCallback(p_window, make_callback<&Window::on_char>());
            glfwSetCharModsCallback(p_window, make_callback<&Window::on_modified_char>());

            return p_window;
        }

    public:
        explicit Window(core::InputDispatcher *dispatcher, const Hints &hints) noexcept
            : glfw_window(create(hints)), m_dispatcher(dispatcher) {
            glfwSetWindowUserPointer(glfw_window, this);
        }

        ~Window() noexcept {
            if (glfw_window != nullptr) {
                glfwSetMouseButtonCallback(glfw_window, nullptr);
                glfwSetCursorPosCallback(glfw_window, nullptr);
                glfwSetScrollCallback(glfw_window, nullptr);
                glfwSetKeyCallback(glfw_window, nullptr);
                glfwSetCharCallback(glfw_window, nullptr);
                glfwSetCharModsCallback(glfw_window, nullptr);
                glfwSetWindowUserPointer(glfw_window, nullptr);
                glfwDestroyWindow(glfw_window);
                glfw_window = nullptr;
            }
        }

    private:
        void on_mouse(int button, int action, int mods) const;
        void on_cursor(double x_pos, double y_pos);
        void on_scroll(double x_offset, double y_offset) const;
        void on_key(int key, int scancode, int action, int mods) const;
        void on_char(unsigned int codepoint) const;
        void on_modified_char(unsigned int codepoint, int mods) const;

        template <auto Candidate>
        static auto make_callback() {
            return []<typename... Args>(GLFWwindow *p_window, Args... args) {
                static_assert(
                    std::is_invocable_v<decltype(Candidate), Window *, Args...>,
                    "Member function has invalid signature.");
                if (const auto window = static_cast<Window *>(glfwGetWindowUserPointer(p_window)); window != nullptr) {
                    std::invoke(Candidate, window, args...);
                } else {
                    std::cerr << "Got null Window in callback." << '\n';
                }
            };
        }
    };

    void Window::on_mouse(const int button, const int action, const int mods) const {
        if (action != GLFW_PRESS and action != GLFW_RELEASE) {
            return;
        }
        const input::MouseButtonEvent event{
            to_mouse_button(button),
            action == GLFW_PRESS ? input::Activation::Press : input::Activation::Release,
            mods,
            m_cursor};
        m_dispatcher->trigger(event);
    }

    void Window::on_cursor(const double x_pos, const double y_pos) {
        m_cursor = glm::dvec2{x_pos, y_pos};
        const input::CursorEvent event{m_cursor};
        m_dispatcher->trigger(event);
    }

    void Window::on_scroll(const double x_offset, const double y_offset) const {
        const input::ScrollEvent event{glm::dvec2{x_offset, y_offset}, m_cursor};
        m_dispatcher->trigger(event);
    }

    void Window::on_key(const int key, const int scancode, const int action, const int mods) const {
        if (action != GLFW_PRESS and action != GLFW_RELEASE) {
            return;
        }
        const input::KeyEvent event{
            to_key(key),
            scancode,
            action == GLFW_PRESS ? input::Activation::Press : input::Activation::Release,
            mods,
            m_cursor};
        m_dispatcher->trigger(event);
    }

    void Window::on_char(const unsigned int codepoint) const {
        const input::TextEvent event{
            codepoint,
        };
        m_dispatcher->trigger(event);
    }

    void Window::on_modified_char(const unsigned int codepoint, const int mods) const {
        const input::ModifiedTextEvent event{codepoint, mods};
        m_dispatcher->trigger(event);
    }

    export void poll_events() { glfwPollEvents(); }

    export float get_primary_monitor_scale() {
        GLFW::get();
        float x_scale;
        glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &x_scale, nullptr);
        return x_scale;
    }

    int from_key(const input::Key key) {
        using enum input::Key;
        switch (key) {
            case None:
            default: return 0;
            case Space: return GLFW_KEY_SPACE;
            case Apostrophe: return GLFW_KEY_APOSTROPHE;
            case Comma: return GLFW_KEY_COMMA;
            case Minus: return GLFW_KEY_MINUS;
            case Period: return GLFW_KEY_PERIOD;
            case Slash: return GLFW_KEY_SLASH;
            case Zero: return GLFW_KEY_0;
            case One: return GLFW_KEY_1;
            case Two: return GLFW_KEY_2;
            case Three: return GLFW_KEY_3;
            case Four: return GLFW_KEY_4;
            case Five: return GLFW_KEY_5;
            case Six: return GLFW_KEY_6;
            case Seven: return GLFW_KEY_7;
            case Eight: return GLFW_KEY_8;
            case Nine: return GLFW_KEY_9;
            case Semicolon: return GLFW_KEY_SEMICOLON;
            case Equal: return GLFW_KEY_EQUAL;
            case A: return GLFW_KEY_A;
            case B: return GLFW_KEY_B;
            case C: return GLFW_KEY_C;
            case D: return GLFW_KEY_D;
            case E: return GLFW_KEY_E;
            case F: return GLFW_KEY_F;
            case G: return GLFW_KEY_G;
            case H: return GLFW_KEY_H;
            case I: return GLFW_KEY_I;
            case J: return GLFW_KEY_J;
            case K: return GLFW_KEY_K;
            case L: return GLFW_KEY_L;
            case M: return GLFW_KEY_M;
            case N: return GLFW_KEY_N;
            case O: return GLFW_KEY_O;
            case P: return GLFW_KEY_P;
            case Q: return GLFW_KEY_Q;
            case R: return GLFW_KEY_R;
            case S: return GLFW_KEY_S;
            case T: return GLFW_KEY_T;
            case U: return GLFW_KEY_U;
            case V: return GLFW_KEY_V;
            case W: return GLFW_KEY_W;
            case X: return GLFW_KEY_X;
            case Y: return GLFW_KEY_Y;
            case Z: return GLFW_KEY_Z;
            case LeftBracket: return GLFW_KEY_LEFT_BRACKET;
            case Backslash: return GLFW_KEY_BACKSLASH;
            case RightBracket: return GLFW_KEY_RIGHT_BRACKET;
            case Grave: return GLFW_KEY_GRAVE_ACCENT;
            case World1: return GLFW_KEY_WORLD_1;
            case World2: return GLFW_KEY_WORLD_2;
            case Escape: return GLFW_KEY_ESCAPE;
            case Enter: return GLFW_KEY_ENTER;
            case Tab: return GLFW_KEY_TAB;
            case Backspace: return GLFW_KEY_BACKSPACE;
            case Insert: return GLFW_KEY_INSERT;
            case Delete: return GLFW_KEY_DELETE;
            case Right: return GLFW_KEY_RIGHT;
            case Left: return GLFW_KEY_LEFT;
            case Down: return GLFW_KEY_DOWN;
            case Up: return GLFW_KEY_UP;
            case PageUp: return GLFW_KEY_PAGE_UP;
            case PageDown: return GLFW_KEY_PAGE_DOWN;
            case Home: return GLFW_KEY_HOME;
            case End: return GLFW_KEY_END;
            case CapsLock: return GLFW_KEY_CAPS_LOCK;
            case ScrollLock: return GLFW_KEY_SCROLL_LOCK;
            case NumLock: return GLFW_KEY_NUM_LOCK;
            case PrintScreen: return GLFW_KEY_PRINT_SCREEN;
            case Pause: return GLFW_KEY_PAUSE;
            case F1: return GLFW_KEY_F1;
            case F2: return GLFW_KEY_F2;
            case F3: return GLFW_KEY_F3;
            case F4: return GLFW_KEY_F4;
            case F5: return GLFW_KEY_F5;
            case F6: return GLFW_KEY_F6;
            case F7: return GLFW_KEY_F7;
            case F8: return GLFW_KEY_F8;
            case F9: return GLFW_KEY_F9;
            case F10: return GLFW_KEY_F10;
            case F11: return GLFW_KEY_F11;
            case F12: return GLFW_KEY_F12;
            case F13: return GLFW_KEY_F13;
            case F14: return GLFW_KEY_F14;
            case F15: return GLFW_KEY_F15;
            case F16: return GLFW_KEY_F16;
            case F17: return GLFW_KEY_F17;
            case F18: return GLFW_KEY_F18;
            case F19: return GLFW_KEY_F19;
            case F20: return GLFW_KEY_F20;
            case F21: return GLFW_KEY_F21;
            case F22: return GLFW_KEY_F22;
            case F23: return GLFW_KEY_F23;
            case F24: return GLFW_KEY_F24;
            case F25: return GLFW_KEY_F25;
            case KP0: return GLFW_KEY_KP_0;
            case KP1: return GLFW_KEY_KP_1;
            case KP2: return GLFW_KEY_KP_2;
            case KP3: return GLFW_KEY_KP_3;
            case KP4: return GLFW_KEY_KP_4;
            case KP5: return GLFW_KEY_KP_5;
            case KP6: return GLFW_KEY_KP_6;
            case KP7: return GLFW_KEY_KP_7;
            case KP8: return GLFW_KEY_KP_8;
            case KP9: return GLFW_KEY_KP_9;
            case KPDecimal: return GLFW_KEY_KP_DECIMAL;
            case KPDivide: return GLFW_KEY_KP_DIVIDE;
            case KPMultiply: return GLFW_KEY_KP_MULTIPLY;
            case KPSubtract: return GLFW_KEY_KP_SUBTRACT;
            case KPAdd: return GLFW_KEY_KP_ADD;
            case KPEnter: return GLFW_KEY_KP_ENTER;
            case KPEqual: return GLFW_KEY_KP_EQUAL;
            case LeftShift: return GLFW_KEY_LEFT_SHIFT;
            case LeftControl: return GLFW_KEY_LEFT_CONTROL;
            case LeftAlt: return GLFW_KEY_LEFT_ALT;
            case LeftSuper: return GLFW_KEY_LEFT_SUPER;
            case RightShift: return GLFW_KEY_RIGHT_SHIFT;
            case RightControl: return GLFW_KEY_RIGHT_CONTROL;
            case RightAlt: return GLFW_KEY_RIGHT_ALT;
            case RightSuper: return GLFW_KEY_RIGHT_SUPER;
            case Menu: return GLFW_KEY_MENU;
        }
    }

    input::Key to_key(const int keycode) {
        using enum input::Key;
        switch (keycode) {
            case GLFW_KEY_SPACE: return Space;
            case GLFW_KEY_APOSTROPHE: return Apostrophe;
            case GLFW_KEY_COMMA: return Comma;
            case GLFW_KEY_MINUS: return Minus;
            case GLFW_KEY_PERIOD: return Period;
            case GLFW_KEY_SLASH: return Slash;
            case GLFW_KEY_0: return Zero;
            case GLFW_KEY_1: return One;
            case GLFW_KEY_2: return Two;
            case GLFW_KEY_3: return Three;
            case GLFW_KEY_4: return Four;
            case GLFW_KEY_5: return Five;
            case GLFW_KEY_6: return Six;
            case GLFW_KEY_7: return Seven;
            case GLFW_KEY_8: return Eight;
            case GLFW_KEY_9: return Nine;
            case GLFW_KEY_SEMICOLON: return Semicolon;
            case GLFW_KEY_EQUAL: return Equal;
            case GLFW_KEY_A: return A;
            case GLFW_KEY_B: return B;
            case GLFW_KEY_C: return C;
            case GLFW_KEY_D: return D;
            case GLFW_KEY_E: return E;
            case GLFW_KEY_F: return F;
            case GLFW_KEY_G: return G;
            case GLFW_KEY_H: return H;
            case GLFW_KEY_I: return I;
            case GLFW_KEY_J: return J;
            case GLFW_KEY_K: return K;
            case GLFW_KEY_L: return L;
            case GLFW_KEY_M: return M;
            case GLFW_KEY_N: return N;
            case GLFW_KEY_O: return O;
            case GLFW_KEY_P: return P;
            case GLFW_KEY_Q: return Q;
            case GLFW_KEY_R: return R;
            case GLFW_KEY_S: return S;
            case GLFW_KEY_T: return T;
            case GLFW_KEY_U: return U;
            case GLFW_KEY_V: return V;
            case GLFW_KEY_W: return W;
            case GLFW_KEY_X: return X;
            case GLFW_KEY_Y: return Y;
            case GLFW_KEY_Z: return Z;
            case GLFW_KEY_LEFT_BRACKET: return LeftBracket;
            case GLFW_KEY_BACKSLASH: return Backslash;
            case GLFW_KEY_RIGHT_BRACKET: return RightBracket;
            case GLFW_KEY_GRAVE_ACCENT: return Grave;
            case GLFW_KEY_WORLD_1: return World1;
            case GLFW_KEY_WORLD_2: return World2;

            case GLFW_KEY_ESCAPE: return Escape;
            case GLFW_KEY_ENTER: return Enter;
            case GLFW_KEY_TAB: return Tab;
            case GLFW_KEY_BACKSPACE: return Backspace;
            case GLFW_KEY_INSERT: return Insert;
            case GLFW_KEY_DELETE: return Delete;
            case GLFW_KEY_RIGHT: return Right;
            case GLFW_KEY_LEFT: return Left;
            case GLFW_KEY_DOWN: return Down;
            case GLFW_KEY_UP: return Up;
            case GLFW_KEY_PAGE_UP: return PageUp;
            case GLFW_KEY_PAGE_DOWN: return PageDown;
            case GLFW_KEY_HOME: return Home;
            case GLFW_KEY_END: return End;
            case GLFW_KEY_CAPS_LOCK: return CapsLock;
            case GLFW_KEY_SCROLL_LOCK: return ScrollLock;
            case GLFW_KEY_NUM_LOCK: return NumLock;
            case GLFW_KEY_PRINT_SCREEN: return PrintScreen;
            case GLFW_KEY_PAUSE: return Pause;
            case GLFW_KEY_F1: return F1;
            case GLFW_KEY_F2: return F2;
            case GLFW_KEY_F3: return F3;
            case GLFW_KEY_F4: return F4;
            case GLFW_KEY_F5: return F5;
            case GLFW_KEY_F6: return F6;
            case GLFW_KEY_F7: return F7;
            case GLFW_KEY_F8: return F8;
            case GLFW_KEY_F9: return F9;
            case GLFW_KEY_F10: return F10;
            case GLFW_KEY_F11: return F11;
            case GLFW_KEY_F12: return F12;
            case GLFW_KEY_F13: return F13;
            case GLFW_KEY_F14: return F14;
            case GLFW_KEY_F15: return F15;
            case GLFW_KEY_F16: return F16;
            case GLFW_KEY_F17: return F17;
            case GLFW_KEY_F18: return F18;
            case GLFW_KEY_F19: return F19;
            case GLFW_KEY_F20: return F20;
            case GLFW_KEY_F21: return F21;
            case GLFW_KEY_F22: return F22;
            case GLFW_KEY_F23: return F23;
            case GLFW_KEY_F24: return F24;
            case GLFW_KEY_F25: return F25;
            case GLFW_KEY_KP_0: return KP0;
            case GLFW_KEY_KP_1: return KP1;
            case GLFW_KEY_KP_2: return KP2;
            case GLFW_KEY_KP_3: return KP3;
            case GLFW_KEY_KP_4: return KP4;
            case GLFW_KEY_KP_5: return KP5;
            case GLFW_KEY_KP_6: return KP6;
            case GLFW_KEY_KP_7: return KP7;
            case GLFW_KEY_KP_8: return KP8;
            case GLFW_KEY_KP_9: return KP9;
            case GLFW_KEY_KP_DECIMAL: return KPDecimal;
            case GLFW_KEY_KP_DIVIDE: return KPDivide;
            case GLFW_KEY_KP_MULTIPLY: return KPMultiply;
            case GLFW_KEY_KP_SUBTRACT: return KPSubtract;
            case GLFW_KEY_KP_ADD: return KPAdd;
            case GLFW_KEY_KP_ENTER: return KPEnter;
            case GLFW_KEY_KP_EQUAL: return KPEqual;
            case GLFW_KEY_LEFT_SHIFT: return LeftShift;
            case GLFW_KEY_LEFT_CONTROL: return LeftControl;
            case GLFW_KEY_LEFT_ALT: return LeftAlt;
            case GLFW_KEY_LEFT_SUPER: return LeftSuper;
            case GLFW_KEY_RIGHT_SHIFT: return RightShift;
            case GLFW_KEY_RIGHT_CONTROL: return RightControl;
            case GLFW_KEY_RIGHT_ALT: return RightAlt;
            case GLFW_KEY_RIGHT_SUPER: return RightSuper;
            case GLFW_KEY_MENU: return Menu;
            default: return None;
        }
    }

    int from_mouse_button(const input::MouseButton button) {
        using enum input::MouseButton;
        switch (button) {
            case None:
            default: return 0;
            case Mouse1: return GLFW_MOUSE_BUTTON_1;
            case Mouse2: return GLFW_MOUSE_BUTTON_2;
            case Mouse3: return GLFW_MOUSE_BUTTON_3;
            case Mouse4: return GLFW_MOUSE_BUTTON_4;
            case Mouse5: return GLFW_MOUSE_BUTTON_5;
            case Mouse6: return GLFW_MOUSE_BUTTON_6;
            case Mouse7: return GLFW_MOUSE_BUTTON_7;
            case Mouse8: return GLFW_MOUSE_BUTTON_8;
        }
    }

    input::MouseButton to_mouse_button(const int button) {
        using enum input::MouseButton;
        switch (button) {
            case GLFW_MOUSE_BUTTON_1: return Mouse1;
            case GLFW_MOUSE_BUTTON_2: return Mouse2;
            case GLFW_MOUSE_BUTTON_3: return Mouse3;
            case GLFW_MOUSE_BUTTON_4: return Mouse4;
            case GLFW_MOUSE_BUTTON_5: return Mouse5;
            case GLFW_MOUSE_BUTTON_6: return Mouse6;
            case GLFW_MOUSE_BUTTON_7: return Mouse7;
            case GLFW_MOUSE_BUTTON_8: return Mouse8;
            default: return None;
        }
    }
} // namespace window
