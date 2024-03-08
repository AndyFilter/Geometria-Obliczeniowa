#pragma once

#include "../External/ImGui/imgui.h"
#include "../structures.h"

namespace ImGui {
    void DrawPoint(Vec2& pos, const char* label, ImDrawList* dl, float radius = 8, ImU32 col = GetColorU32(ImGuiCol_Button));
    void DrawCanvas(ImDrawList* dl, Vec2 pos, Vec2 size = {0,0});
    void DrawArrow(ImDrawList* dl, Vec2 start, Vec2 end, ImU32 col = GetColorU32(ImGuiCol_Button), float thickness = 1);
};