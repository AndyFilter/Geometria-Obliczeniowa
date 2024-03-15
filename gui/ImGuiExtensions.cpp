#include "ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"

#include <iostream>

void ImGui::DrawPoint(Vec2& pos, const char *label, ImDrawList* dl, float radius, ImU32 col) {
    ImGui::RenderTextClipped(pos - Vec2(50, 20), pos + Vec2(50, 0), label, nullptr, nullptr, {0.5, 0});
    dl->AddCircleFilled(pos, radius, col);

    /*
    if(IsMouseHoveringRect(pos - Vec2(8,8), pos + Vec2(8,8))) {
        SetMouseCursor(ImGuiMouseCursor_Hand);

        if(IsMouseDragging(0)) {
            return true;
        }
    }

    return false;
     */
}

void ImGui::DrawCanvas(ImDrawList* dl, Vec2 pos, Vec2 size) {
    for(int x = 1; x < 10; ++x) {
        dl->AddLine(pos + Vec2(size.x * (x / 10.f),0), pos + Vec2(size.x * (x / 10.f), size.y), GetColorU32(ImGuiCol_Border));
    }
    for(int y = 1; y < 10; ++y) {
        dl->AddLine(pos + Vec2(0,size.y * (y / 10.f)), pos + Vec2(size.x, size.y * (y / 10.f)), GetColorU32(ImGuiCol_Border));
    }
}

void ImGui::DrawArrow(ImDrawList *dl, Vec2 start, Vec2 end, ImU32 col, float thickness) {
    dl->AddLine(start, end, col, thickness);

    constexpr float sin_45 = 0.707106781186547;
    Vec2 dir = Vec2(end - start);
    Vec2 rotated_dir1 = (Vec2(dir.x - dir.y, dir.x + dir.y) * sin_45).normal() * 20;
    dl->AddLine(end, end - rotated_dir1, col, thickness);

    Vec2 rotated_dir2 = (Vec2(dir.x + dir.y, -dir.x + dir.y) * sin_45).normal() * 20;
    dl->AddLine(end, end - rotated_dir2, col, thickness);
}

bool ImGui::DirectionalLineParams(DirectionalLineFunc &func) {

    ImGui::Text("y ="); ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2, 2});
    ImGui::DragFloat("##LineFunc_a", &func.a, 0.05, -10, 10, "%.2fx");
    ImGui::SameLine();
    ImGui::Text("+");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50);
    ImGui::DragFloat("##LineFunc_b", &func.b, 0.05, -10, 10, "%.2f");
    ImGui::PopStyleVar();

    return false;
}

bool ImGui::GeneralLineParams(GeneralLineFunc &func, int idx) {

    bool changed = false;

    ImGui::PushID(idx);
    ImGui::Indent(4);
    ImGui::SetNextItemWidth(80);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2, 2});
    changed |= ImGui::DragFloat("##LineFunc_A", &func.A, 0.05, -10, 10, "%.2fx");
    ImGui::Unindent(4);
    ImGui::SameLine();
    ImGui::Text("+");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    changed |= ImGui::DragFloat("##LineFunc_B", &func.B, 0.05, -10, 10, "%.2fy");
    ImGui::SameLine();
    ImGui::Text("+");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(50);
    changed |= ImGui::DragFloat("##LineFunc_C", &func.C, 0.05, -10, 10, "%.2f");
    ImGui::SameLine();
    ImGui::Text("= 0");
    ImGui::PopStyleVar();
    ImGui::PopID();

    return changed;
}
