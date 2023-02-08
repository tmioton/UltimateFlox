#include "pch.hpp"
#include "Window.hpp"


// ****** Flags ******
window::Flags::Flags(bool v, bool d, bool t) {
    m_flags.set(0, v);
    m_flags.set(1, d);
    m_flags.set(2, t);
}

bool window::Flags::vsync() const { return m_flags.test(0); }
bool window::Flags::decorated() const { return m_flags.test(1); }
bool window::Flags::transparent() const { return m_flags.test(2); }


// ****** Hints ******
window::Hints::Hints(const int width, const int height, const int s, Flags f):
    m_dimensions(width, height), m_samples(static_cast<uint8_t>(s)), flags(f)
{}

int window::Hints::width() const { return m_dimensions.x; }
int window::Hints::height() const { return m_dimensions.y; }
int window::Hints::samples() const { return m_samples; }


// ****** Window ******
window::Window &window::Window::get() {
    static Window instance;
    return instance;
}

void window::Window::glfw_init() {
    glfwSetErrorCallback(
        [](int code, const char *message) {
            std::cerr << message << '\n';
        }
    );

    /* Initialize GLFW. */
    if (!glfwInit()) {
        // FIXME: Hack just to compile on linux
#ifdef _WIN32
        throw std::exception("Failed to initialize GLFW.");
#else
        std::cerr << "Failed to initialize GLFW.\n";
        throw std::exception();
#endif
    }
}

window::Window::Window() {
    glfw_init();
    m_events.reserve(DefaultEventStackCapacity);
}

window::Window::~Window() {
    glfwTerminate();
}

window::Window *window::Window::get_state(GLFWwindow *state) {
    return static_cast<Window *>(glfwGetWindowUserPointer(state));
}

bool window::Window::should_close() {
    return m_state->should_close();
}

void window::Window::should_close(bool value) {
    m_state->should_close(value);
}

void window::Window::swap_buffers() {
    m_state->swap_buffers();
}

void window::Window::clear() {
    m_state->clear();
}

void window::Window::update() {
    m_events.clear();
    glfwPollEvents();
}

void window::Window::create(const char *title, const window::Hints &hints) {
    m_state = std::make_unique<details::GLFWState>(title, hints);
    m_state->set_user_pointer(this);
    m_state->set_key_callback(
        [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            switch (action) {
                case GLFW_PRESS:
                    Window::get_state(window)->m_events.emplace_back(
                        Event::Type::KeyPress, KeyboardEvent {scancode, (int16_t) key, (int16_t) mods}
                    );
                    return;
                case GLFW_RELEASE:
                    Window::get_state(window)->m_events.emplace_back(
                        Event::Type::KeyRelease, KeyboardEvent {scancode, (int16_t) key, (int16_t) mods}
                    );
                    return;
                case GLFW_REPEAT:
                    Window::get_state(window)->m_events.emplace_back(
                        Event::Type::KeyRepeat, KeyboardEvent {scancode, (int16_t) key, (int16_t) mods}
                    );
                    return;
                default: unexpected();
            }
        }
    );

    m_state->set_cursor_callback(
        [](GLFWwindow *window, double xpos, double ypos) {
            Window::get_state(window)->m_events.emplace_back(Event::Type::MouseMotion, MouseMotionEvent {xpos, ypos});
        }
    );

    m_state->set_mouse_callback(
        [](GLFWwindow *window, int button, int action, int mods) {
            switch (action) {
                case GLFW_PRESS: {
                    Window::get_state(window)->m_events.emplace_back(
                        Event::Type::MouseDown, MouseButtonEvent {button, mods}
                    );
                    return;
                }
                case GLFW_RELEASE: {
                    Window::get_state(window)->m_events.emplace_back(
                        Event::Type::MouseUp, MouseButtonEvent {button, mods}
                    );
                    return;
                }
                default: unexpected();
            }
        }
    );

    m_state->set_text_callback(
        [](GLFWwindow *window, unsigned int codepoint) {
            Window::get_state(window)->m_events.emplace_back(Event::Type::TextInput, TextEvent {codepoint});
        }
    );
}

bool window::Window::created() {
    return m_state->created();
}

std::optional<window::Event> window::Window::poll_event() {
    if (m_events.empty()) {
        return std::nullopt;
    } else {
        Event event = m_events.back();
        m_events.pop_back();
        return event;
    }
}
