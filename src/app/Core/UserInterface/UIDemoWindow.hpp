#pragma once
#include "pch.hpp"
#include "UIComponent.hpp"


namespace ui {
    class UIDemoWindow final : public UIComponent {
    public:
        void draw(bool *p_open) override;
        //~UIDemoWindow() override = default;
    };
}
