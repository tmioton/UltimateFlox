#pragma once

#include "pch.hpp"
#include "UIComponent.hpp"


namespace ui {
    class UserInterface {
        UserInterface();
    public:
        static UserInterface &get();
        UserInterface(UserInterface const &) = delete;
        UserInterface(UserInterface &&) = delete;
        ~UserInterface();

        static bool capture_keyboard();
        static void new_frame();
        static void update(UIComponent *component, bool *p_open);
        static void render();
        static void compile();

        static void register_glfw(GLFWwindow *state);
    private:
        float m_ui_scale = 1.25f;
    };
}
