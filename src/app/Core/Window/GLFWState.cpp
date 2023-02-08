#include "pch.hpp"
#include "Window.hpp"


void window::details::GLFWState::destroy() {
    if (m_state != nullptr) {
        glfwSetWindowUserPointer(m_state, nullptr);
        glfwDestroyWindow(m_state);
        glfwPollEvents();
    }
}

window::details::GLFWState::GLFWState(const char *title, const window::Hints &hints) {
    constexpr int glfw_enable[2] {GLFW_FALSE, GLFW_TRUE};

    // Set GLFW window hints.
    // ****** Hard Constraints ******
    // Must match available capabilities exactly.
    glfwWindowHint(GLFW_STEREO, GLFW_FALSE);  // Specifies whether to use OpenGL stereoscopic rendering.

    // Specifies whether the framebuffer should be double buffered. You nearly always want to use double buffering.
    // Investigate to see why this doesn't work when false.
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    // Specifies which context creation API to use to create the context.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // An extension loader library that assumes it knows which API was used to create the current context ...
    //  may fail if you change this hint. This can be resolved by having it load functions via glfwGetProcAddress.
    // Maybe have to change for linux.
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    //glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);  // Disable deprecated OpenGL operations.

    // Specifies which OpenGL profile to create the context for.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    // ****** Soft Constraints ******
    // **** OpenGL ****
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // Specifies the robustness strategy to be used by the context.
    //glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_RESET_NOTIFICATION);
    //glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
    //glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_ROBUSTNESS);

    // Specifies the release behavior to be used by the context.
    // https://registry.khronos.org/OpenGL/extensions/KHR/KHR_context_flush_control.txt
    glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_ANY_RELEASE_BEHAVIOR);
    //glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
    //glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_NONE);

    glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_FALSE);
#ifdef FLOX_SHOW_DEBUG_INFO
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif // FLOX_SHOW_DEBUG_INFO

    // **** GLFW ****
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, glfw_enable[hints.flags.decorated()]);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
    glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, glfw_enable[hints.flags.transparent()]);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
    glfwWindowHint(GLFW_SAMPLES, hints.samples());
    glfwWindowHint(GLFW_SRGB_CAPABLE, true);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    /* Create a GLFW window and its OpenGL context. */
    m_state = glfwCreateWindow(hints.width(), hints.height(), title, nullptr, nullptr);
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

    if (hints.samples() > 0) {
        glEnable(GL_MULTISAMPLE);
    }

    // Output the current GLFW version.
    std::cout << "GLFW " << glfwGetVersionString() << std::endl;

    // Output the current OpenGL version.
    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

    glfwSwapInterval(hints.flags.vsync());
}

void window::details::GLFWState::set_user_pointer(void *user_ptr) {
    glfwSetWindowUserPointer(m_state, user_ptr);
}

void window::details::GLFWState::set_key_callback(GLFWkeyfun callback) {
    glfwSetKeyCallback(m_state, callback);
}

void window::details::GLFWState::set_cursor_callback(GLFWcursorposfun callback) {
    glfwSetCursorPosCallback(m_state, callback);
}

void window::details::GLFWState::set_mouse_callback(GLFWmousebuttonfun callback) {
    glfwSetMouseButtonCallback(m_state, callback);
}

void window::details::GLFWState::set_text_callback(GLFWcharfun callback) {
    glfwSetCharCallback(m_state, callback);
}

bool window::details::GLFWState::should_close() {
    return glfwWindowShouldClose(m_state);
}

void window::details::GLFWState::should_close(bool value) {
    glfwSetWindowShouldClose(m_state, value);
}

void window::details::GLFWState::swap_buffers() {
    glfwSwapBuffers(m_state);
}

void window::details::GLFWState::clear() {
    // Requires OpenGL context.
    glClear(GL_COLOR_BUFFER_BIT);
}

bool window::details::GLFWState::created() {
    return true;
}
