// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once

#include "pch.hpp"

namespace window {
    struct KeyboardEvent { int scancode; int16_t key, mods; };
    struct TextEvent { unsigned int codepoint; };
    struct MouseMotionEvent { double x_pos, y_pos; };
    struct MouseButtonEvent { int button, mods; };
    struct ScrollEvent { double x_offset, y_offset; };

    using AnonymousEvent = \
std::variant<
        KeyboardEvent,
        TextEvent,
        MouseMotionEvent,
        MouseButtonEvent,
        ScrollEvent
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
            MouseUp,

            // ScrollEvent
            Scroll
        };

        Type type;
        AnonymousEvent event;

        Event();
        Event(Type type, AnonymousEvent event);
    };
}
