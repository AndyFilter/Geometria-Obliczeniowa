#include "ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"

#include <iostream>

bool ImGui::DrawPoint(Vec2 pos, const char *label, ImDrawList* dl, float radius, ImU32 col) {
    ImGui::RenderTextClipped(pos - Vec2(50, 20), pos + Vec2(50, 0), label, nullptr, nullptr, {0.5, 0});
    dl->AddCircleFilled(pos, radius, col);

    if(IsMouseHoveringRect(pos - Vec2(radius,radius), pos + Vec2(radius,radius))) {
        SetMouseCursor(ImGuiMouseCursor_Hand);
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

void ImGui::DrawConvexHull(PointCloud cp, ImDrawList *dl, bool draw_outline, float point_size,
                           Vec2 size, Vec2 offset, ImU32 hull_points_col, Vec2 pos_offset) {
    bool should_transform = size.x != -1;

    for(auto& point : cp.points) {
        auto pos = should_transform ? Local2Canvas(point + pos_offset, size, offset) : (point + pos_offset);
        dl->AddCircleFilled(pos, point_size, POINT_BASE_COLOR);
    }

    if(!draw_outline || cp.hull_points.empty())
        return;

    auto last_pos = cp.points[cp.hull_points[cp.hull_points.size() - 1]];
    last_pos = should_transform ? Local2Canvas(last_pos + pos_offset, size, offset) : (last_pos + pos_offset);
    for(auto& point_idx : cp.hull_points) {
        auto point = cp.points[point_idx];
        auto pos = should_transform ? Local2Canvas(point + pos_offset, size, offset) : (point + pos_offset);
        dl->AddLine(pos, last_pos, hull_points_col, LINE_BASE_THICKNESS);
        last_pos = pos;
        dl->AddCircleFilled(pos, point_size, hull_points_col);
    }
}

int _tree_height = 0;

void draw_tree_node_1D(RangeTree1D::Node* node, Vec2 pos, int level = 0) {
    if(!node)
        return;
    auto dl = ImGui::GetWindowDrawList();

    //float hf = powf(2, -level) * powf(2, _tree_height-2) * TREE_NODE_RADIUS; // Working
    float hf = powf(2, _tree_height-2-level) * TREE_NODE_RADIUS; // Working
    //float hf = powf(2, height - 1) * TREE_NODE_RADIUS;

    if(node->left) {
        auto newPos = pos + Vec2(-hf, 60);
        dl->AddLine(pos, newPos, TREE_NODE_LINE_COLOR, LINE_BASE_THICKNESS);
        draw_tree_node_1D(node->left, newPos, level + 1);
    }
    if(node->right) {
        auto newPos = pos + Vec2(hf, 60);
        dl->AddLine(pos, newPos, TREE_NODE_LINE_COLOR, LINE_BASE_THICKNESS);
        draw_tree_node_1D(node->right, newPos, level + 1);
    }

    dl->AddCircleFilled(pos, TREE_NODE_RADIUS, node->is_selected ? TREE_NODE_SPECIAL_COLOR : TREE_NODE_COLOR);
    char buf[10];
    sprintf(buf, "%.1f", node->value);
    ImGui::RenderTextClipped(pos - TREE_NODE_RADIUS, pos + TREE_NODE_RADIUS, buf, 0, 0, {0.5, 0.5});
}

int ImGui::DrawTree1D(RangeTree1D* tree, Vec2 pos) {
    _tree_height = tree->height;
    draw_tree_node_1D(tree->head, pos);

    return 0;
}


void draw_tree_node_2D(RangeTree2D::Node* node, Vec2 pos, int level = 0, bool is_sub_tree = false) {
    if(!node)
        return;
    auto dl = ImGui::GetWindowDrawList();

    const float y_scale = 0.9f;
    const float ADJ_RADIUS = is_sub_tree ? (TREE_NODE_RADIUS * y_scale) : TREE_NODE_RADIUS;
    const float ADJ_HEIGHT = is_sub_tree ? (TREE_NODE_HEIGHT_2D * y_scale) : TREE_NODE_HEIGHT_2D;

    float hf = powf(2, _tree_height-2-level) * ADJ_RADIUS; // Working

    if(node->left) {
        auto newPos = pos + Vec2(-hf, ADJ_HEIGHT);
        dl->AddLine(pos, newPos, TREE_NODE_LINE_COLOR, LINE_BASE_THICKNESS);
        draw_tree_node_2D(node->left, newPos, level + 1, is_sub_tree);
    }
    if(node->right) {
        auto newPos = pos + Vec2(hf, ADJ_HEIGHT);
        dl->AddLine(pos, newPos, TREE_NODE_LINE_COLOR, LINE_BASE_THICKNESS);
        draw_tree_node_2D(node->right, newPos, level + 1, is_sub_tree);
    }

    Vec2 margin = ImGui::GetStyle().WindowPadding;
    //ImDrawList* bkp_dl = nullptr;

    // Start drawing the sub tree
    if(node->sub_left && !is_sub_tree &&
       ImGui::IsMouseHoveringRect(pos - Vec2(ADJ_RADIUS,ADJ_RADIUS), pos + Vec2(ADJ_RADIUS,ADJ_RADIUS))) {

        // w, h are the new (scaled) parameters defining the tree shape
        float w = (TREE_NODE_RADIUS * y_scale), h = (TREE_NODE_HEIGHT_2D * y_scale);
        // sub_tree's width (assuming all balance)
        float width = (2 << (_tree_height - level - 2)) * w;
        Vec2 sub_tree_offset = Vec2(0,  h);
        Rect bb = {pos - Vec2(width,TREE_NODE_RADIUS) - margin,
                   pos + Vec2(width,(_tree_height - level - 1) * h + w) + margin};

        //bkp_dl = ImGui::GetCurrentWindow()->DrawList;
        //ImGui::GetCurrentWindow()->DrawList = ImGui::GetForegroundDrawList(ImGui::GetCurrentWindow());
        //dl = ImGui::GetCurrentWindow()->DrawList;

        dl->AddRectFilled(bb.min, bb.max, TREE_NODE_FRAME_BG, 10);
        dl->AddRect(bb.min, bb.max, LINE_BASE_COLOR, 10); // Draw frame

        auto child_pos = pos + sub_tree_offset;
        if(node->sub_left) {
            auto newPos = child_pos + Vec2(-width/2, 0);
            dl->AddLine(pos, newPos, TREE_NODE_LINE_COLOR, LINE_BASE_THICKNESS);
            draw_tree_node_2D(node->sub_left, newPos, level + 1, true);
        }
        if(node->sub_right) {
            auto newPos = child_pos + Vec2(width/2, 0);
            dl->AddLine(pos, newPos, TREE_NODE_LINE_COLOR, LINE_BASE_THICKNESS);
            draw_tree_node_2D(node->sub_right, newPos, level + 1, true);
        }
    }

    dl->AddCircleFilled(pos, ADJ_RADIUS, node->is_selected ? TREE_NODE_SPECIAL_COLOR : TREE_NODE_COLOR);
    char buf[10];
    sprintf(buf, "%.1f\n%.1f", node->value.x, node->value.y);

    // Apply all the transformation needed to draw the subtree
    if(is_sub_tree) {
        // kinda hacky way to change the font's size
        auto font = ImGui::GetFont();
        float ogScale = font->Scale;
        font->Scale = sqrt(y_scale);
        ImGui::PushFont(font);
        ImGui::RenderTextClipped(pos - ADJ_RADIUS, pos + ADJ_RADIUS, buf, 0, 0, {0.5, 0.5});
        font->Scale = ogScale;
        ImGui::PopFont();
    }
    else
        ImGui::RenderTextClipped(pos - ADJ_RADIUS, pos + ADJ_RADIUS, buf, 0, 0, {0.5, 0.5});

    //if(bkp_dl)
    //    ImGui::GetCurrentWindow()->DrawList = bkp_dl;
}

int ImGui::DrawTree2D(RangeTree2D *tree, Vec2 pos) {
    _tree_height = tree->height;
    draw_tree_node_2D(tree->head, pos);

    return 0;
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

bool ImGui::PointParams(Vec2 &P1, int idx, float w) {
    bool changed = false;

    // - 4 to account for the future indent
    float width = (w == -1 ? GetContentRegionAvail().x : w) - 4;

    PushID(idx);

    Indent(4);
    //ImGui::DragFloat2("##P1", &P1.x, 0.05, -10, 10);
    if(w != 0)
        SetNextItemWidth(width);
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

bool ImGui::PolygonParameters(Polygon& poly, int idx) {
    bool dirty = false;

    PushStyleColor(ImGuiCol_ChildBg, (ImVec4)ImColor(255, 255, 255, 4));
    if(BeginChild("PolyParams", {-1, 0},ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY,
                  ImGuiWindowFlags_NoSavedSettings)) {

        Vec2 slider_avail = GetContentRegionAvail();
        float frame_size = GetFrameHeight();
        slider_avail.x -= frame_size + GetStyle().ItemSpacing.x;

        //if(BeginMenuBar()) {
        //    if(BeginMenu("Parametry Wielokąta")) {
        //        ImGui::EndMenu();
        //    }
        //    ImGui::EndMenuBar();
        //}

        SeparatorText("Wielokąt");

        for (int i = 0; i < poly.vtx.size(); i++) {
            Text("Punkt P%i", i + 1);
            dirty |= PointParams(poly.vtx[i], (idx + 1) * (i + 1), slider_avail.x);

            SameLine();

            PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(DELETE_BUTTON_HUE, 0.63, 0.54));
            PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(DELETE_BUTTON_HUE, 0.7f, 0.7f));
            PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(DELETE_BUTTON_HUE, 0.8f, 0.8f));

            PushID(i);
            if(Button("X", {frame_size, frame_size})){
                poly.vtx.erase(poly.vtx.begin() + i);
                i--;
            }
            PopID();

            PopStyleColor(3);
        }

        //Spacing();Spacing();Spacing();
        Dummy({0, GetStyle().ItemSpacing.y});

        if(Button("+", {-1, 0})){
            poly.AddVertex();
        }
        if(IsItemHovered())
            SetMouseCursor(ImGuiMouseCursor_Hand);
    }
    PopStyleColor();
    EndChild();

    return dirty;
}
