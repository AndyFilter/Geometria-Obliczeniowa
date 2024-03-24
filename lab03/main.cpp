#include <iostream>

#include "../gui/gui.h"
#include "../gui/ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"
#include "../Helpers.h"

int OnGui()
{
    ImGuiStyle& style = ImGui::GetStyle();

    if(ImGui::BeginTabBar("Items")) {

        if(ImGui::BeginTabItem("Pole trójkąta"))
        {
            static Triangle triangle{Vec2{-5, 7}, {2, -5}, {2, 1}};

            static Vec2* dragging_point = nullptr;
            static bool dirty = true;

            static float area = triangle.GetArea();

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt P1:");
                ImGui::PointParams(triangle.vtx[0], 0);
                ImGui::Text("Punkt P2:");
                ImGui::PointParams(triangle.vtx[1], 1);
                ImGui::Text("Punkt P3:");
                ImGui::PointParams(triangle.vtx[2], 2);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Powierzchnia trójkąta to: %.2f", area);
            }
            ImGui::EndChild();

            if(dirty) {
                area = triangle.GetArea();
                dirty = false;
            }

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                Vec2 pos1 = ImGui::Local2Canvas(triangle.vtx[0], avail, startPos);
                Vec2 pos2 = ImGui::Local2Canvas(triangle.vtx[1], avail, startPos);
                Vec2 pos3 = ImGui::Local2Canvas(triangle.vtx[2], avail, startPos);

                dl->AddLine(pos1, pos2, LINE_BASE_COLOR, LINE_BASE_THICKNESS);
                dl->AddLine(pos1, pos3, LINE_BASE_COLOR, LINE_BASE_THICKNESS);
                dl->AddLine(pos2, pos3, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                // Draw Points
                if(ImGui::DrawPoint(pos1, "P1", dl) &&
                   ImGui::IsMouseClicked(0))
                    dragging_point = &triangle.vtx[0];

                if(ImGui::DrawPoint(pos2, "P2", dl) &&
                   ImGui::IsMouseClicked(0))
                    dragging_point = &triangle.vtx[1];

                if(ImGui::DrawPoint(pos3, "P3", dl) &&
                   ImGui::IsMouseClicked(0))
                    dragging_point = &triangle.vtx[2];

                if(area > 10) {
                    char buffer[10];  // maximum expected length of the float
                    std::snprintf(buffer, 10, "%.2f", area);
                    ImGui::RenderText(ImGui::Local2Canvas(triangle.GetMidPos(), avail, startPos), buffer);
                }

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
                    dirty = true;
                }
            }
            ImGui::EndChild();


            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Kąt pomiędzy liniami"))
        {
            static GeneralLineFunc func1(-8, 1, 1), func2(5, 7, 4);

            static float angle_rad = func1.GetAngleBetween(func2);

            static bool dirty = true;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Forma ogólna funkcji 1:");
                dirty |= ImGui::GeneralLineParams(func1);

                ImGui::Text("Forma ogólna funkcji 2:");
                dirty |= ImGui::GeneralLineParams(func2, 1);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("kąt pomiędzy liniami to: %.2f", RAD_2_DEG * angle_rad);
            }
            ImGui::EndChild();

            if(dirty) {
                angle_rad = func1.GetAngleBetween(func2);
                dirty = false;
            }

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                // Draw the angle arc
                Vec2 inter = func1.GetCollisionPoint(func2);
                float a1 = atan2f(func1.A, func1.B);
                dl->PathArcTo(ImGui::Local2Canvas(inter, avail, startPos),
                              20, a1 + angle_rad, a1 + 0.01, 10);
                dl->PathStroke(LINE_SPECIAL_COLOR, 0, LINE_BASE_THICKNESS);

                Vec2 funcOffset = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func1), avail, startPos);
                Vec2 dir = ImGui::GetGeneralLineDir(func1, avail) * 100000.f;
                dl->AddLine(funcOffset + dir, funcOffset - dir, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                funcOffset = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func2), avail, startPos);
                dir = ImGui::GetGeneralLineDir(func2, avail) * 100000.f;
                dl->AddLine(funcOffset + dir, funcOffset - dir, LINE_BASE_COLOR, LINE_BASE_THICKNESS);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Przynależność do trójkąta"))
        {
            static Triangle triangle{Vec2{-8, 3}, {1, -7}, {8, 7}};
            static Vec2 P1 {1, 0};

            static bool first_method = true;

            static Vec2* dragging_point = nullptr;
            static bool dirty = true;

            static bool is_inside = triangle.IsContained_Area(P1);

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Trójkąt P1:");
                dirty |= ImGui::PointParams(triangle.vtx[0], 0);
                ImGui::Text("Trójkąt P2:");
                dirty |= ImGui::PointParams(triangle.vtx[1], 1);
                ImGui::Text("Trójkąt P3:");
                dirty |= ImGui::PointParams(triangle.vtx[2], 2);

                ImGui::Text("Punkt P:");
                dirty |= ImGui::PointParams(P1, 3);

                ImGui::Spacing(); ImGui::Spacing();
                dirty |= ImGui::Checkbox("Metoda pól", &first_method);

                ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Punkt jest %s trójkąta", is_inside ? "wewnątrz" : "na zewnątrz");
            }
            ImGui::EndChild();

            if(dirty) {
                is_inside = first_method ? triangle.IsContained_Area(P1) : triangle.IsContained_Angles(P1);
                dirty = false;
            }

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                Vec2 pos1 = ImGui::Local2Canvas(triangle.vtx[0], avail, startPos);
                Vec2 pos2 = ImGui::Local2Canvas(triangle.vtx[1], avail, startPos);
                Vec2 pos3 = ImGui::Local2Canvas(triangle.vtx[2], avail, startPos);

                if(first_method) {
                    for(int i = 0; i < 3; i++) {
                        Triangle subTri = Triangle(P1, triangle.vtx[i], triangle.vtx[(i + 1) % 3]);
                        float area = subTri.GetArea();

                        if(area > 10) {
                            char buffer[10];  // maximum expected length of the float
                            std::snprintf(buffer, 10, "%.2f", area);
                            ImGui::RenderText(
                                    ImGui::Local2Canvas(subTri.GetMidPos(), avail, startPos),
                                    buffer);
                        }
                    }
                    dl->AddLine(ImGui::Local2Canvas(P1, avail, startPos), pos1,
                                SHAPE_FILL_COLOR + 0x22000000, LINE_BASE_THICKNESS);
                    dl->AddLine(ImGui::Local2Canvas(P1, avail, startPos), pos2,
                                SHAPE_FILL_COLOR + 0x22000000, LINE_BASE_THICKNESS);
                    dl->AddLine(ImGui::Local2Canvas(P1, avail, startPos), pos3,
                                SHAPE_FILL_COLOR + 0x22000000, LINE_BASE_THICKNESS);
                }

                dl->AddLine(pos1, pos2, LINE_BASE_COLOR, LINE_BASE_THICKNESS);
                dl->AddLine(pos1, pos3, LINE_BASE_COLOR, LINE_BASE_THICKNESS);
                dl->AddLine(pos2, pos3, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                dl->PathLineTo(pos1);
                dl->PathLineTo(pos2);
                dl->PathLineTo(pos3);
                dl->PathFillConvex(SHAPE_FILL_COLOR);

                // Draw Points
                if(ImGui::DrawPoint(pos1, "P1", dl) &&
                   ImGui::IsMouseClicked(0))
                    dragging_point = &triangle.vtx[0];

                if(ImGui::DrawPoint(pos2, "P2", dl) &&
                   ImGui::IsMouseClicked(0))
                    dragging_point = &triangle.vtx[1];

                if(ImGui::DrawPoint(pos3, "P3", dl) &&
                   ImGui::IsMouseClicked(0))
                    dragging_point = &triangle.vtx[2];

                if(ImGui::DrawPoint(ImGui::Local2Canvas(P1, avail, startPos),
                                    "P", dl, POINT_BASE_RADIUS, POINT_SPECIAL_COLOR) &&
                   ImGui::IsMouseClicked(0))
                    dragging_point = &P1;

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
                    dirty = true;
                }
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Przynależność punktu do wielokąta"))
        {
            static Polygon poly{Vec2{-5, 7}, {2, -5}, {2, 1}, {4, 8}};
            static Vec2 P1{-3, 7};

            static bool is_inside = poly.PointTest(P1);

            static Vec2* dragging_point = nullptr;
            static bool dirty = true;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt A:");
                dirty |= ImGui::PointParams(P1, 0);

                ImGui::Dummy({0, style.ItemSpacing.x});

                dirty |= ImGui::PolygonParameters(poly, 1);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Punkt %s wewnątrz wielokąta", is_inside ? "jest" : "nie jest");
            }
            ImGui::EndChild();

            if(dirty) {
                is_inside = poly.PointTest(P1);
                dirty = false;
            }

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                // Fill in the poly (doesn't work for concave shapes)
                //dl->PathClear();
                //for(int i = 0; i < poly.vtx.size(); i++) {
                //    dl->PathLineTo(ImGui::Local2Canvas(poly.vtx[(i + start_idx) % poly.vtx.size()], avail, startPos));
                //}
                //dl->PathFillConcave(SHAPE_FILL_COLOR);

                for(int i = 0; i < poly.vtx.size(); i++) {
                    dl->PathLineTo(ImGui::Local2Canvas(poly.vtx[i % poly.vtx.size()], avail, startPos));
                    dl->PathLineTo(ImGui::Local2Canvas(poly.vtx[(i + 1) % poly.vtx.size()], avail, startPos));
                    dl->PathStroke(LINE_BASE_COLOR, ImDrawFlags_RoundCornersAll, LINE_BASE_THICKNESS);

                    //dl->AddLine(ImGui::Local2Canvas(poly.vtx[i % poly.vtx.size()], avail, startPos), ImGui::Local2Canvas(poly.vtx[(i + 1) % poly.vtx.size()], avail, startPos), LINE_BASE_COLOR, LINE_BASE_THICKNESS);
                }

                // Separate loops for points and lines, to avoid Z-Clipping
                for(int i = 0; i < poly.vtx.size(); i++) {
                    char label[8];
                    sprintf(label, "P%i", i+1);
                    if(ImGui::DrawPoint(ImGui::Local2Canvas(poly.vtx[i], avail, startPos), label, dl) &&
                       ImGui::IsMouseClicked(0))
                        dragging_point = &poly.vtx[i];
                }

                if(ImGui::DrawPoint(ImGui::Local2Canvas(P1, avail, startPos),
                                    "A", dl, POINT_BASE_RADIUS, POINT_SPECIAL_COLOR) &&
                   ImGui::IsMouseClicked(0))
                    dragging_point = &P1;

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
                    dirty = true;
                }
            }
            ImGui::EndChild();


            ImGui::EndTabItem();
        }


        ImGui::EndTabBar();
    }

    return 0;
}

int main() {
    GUI::Setup(OnGui);

    srand(time(nullptr));

    while (true) {
        if(GUI::RenderFrame())
            break;
    }

    GUI::ShutDown();
}
