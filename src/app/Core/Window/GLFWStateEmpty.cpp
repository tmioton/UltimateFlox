#include "pch.hpp"
#include "Window.hpp"


static void called_empty_state(const char *method_name) {
    // Inform user here. Probably log.
    std::cout << "Attempted " << method_name << " operation on uninitialized window.\n";
}

void window::details::GLFWStateEmpty::set_user_pointer(void *) {
    called_empty_state("set_user_pointer(void*)");
}

void window::details::GLFWStateEmpty::set_key_callback(GLFWkeyfun) {
    called_empty_state("set_key_callback(GLFWkeyfun)");
}

void window::details::GLFWStateEmpty::set_cursor_callback(GLFWcursorposfun) {
    called_empty_state("set_cursor_callback(GLFWcursorposfun");
}

void window::details::GLFWStateEmpty::set_mouse_callback(GLFWmousebuttonfun) {
    called_empty_state("set_mouse_callback(GLFWmousebuttonfun");
}

void window::details::GLFWStateEmpty::set_text_callback(GLFWcharfun) {
    called_empty_state("set_text_callback(GLFWcharfun)");
}

void window::details::GLFWStateEmpty::set_scroll_callback(GLFWscrollfun) {
    called_empty_state("set_scroll_callback(GLFWscrollfun");
}

window::Dimensions window::details::GLFWStateEmpty::real_size() {
    called_empty_state("real_size()");
    return { 0, 0 };
}

bool window::details::GLFWStateEmpty::should_close() {
    called_empty_state("should_close()");
    return true;
}

void window::details::GLFWStateEmpty::should_close(bool) {
    called_empty_state("should_close(bool)");
}

void window::details::GLFWStateEmpty::swap_buffers() {
    called_empty_state("swap_buffers()");
}

void window::details::GLFWStateEmpty::clear() {
    called_empty_state("clear()");
}

bool window::details::GLFWStateEmpty::created() {
    return false;
}
