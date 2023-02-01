#pragma once
#include "pch.hpp"

namespace ui {
    class UIComponent {
    public:
        virtual ~UIComponent() = default;
        virtual void draw(bool *p_open) = 0;
    };
}
