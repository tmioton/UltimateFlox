module;
#include "pch.hpp"
export module Event;

import Input;

namespace input {
    export struct MouseButtonEvent {
        MouseButton button;
        Activation  action;
        int         mods;
        glm::dvec2  cursor;
    };

    export struct CursorEvent {
        glm::dvec2 cursor;
    };

    export struct ScrollEvent {
        glm::dvec2 offset;
        glm::dvec2 cursor;
    };

    export struct KeyEvent {
        Key        key;
        int        scancode;
        Activation action;
        int        mods;
        glm::dvec2 cursor;
    };

    export struct TextEvent {
        unsigned int codepoint;
    };

    export struct ModifiedTextEvent {
        unsigned int codepoint;
        int          mods;
    };

    export struct Event {
        enum class Type { MouseButton, Cursor, Scroll, Key, Text, ModifiedText };
        Type type;
        union {
            MouseButtonEvent  button;
            CursorEvent       cursor;
            ScrollEvent       scroll;
            KeyEvent          key;
            TextEvent         text;
            ModifiedTextEvent modified_text;
        };
    };

    export std::string_view to_string(const Event::Type type) {
        using enum Event::Type;
        switch (type) {
            default: return "None";
            case MouseButton: return "MouseButton";
            case Cursor: return "Cursor";
            case Scroll: return "Scroll";
            case Key: return "Key";
            case Text: return "Text";
            case ModifiedText: return "ModifiedText";
        }
    }
} // namespace input
