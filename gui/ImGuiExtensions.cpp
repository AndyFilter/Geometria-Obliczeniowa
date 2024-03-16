#include "ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"

#include <iostream>

bool ImGui::DrawPoint(Vec2 pos, const char *label, ImDrawList* dl, float radius, ImU32 col) {
    ImGui::RenderTextClipped(pos - Vec2(50, 20), pos + Vec2(50, 0), label, nullptr, nullptr, {0.5, 0});
    dl->AddCircleFilled(pos, radius, col);

    if(IsMouseHoveringRect(pos - Vec2(radius,radius), pos + Vec2(radius,radius))) {
        return true;
    }

    return false;
}

void ImGui::DrawCanvas(ImDrawList* dl, Vec2 pos, Vec2 size) {
    for(int x = 1; x < 10; ++x) {
        dl->AddLine(pos + Vec2(size.x * (x / CANVAS_SIZE),0), pos + Vec2(size.x * (x / CANVAS_SIZE), size.y), GetColorU32(ImGuiCol_Border));
    }
    for(int y = 1; y < 10; ++y) {
        dl->AddLine(pos + Vec2(0,size.y * (y / CANVAS_SIZE)), pos + Vec2(size.x, size.y * (y / CANVAS_SIZE)), GetColorU32(ImGuiCol_Border));
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

bool ImGui::PointParams(Vec2 &P1, int idx) {
    bool changed = false;

    PushID(idx);

    Indent(4);
    //ImGui::DragFloat2("##P1", &P1.x, 0.05, -10, 10);
    SetNextItemWidth(GetContentRegionAvail().x);
    changed = SliderFloat2("##P1", &P1.x, -10, 10,"%.2f");
    Unindent(4);

    PopID();

    return changed;
}


// Thanks to "carasuca" on GitHub for this code!
int rotation_start_index;
void ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}
ImVec2 ImRotationCenter()
{
    using namespace ImGui;
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return {(l.x+u.x)/2, (l.y+u.y)/2}; // or use _ClipRectStack?
}
void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
{
    using namespace ImGui;
    float s=sin(rad), c=cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}


void ImGui::DrawDistanceLine(Vec2 p1, Vec2 p2, ImDrawList *dl, float distance_if_known, ImU32 col, float thickness) {
    float dist = distance_if_known == FLT_MAX ? p1.dist(p2) : distance_if_known;

    dl->AddLine(p1, p2, col, thickness);

    // 2 works better, but I like "<3" more
    if(dist <3)
        return;

    Vec2 dir = p1 - p2;
    float ang = atanf(dir.x / dir.y);

    ImRect text_bb = { {fminf(p1.x, p2.x) - 100, fminf(p1.y, p2.y) - 100}, {fmaxf(p1.x, p2.x) + 100, fmaxf(p1.y, p2.y) + 100} };
    //Vec2 center = text_bb.GetCenter();
    //float s = sinf(ang), c = cosf(ang);
    //center = ImRotate(center, s, c) - center;
    //ImRect rot_text_bb = { ImRotate(text_bb.Min, s, c) - center, ImRotate(text_bb.Max, s, c) - center };
    //RenderFrame(text_bb.Min, text_bb.Max, LINE_BASE_COLOR);
    ImRotateStart();
    //RenderFrame(text_bb.Min, text_bb.Max, LINE_BASE_COLOR);
    char buffer[10];  // maximum expected length of the float
    std::snprintf(buffer, 10, "%.2f", dist);
    RenderTextClipped(text_bb.Min, text_bb.Max, buffer, nullptr, nullptr, {0.5, 0.5});
    if(ang < 0)
        ang = M_PIf + ang;
    ImRotateEnd(ang);
}
