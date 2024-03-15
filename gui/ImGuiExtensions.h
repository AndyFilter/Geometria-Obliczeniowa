#pragma once

#include "../External/ImGui/imgui.h"
#include "../structures.h"

#define LINE_BASE_COLOR ImGui::GetColorU32(ImGuiCol_Button)
#define LINE_SPECIAL_COLOR ImGui::GetColorU32(ImGuiCol_PlotHistogram)

#define POINT_BASE_RADIUS 8

namespace ImGui {
    void DrawPoint(Vec2& pos, const char* label, ImDrawList* dl, float radius = POINT_BASE_RADIUS, ImU32 col = LINE_BASE_COLOR);
    void DrawCanvas(ImDrawList* dl, Vec2 pos, Vec2 size = {0,0});
    void DrawArrow(ImDrawList* dl, Vec2 start, Vec2 end, ImU32 col = LINE_BASE_COLOR, float thickness = 1);
    bool DirectionalLineParams(DirectionalLineFunc& func);
    bool GeneralLineParams(GeneralLineFunc& func, int idx = 0);

    // Helpers
    inline Vec2 GetGeneralFuncOffset(GeneralLineFunc& func) {
        short mean_val = func.A * func.B == 0 ? 1 : 2;
        return Vec2(func.A == 0 ? 0 : (func.C / func.A), func.B == 0 ? 0 : (-func.C / func.B)) / 20 / mean_val;

        //short mean_val = func1.A * func1.B == 0 ? 1 : 2;
        //Vec2 free_offset = Vec2(func1.A == 0 ? 0 : (-func1.C / func1.A), func1.B == 0 ? 0 : (func1.C / func1.B)) / 20 / mean_val * avail;
    }
};