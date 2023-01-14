#pragma once

#include "pch.hpp"

namespace window {
    struct KeyboardEvent { int scancode; int16_t key, mods; };
    struct TextEvent { unsigned int codepoint; };
    struct MouseMotionEvent { double xpos, ypos; };
    struct MouseButtonEvent { int button, mods; };

    using AnonymousEvent = \
std::variant<
        KeyboardEvent,
        TextEvent,
        MouseMotionEvent,
        MouseButtonEvent
    >;


    struct Event {
    public:
        enum class Type {
            // KeyboardEvent
            KeyPress,
            KeyRelease,
            KeyRepeat,

            // TextEvent
            TextInput,

            // MouseMotionEvent
            MouseMotion,

            // MouseButtonEvent
            MouseDown,
            MouseUp
        };

        Type type;
        AnonymousEvent event;

        Event();
        Event(Type type, AnonymousEvent event);
    };
}
