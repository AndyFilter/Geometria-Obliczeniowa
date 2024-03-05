#include "ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"

#include <iostream>

void ImGui::DrawPoint(Vec2& pos, const char *label, ImDrawList* dl, float radius, ImU32 col) {
    ImGui::RenderTextClipped(pos - Vec2(50, 20), pos + Vec2(50, 0), label, nullptr, nullptr, {0.5, 0});
    dl->AddCircleFilled(pos, radius, col);

    //if(IsMouseHoveringRect(pos - Vec2(8,8), pos + Vec2(8,8))) {
    //    SetMouseCursor(ImGuiMouseCursor_Hand);
    //
    //    if(IsMouseDragging(0)) {
    //        return true;
    //    }
    //}
    //
    //return false;
}

void ImGui::DrawCanvas(ImDrawList* dl, Vec2 pos, Vec2 size) {
    for(int x = 1; x < 10; ++x) {
        dl->AddLine(pos + Vec2(size.x * (x / 10.f),0), pos + Vec2(size.x * (x / 10.f), size.y), GetColorU32(ImGuiCol_Border));
    }
    for(int y = 1; y < 10; ++y) {
        dl->AddLine(pos + Vec2(0,size.y * (y / 10.f)), pos + Vec2(size.x, size.y * (y / 10.f)), GetColorU32(ImGuiCol_Border));
    }
}
