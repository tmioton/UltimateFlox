#pragma once

#include "pch.hpp"
#include "Event.hpp"


extern constexpr size_t eventStackCapacity = 3;


struct Config {
    int width;
    int height;

    uint8_t samples = 8;
    bool resizable = false;  // This can be converted to a flag bit type thing when additional features are required.
};


class Window {
public:
    const Config config;
private:
    GLFWwindow *m_window;

    std::vector<Event> m_events;

    static Window *getState(GLFWwindow *window);

    void terminate();

public:
    Window(int width, int height, const char *title, GLFWmonitor *monitor = nullptr);

    bool shouldClose();

    void shouldClose(bool value);

    void swapBuffers();

    static void update();

    static void clear();

    void pushEvent(Event event);

    std::optional<Event> pollEvent();
};
