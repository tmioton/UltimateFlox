#include "pch.hpp"
#include "Window.hpp"


static void called_empty_state(const char* method_name) {
    // Inform user here. Probably log.
    std::cout << "Attempted " << method_name << " operation on uninitialized window.\n";
}

void window::details::EmptyGLFWState::set_user_pointer(void *) {
    called_empty_state("set_user_pointer(void*)");
}

void window::details::EmptyGLFWState::set_key_callback(GLFWkeyfun) {
    called_empty_state("set_key_callback(GLFWkeyfun)");
}

void window::details::EmptyGLFWState::set_cursor_callback(GLFWcursorposfun) {
    called_empty_state("set_cursor_callback(GLFWcursorposfun");
}

void window::details::EmptyGLFWState::set_mouse_callback(GLFWmousebuttonfun) {
    called_empty_state("set_mouse_callback(GLFWmousebuttonfun");
}

void window::details::EmptyGLFWState::set_text_callback(GLFWcharfun) {
    called_empty_state("set_text_callback(GLFWcharfun)");
}

bool window::details::EmptyGLFWState::should_close() {
    called_empty_state("should_close()");
    return true;
}

void window::details::EmptyGLFWState::should_close(bool) {
    called_empty_state("should_close(bool)");
}

void window::details::EmptyGLFWState::swap_buffers() {
    called_empty_state("swap_buffers()");
}

void window::details::EmptyGLFWState::clear() {
    called_empty_state("clear()");
}

bool window::details::EmptyGLFWState::created() {
    return false;
}


void window::details::CreatedGLFWState::destroy() {
    if (m_state != nullptr) {
        glfwSetWindowUserPointer(m_state, nullptr);
        glfwDestroyWindow(m_state);
        glfwPollEvents();
    }

    glfwTerminate();
}

window::details::CreatedGLFWState::CreatedGLFWState(const char* title, const window::Config &config) {
    // Set GLFW window hints.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // !NDEBUG

    glfwWindowHint(GLFW_SAMPLES, config.samples);

    glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);
    //glfwWindowHint(GLFW_SRGB_CAPABLE, true);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, true);

    /* Create a GLFW window and its OpenGL context. */
    m_state = glfwCreateWindow(config.width, config.height, title, nullptr, nullptr);
    glfwMakeContextCurrent(m_state);
    if (!m_state) {
        destroy();
// FIXME: Hack just to compile on linux
#ifdef _WIN32
        throw std::exception("Failed to create GLFW window.");
#else
        std::cerr << "Failed to create GLFW window.\n";
        throw std::exception();
#endif
    }

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        destroy();
// FIXME: Hack just to compile on linux
#ifdef _WIN32
        throw std::exception("Failed to initialize Glad.");
#else
        std::cerr << "Failed to initialize Glad.\n";
        throw std::exception();
#endif
    }

    if (config.samples > 1) {
        glEnable(GL_MULTISAMPLE);
    }

    /* Output the current OpenGL version. */
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
}

void window::details::CreatedGLFWState::set_user_pointer(void *user_ptr) {
    glfwSetWindowUserPointer(m_state, user_ptr);
}

void window::details::CreatedGLFWState::set_key_callback(GLFWkeyfun callback) {
    glfwSetKeyCallback(m_state, callback);
}

void window::details::CreatedGLFWState::set_cursor_callback(GLFWcursorposfun callback) {
    glfwSetCursorPosCallback(m_state, callback);
}

void window::details::CreatedGLFWState::set_mouse_callback(GLFWmousebuttonfun callback) {
    glfwSetMouseButtonCallback(m_state, callback);
}

void window::details::CreatedGLFWState::set_text_callback(GLFWcharfun callback) {
    glfwSetCharCallback(m_state, callback);
}

bool window::details::CreatedGLFWState::should_close() {
    return glfwWindowShouldClose(m_state);
}

void window::details::CreatedGLFWState::should_close(bool value) {
    glfwSetWindowShouldClose(m_state, value);
}

void window::details::CreatedGLFWState::swap_buffers() {
    glfwSwapBuffers(m_state);
}

void window::details::CreatedGLFWState::clear() {
    // Requires OpenGL context.
    glClear(GL_COLOR_BUFFER_BIT);
}

bool window::details::CreatedGLFWState::created() {
    return true;
}


window::Window &window::Window::get() {
    static Window instance;
    return instance;
}

void window::Window::glfw_init() {
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

window::Window *window::Window::get_state(GLFWwindow* state) {
    return static_cast<Window*>(glfwGetWindowUserPointer(state));
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

void window::Window::create(const char *title, const window::Config &config) {
    m_state = std::make_unique<details::CreatedGLFWState>(title, config);
    m_state->set_user_pointer(this);
    m_state->set_key_callback([](GLFWwindow *window, int key, int scancode, int action, int mods) {
        Window * state = Window::get_state(window);
        switch (action) {
            case GLFW_PRESS:
                state->m_events.emplace_back(
                    Event::Type::KeyPress, KeyboardEvent{scancode, (int16_t)key, (int16_t)mods}
                );
                return;
            case GLFW_RELEASE:
                state->m_events.emplace_back(
                    Event::Type::KeyRelease, KeyboardEvent{scancode, (int16_t)key, (int16_t)mods}
                );
                return;
            case GLFW_REPEAT:
                state->m_events.emplace_back(
                    Event::Type::KeyRepeat, KeyboardEvent{scancode, (int16_t)key, (int16_t)mods}
                );
                return;
            default:return;
        }
    });

    m_state->set_cursor_callback([](GLFWwindow *window, double xpos, double ypos) {
        Window * state = Window::get_state(window);
        state->m_events.emplace_back(Event::Type::MouseMotion, MouseMotionEvent{xpos, ypos});
    });

    m_state->set_mouse_callback([](GLFWwindow *window, int button, int action, int mods) {
        Window * state = Window::get_state(window);
        switch (action) {
            case GLFW_PRESS:state->m_events.emplace_back(Event::Type::MouseDown, MouseButtonEvent{button, mods});
                return;
            case GLFW_RELEASE:state->m_events.emplace_back(Event::Type::MouseUp, MouseButtonEvent{button, mods});
            default:return;
        }
    });

    m_state->set_text_callback([](GLFWwindow *window, unsigned int codepoint) {
        Window * state = Window::get_state(window);
        state->m_events.emplace_back(Event::Type::TextInput, TextEvent{codepoint});
    });
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
