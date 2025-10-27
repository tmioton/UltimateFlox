module;
#include "pch.hpp"
export module Input;

namespace input {
    export enum class Activation { Press, Release };

    // Explicitly defined key codes for save files, mostly follow GLFW codes.
    export enum class Key : std::uint32_t {
        None         = 0,
        Space        = 32,
        Apostrophe   = 39,
        Comma        = 44,
        Minus        = 45,
        Period       = 46,
        Slash        = 47,
        Zero         = 48,
        One          = 49,
        Two          = 50,
        Three        = 51,
        Four         = 52,
        Five         = 53,
        Six          = 54,
        Seven        = 55,
        Eight        = 56,
        Nine         = 57,
        Semicolon    = 59,
        Equal        = 61,
        A            = 65,
        B            = 66,
        C            = 67,
        D            = 68,
        E            = 69,
        F            = 70,
        G            = 71,
        H            = 72,
        I            = 73,
        J            = 74,
        K            = 75,
        L            = 76,
        M            = 77,
        N            = 78,
        O            = 79,
        P            = 80,
        Q            = 81,
        R            = 82,
        S            = 83,
        T            = 84,
        U            = 85,
        V            = 86,
        W            = 87,
        X            = 88,
        Y            = 89,
        Z            = 90,
        LeftBracket  = 91,
        Backslash    = 92,
        RightBracket = 93,
        Grave        = 96,
        World1       = 161,
        World2       = 162,

        Escape       = 256,
        Enter        = 257,
        Tab          = 258,
        Backspace    = 259,
        Insert       = 260,
        Delete       = 261,
        Right        = 262,
        Left         = 263,
        Down         = 264,
        Up           = 265,
        PageUp       = 266,
        PageDown     = 267,
        Home         = 268,
        End          = 269,
        CapsLock     = 280,
        ScrollLock   = 281,
        NumLock      = 282,
        PrintScreen  = 283,
        Pause        = 284,
        F1           = 290,
        F2           = 291,
        F3           = 292,
        F4           = 293,
        F5           = 294,
        F6           = 295,
        F7           = 296,
        F8           = 297,
        F9           = 298,
        F10          = 299,
        F11          = 300,
        F12          = 301,
        F13          = 302,
        F14          = 303,
        F15          = 304,
        F16          = 305,
        F17          = 306,
        F18          = 307,
        F19          = 308,
        F20          = 309,
        F21          = 310,
        F22          = 311,
        F23          = 312,
        F24          = 313,
        F25          = 314,
        KP0          = 320,
        KP1          = 321,
        KP2          = 322,
        KP3          = 323,
        KP4          = 324,
        KP5          = 325,
        KP6          = 326,
        KP7          = 327,
        KP8          = 328,
        KP9          = 329,
        KPDecimal    = 330,
        KPDivide     = 331,
        KPMultiply   = 332,
        KPSubtract   = 333,
        KPAdd        = 334,
        KPEnter      = 335,
        KPEqual      = 336,
        LeftShift    = 340,
        LeftControl  = 341,
        LeftAlt      = 342,
        LeftSuper    = 343,
        RightShift   = 344,
        RightControl = 345,
        RightAlt     = 346,
        RightSuper   = 347,
        Menu         = 348,
        Last = Menu,
    };

    export const char *to_string(const Key e) {
        switch (e) {
            case Key::None:
            default: return "None";
            case Key::Space: return "Space";
            case Key::Apostrophe: return "Apostrophe";
            case Key::Comma: return "Comma";
            case Key::Minus: return "Minus";
            case Key::Period: return "Period";
            case Key::Slash: return "Slash";
            case Key::Zero: return "Zero";
            case Key::One: return "One";
            case Key::Two: return "Two";
            case Key::Three: return "Three";
            case Key::Four: return "Four";
            case Key::Five: return "Five";
            case Key::Six: return "Six";
            case Key::Seven: return "Seven";
            case Key::Eight: return "Eight";
            case Key::Nine: return "Nine";
            case Key::Semicolon: return "Semicolon";
            case Key::Equal: return "Equal";
            case Key::A: return "A";
            case Key::B: return "B";
            case Key::C: return "C";
            case Key::D: return "D";
            case Key::E: return "E";
            case Key::F: return "F";
            case Key::G: return "G";
            case Key::H: return "H";
            case Key::I: return "I";
            case Key::J: return "J";
            case Key::K: return "K";
            case Key::L: return "L";
            case Key::M: return "M";
            case Key::N: return "N";
            case Key::O: return "O";
            case Key::P: return "P";
            case Key::Q: return "Q";
            case Key::R: return "R";
            case Key::S: return "S";
            case Key::T: return "T";
            case Key::U: return "U";
            case Key::V: return "V";
            case Key::W: return "W";
            case Key::X: return "X";
            case Key::Y: return "Y";
            case Key::Z: return "Z";
            case Key::LeftBracket: return "LeftBracket";
            case Key::Backslash: return "Backslash";
            case Key::RightBracket: return "RightBracket";
            case Key::Grave: return "Grave";
            case Key::World1: return "World1";
            case Key::World2: return "World2";
            case Key::Escape: return "Escape";
            case Key::Enter: return "Enter";
            case Key::Tab: return "Tab";
            case Key::Backspace: return "Backspace";
            case Key::Insert: return "Insert";
            case Key::Delete: return "Delete";
            case Key::Right: return "Right";
            case Key::Left: return "Left";
            case Key::Down: return "Down";
            case Key::Up: return "Up";
            case Key::PageUp: return "PageUp";
            case Key::PageDown: return "PageDown";
            case Key::Home: return "Home";
            case Key::End: return "End";
            case Key::CapsLock: return "CapsLock";
            case Key::ScrollLock: return "ScrollLock";
            case Key::NumLock: return "NumLock";
            case Key::PrintScreen: return "PrintScreen";
            case Key::Pause: return "Pause";
            case Key::F1: return "F1";
            case Key::F2: return "F2";
            case Key::F3: return "F3";
            case Key::F4: return "F4";
            case Key::F5: return "F5";
            case Key::F6: return "F6";
            case Key::F7: return "F7";
            case Key::F8: return "F8";
            case Key::F9: return "F9";
            case Key::F10: return "F10";
            case Key::F11: return "F11";
            case Key::F12: return "F12";
            case Key::F13: return "F13";
            case Key::F14: return "F14";
            case Key::F15: return "F15";
            case Key::F16: return "F16";
            case Key::F17: return "F17";
            case Key::F18: return "F18";
            case Key::F19: return "F19";
            case Key::F20: return "F20";
            case Key::F21: return "F21";
            case Key::F22: return "F22";
            case Key::F23: return "F23";
            case Key::F24: return "F24";
            case Key::F25: return "F25";
            case Key::KP0: return "KP0";
            case Key::KP1: return "KP1";
            case Key::KP2: return "KP2";
            case Key::KP3: return "KP3";
            case Key::KP4: return "KP4";
            case Key::KP5: return "KP5";
            case Key::KP6: return "KP6";
            case Key::KP7: return "KP7";
            case Key::KP8: return "KP8";
            case Key::KP9: return "KP9";
            case Key::KPDecimal: return "KPDecimal";
            case Key::KPDivide: return "KPDivide";
            case Key::KPMultiply: return "KPMultiply";
            case Key::KPSubtract: return "KPSubtract";
            case Key::KPAdd: return "KPAdd";
            case Key::KPEnter: return "KPEnter";
            case Key::KPEqual: return "KPEqual";
            case Key::LeftShift: return "LeftShift";
            case Key::LeftControl: return "LeftControl";
            case Key::LeftAlt: return "LeftAlt";
            case Key::LeftSuper: return "LeftSuper";
            case Key::RightShift: return "RightShift";
            case Key::RightControl: return "RightControl";
            case Key::RightAlt: return "RightAlt";
            case Key::RightSuper: return "RightSuper";
            case Key::Menu: return "Menu";
        }
    }

    export std::array all_keys {
        Key::Space,
        Key::Apostrophe,
        Key::Comma,
        Key::Minus,
        Key::Period,
        Key::Slash,
        Key::Zero,
        Key::One,
        Key::Two,
        Key::Three,
        Key::Four,
        Key::Five,
        Key::Six,
        Key::Seven,
        Key::Eight,
        Key::Nine,
        Key::Semicolon,
        Key::Equal,
        Key::A,
        Key::B,
        Key::C,
        Key::D,
        Key::E,
        Key::F,
        Key::G,
        Key::H,
        Key::I,
        Key::J,
        Key::K,
        Key::L,
        Key::M,
        Key::N,
        Key::O,
        Key::P,
        Key::Q,
        Key::R,
        Key::S,
        Key::T,
        Key::U,
        Key::V,
        Key::W,
        Key::X,
        Key::Y,
        Key::Z,
        Key::LeftBracket,
        Key::Backslash,
        Key::RightBracket,
        Key::Grave,
        Key::World1,
        Key::World2,

        Key::Escape,
        Key::Enter,
        Key::Tab,
        Key::Backspace,
        Key::Insert,
        Key::Delete,
        Key::Right,
        Key::Left,
        Key::Down,
        Key::Up,
        Key::PageUp,
        Key::PageDown,
        Key::Home,
        Key::End,
        Key::CapsLock,
        Key::ScrollLock,
        Key::NumLock,
        Key::PrintScreen,
        Key::Pause,
        Key::F1,
        Key::F2,
        Key::F3,
        Key::F4,
        Key::F5,
        Key::F6,
        Key::F7,
        Key::F8,
        Key::F9,
        Key::F10,
        Key::F11,
        Key::F12,
        Key::F13,
        Key::F14,
        Key::F15,
        Key::F16,
        Key::F17,
        Key::F18,
        Key::F19,
        Key::F20,
        Key::F21,
        Key::F22,
        Key::F23,
        Key::F24,
        Key::F25,
        Key::KP0,
        Key::KP1,
        Key::KP2,
        Key::KP3,
        Key::KP4,
        Key::KP5,
        Key::KP6,
        Key::KP7,
        Key::KP8,
        Key::KP9,
        Key::KPDecimal,
        Key::KPDivide,
        Key::KPMultiply,
        Key::KPSubtract,
        Key::KPAdd,
        Key::KPEnter,
        Key::KPEqual,
        Key::LeftShift,
        Key::LeftControl,
        Key::LeftAlt,
        Key::LeftSuper,
        Key::RightShift,
        Key::RightControl,
        Key::RightAlt,
        Key::RightSuper,
        Key::Menu
    };

    export enum class MouseButton : std::uint32_t {
        None = 0,
        Mouse1 = 400,
        Mouse2 = 401,
        Mouse3 = 402,
        Mouse4 = 403,
        Mouse5 = 404,
        Mouse6 = 405,
        Mouse7 = 406,
        Mouse8 = 407,
        MouseLeft = Mouse1,
        MouseRight = Mouse2,
        MouseMiddle = Mouse3,
        MouseForward = Mouse4,
        MouseBack = Mouse5,
        Last = Mouse8,
    };

    export const char *to_string(const MouseButton button) {
        switch (button) {
            case MouseButton::None:
            default: return "None";
            case MouseButton::Mouse1: return "Mouse1";
            case MouseButton::Mouse2: return "Mouse2";
            case MouseButton::Mouse3: return "Mouse3";
            case MouseButton::Mouse4: return "Mouse4";
            case MouseButton::Mouse5: return "Mouse5";
            case MouseButton::Mouse6: return "Mouse6";
            case MouseButton::Mouse7: return "Mouse7";
            case MouseButton::Mouse8: return "Mouse8";
        }
    }
}
