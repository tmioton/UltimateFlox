#include "pch.hpp"
#include "UIDemoWindow.hpp"


void ui::UIDemoWindow::draw(bool *p_open) {
    ImGui::ShowDemoWindow(p_open);
}
