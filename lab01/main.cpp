#include <iostream>

#include "../gui/gui.h"
#include "../External/ImGui/imgui_internal.h"
#include "../gui/ImGuiExtensions.h"
#include "../structures.h"
#include "../Helpers.h"

#define FLOAT_VALUE_THRESHOLD 0.05

#define sign(x) (x >= 0 ? 1 : -1)

int OnGui()
{
    auto& style = ImGui::GetStyle();

    if(ImGui::BeginTabBar("Items")) {

#pragma region LAB_01
        if(ImGui::BeginTabItem("Równanie Prostej")) {

            static Vec2 P1{7, 0}, P2{2, -5};
            static DirectionalLineFunc func(P1, P2);
            static bool isDirty = false;

            static Vec2* dragging_point = nullptr;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                auto avail = ImGui::GetContentRegionAvail();

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt 1:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(avail.x);
                //ImGui::DragFloat2("##P1", &P1.x, 0.05, -10, 10);
                if(ImGui::SliderFloat2("##P1", &P1.x, -10, 10,"%.2f"))
                    isDirty = true;
                ImGui::Unindent(4);

                ImGui::Text("Punkt 2:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(avail.x);
                if(ImGui::SliderFloat2("##P2", &P2.x, -10, 10,"%.2f"))
                    isDirty = true;
                ImGui::Unindent(4);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Forma kierunkowa prostej");
                ImGui::Indent(4);
                ImGui::Text("y = %.2fx + %.2f", func.a, func.b);
                ImGui::Unindent(4);

                ImGui::Spacing();

                GeneralLineFunc constructed_func(P1, P2);
                ImGui::Text("Forma ogólna prostej");
                ImGui::Indent(4);
                ImGui::Text("%.2fx + %.2fy + %.2f = 0", constructed_func.A, constructed_func.B, constructed_func.C);
                ImGui::Unindent(4);
            }
            ImGui::EndChild();

            if(isDirty) {
                func = DirectionalLineFunc(P1, P2);
                isDirty = false;
            }

            // Canvas
            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2 ;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                // Draw the grind and the canvas
                ImGui::DrawCanvas(dl, startPos, avail);

                // Canvas Padding
                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                //ImVec2 pos = {(P1.x / 20) * avail.x + startPos.x + (avail.x/2), (P1.y / 10) * avail.y + startPos.y};
                Vec2 pos1 = ImGui::Local2Canvas(P1, avail, startPos);
                Vec2 pos2 = ImGui::Local2Canvas(P2, avail, startPos);

                Vec2 dir_vec = pos1 - pos2;
                Vec2 mid_point = (pos1 + pos2)/2;// startPos + (avail/2);

                dl->AddLine((dir_vec * -10000) + mid_point, (dir_vec * 10000) + mid_point, LINE_INACTIVE_COLOR, 1);
                dl->AddLine(pos1, pos2, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                if(ImGui::DrawPoint(pos1, "P1", dl) && ImGui::IsMouseClicked(0, false))
                    dragging_point = &P1;

                if(ImGui::DrawPoint(pos2, "P2", dl) && ImGui::IsMouseClicked(0, false))
                    dragging_point = &P2;

                if(dragging_point)
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Punkt na prostej")) {
            static Vec2 P1{6, 4};
            static GeneralLineFunc func(2, 1, 3);

            // dist = |A * P1.x + B * P1.y + C| / sqrt(A^2+B^2)
            float dist = fabsf(func.A * P1.x + (func.B * P1.y) + func.C) / sqrtf(func.A * func.A + func.B * func.B);

            static Vec2* dragging_point = nullptr;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                auto avail = ImGui::GetContentRegionAvail();

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt 1:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(avail.x);
                //ImGui::DragFloat2("##P1", &P1.x, 0.05, -10, 10);
                ImGui::SliderFloat2("##P1", &P1.x, -10, 10,"%.2f");
                ImGui::Unindent(4);

                ImGui::Text("Forma ogólna prostej:");
                ImGui::GeneralLineParams(func);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Dystans punktu od prostej = %.2f", dist);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                // Change the point's color based on whether it is on the line or not
                ImGui::DrawPoint(ImGui::Local2Canvas(P1, avail, startPos), "P1", dl, POINT_BASE_RADIUS,
                                 dist < FLOAT_VALUE_THRESHOLD ?
                                 POINT_SPECIAL_COLOR :
                                 POINT_BASE_COLOR);

                if(ImGui::DrawPoint(ImGui::Local2Canvas(P1, avail, startPos),
                                    "P1", dl, POINT_BASE_RADIUS,
                                    dist < FLOAT_VALUE_THRESHOLD ?
                                    POINT_SPECIAL_COLOR :
                                    POINT_BASE_COLOR) &&
                                    ImGui::IsMouseClicked(0, false))
                    dragging_point = &P1;

                Vec2 canvas_center = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func), avail, startPos);
                // startPos + (avail/2) + ImGui::GetGeneralFuncOffset(func) * avail;
                // offset + (size/2) + (pos / (CANVAS_SIZE*2)) * size;
                //Vec2 dir = Vec2(func.A, func.B * avail.y / avail.x) * 100000.f;
                Vec2 dir = Vec2(func.B, func.A * avail.y / avail.x) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, LINE_BASE_COLOR, 2);

                if(dragging_point)
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Punkt na odcinku")) {
            static Vec2 P1{-3.5, 4};
            static Vec2 A{-3, -2}, B{5, 9}; // Points making up the line

            //LineFunc constructed_func(A.x - B.x, B.y - A.y, A.y * (B.x - A.x) - (B.y - A.y) * A.x);
            // dist = |A * P1.x + B * P1.y + C| / sqrt(A^2+B^2)
            //float func_dist = fabsf(constructed_func.A * P1.x + (constructed_func.B * P1.y) + constructed_func.C) / \
            //        sqrtf(constructed_func.A * constructed_func.A + constructed_func.B * constructed_func.B);
            Vec2 mid_point = Vec2((A.x + B.x), (A.y + B.y)) / 2;
            float mid_dist = mid_point.dist(P1);
            float point_dist = fmin(P1.dist(A), P1.dist(B));

            float len = (A - B).mag();

            float dist = fabsf((B.x - A.x) * (A.y - P1.y) - (A.x - P1.x) * (B.y - A.y)) / A.dist(B);
            if(P1.x < fmin(A.x, B.x) || P1.y < fmin(A.y, B.y) ||
            P1.x > fmax(A.x, B.x) || P1.y > fmax(A.y, B.y)) {
                if(len < fmax(P1.dist(A), P1.dist(B)))
                    dist = point_dist;

                if(mid_dist < point_dist)
                    dist = fmin(mid_dist, dist);
            }

            static Vec2* dragging_point = nullptr;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                auto avail = ImGui::GetContentRegionAvail();

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt 1:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(avail.x);
                ImGui::SliderFloat2("##P1", &P1.x, -10, 10,"%.2f");
                ImGui::Unindent(4);

                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                ImGui::Text("Punkt A:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(avail.x);
                ImGui::SliderFloat2("##PA", &A.x, -10, 10,"%.2f");
                ImGui::Unindent(4);

                ImGui::Text("Punkt B:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(avail.x);
                ImGui::SliderFloat2("##PB", &B.x, -10, 10,"%.2f");
                ImGui::Unindent(4);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                // wsp. kierunkowy (a.y - b.y) / (a.x - b.x)
                ImGui::Text("Dystans punktu od linii: %.2f", dist);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                ImColor col = dist < FLOAT_VALUE_THRESHOLD ?
                        POINT_SPECIAL_COLOR :
                        POINT_BASE_COLOR;

                Vec2 posA = ImGui::Local2Canvas(A, avail, startPos);
                Vec2 posB = ImGui::Local2Canvas(B, avail, startPos);
                dl->AddLine(posA, posB, LINE_BASE_COLOR, 2);

                if(ImGui::DrawPoint(ImGui::Local2Canvas(P1, avail, startPos),
                                    "P1", dl, POINT_BASE_RADIUS, col) &&
                                    ImGui::IsMouseClicked(0))
                    dragging_point = &P1;

                if(ImGui::DrawPoint(posA, "A", dl, POINT_BASE_RADIUS, col) && ImGui::IsMouseClicked(0))
                    dragging_point = &A;

                if(ImGui::DrawPoint(posB, "B", dl, POINT_BASE_RADIUS, col) && ImGui::IsMouseClicked(0))
                    dragging_point = &B;

                if(dragging_point)
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Punkt względom linii")) {
            static Vec2 P1{-3.5, 4};
            static DirectionalLineFunc func(0.5, 1);

            static Vec2* dragging_point = nullptr;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                auto avail = ImGui::GetContentRegionAvail();

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt 1:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(avail.x);
                ImGui::SliderFloat2("##P1", &P1.x, -10, 10,"%.2f");
                ImGui::Unindent(4);

                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                ImGui::DirectionalLineParams(func);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Punkt leży po %s stronie", func.a * P1.x - P1.y + func.b > 0 ? "Prawej" : "Lewej");
            }
            ImGui::EndChild();

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                Vec2 canvas_center = startPos + (avail/2) + Vec2(0, -func.b / 20 * avail.y);
                Vec2 dir(1,  -func.a * avail.y / avail.x);
                dl->AddLine((dir * -1000) + canvas_center, (dir * 1000) + canvas_center, LINE_BASE_COLOR);

                if(ImGui::DrawPoint(ImGui::Local2Canvas(P1, avail, startPos), "P1", dl,
                                    POINT_BASE_RADIUS, POINT_BASE_COLOR) &&
                                    ImGui::IsMouseClicked(0))
                    dragging_point = &P1;

                if(dragging_point)
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("translacja linii o wektor")) {
            static Vec2 vec{5, 3};
            static GeneralLineFunc func(1, 2, 0);
            //static LineFunc func(0.5, 1);

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                auto avail = ImGui::GetContentRegionAvail();

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Wektor translacji:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(avail.x);
                ImGui::SliderFloat2("##V1", &vec.x, -10, 10,"%.2f");
                ImGui::Unindent(4);

                ImGui::Text("Forma ogólna funkcji:");
                ImGui::GeneralLineParams(func);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Przesunięcie: [%.2f, %.2f]", vec.x, vec.y);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                Vec2 canvas_center = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func), avail, startPos);
                Vec2 dir = Vec2(func.B, func.A * avail.y / avail.x) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, LINE_BASE_COLOR, 2);

                // Translated line
                Vec2 shift_vec = ImGui::Local2Canvas(vec, avail, avail / -2);
                // Vec2(vec.x, -vec.y) / (CANVAS_SIZE*2) * avail.x;
                dl->AddLine(dir + canvas_center + shift_vec, canvas_center - dir + shift_vec, LINE_SPECIAL_COLOR, 3);

                ImGui::DrawArrow(dl, startPos + (avail/2), startPos + (avail/2) + shift_vec, LINE_BASE_COLOR, 1);
                //dl->AddLine(startPos + (avail/2), startPos + (avail/2) + shift_vec, ImGui::GetColorU32(ImGuiCol_Button), 1);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Odbicie punktu od linii")) {
            static Vec2 P1{6, 4};
            static GeneralLineFunc func(1, 3, 0);

            static Vec2* dragging_point = nullptr;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Calculate the reflected point
            Vec2 reflected_pos = { (func.B * func.B - (func.A * func.A)) * P1.x - 2*(func.A * func.B * P1.y) - 2 * func.A * func.C,
                                   (func.A * func.A - (func.B * func.B)) * P1.y - 2*(func.A * func.B * P1.x) - 2 * func.B * func.C};
            reflected_pos = reflected_pos * 1/(func.A * func.A + (func.B * func.B));

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                //auto avail = ImGui::GetContentRegionAvail();

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt 1:");
                ImGui::PointParams(P1);

                ImGui::Text("Forma ogólna funkcji:");
                ImGui::GeneralLineParams(func);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Pozycja odbitego punktu: [%.2f, %.2f]", reflected_pos.x, reflected_pos.y);
            }
            ImGui::EndChild();

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                avail += Vec2(-20, -20);
                startPos += Vec2(10, 10);

                if(ImGui::DrawPoint(ImGui::Local2Canvas(P1, avail, startPos),
                                 "P", dl, POINT_BASE_RADIUS,POINT_BASE_COLOR) && ImGui::IsMouseClicked(0))
                    dragging_point = &P1;

                ImGui::DrawPoint(ImGui::Local2Canvas(reflected_pos, avail, startPos),
                                 "P'", dl, 8, POINT_SPECIAL_COLOR);

                // Draw a function from the general form
                Vec2 canvas_center = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func), avail, startPos);
                Vec2 dir = Vec2(func.B, func.A * avail.y / avail.x) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, LINE_BASE_COLOR, 2);

                if(dragging_point)
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

#pragma endregion LAB_01

        if(ImGui::BeginTabItem(u8"Punkt przecięcia prostych"))
        {
            static GeneralLineFunc func1(2, 0.5, 1);
            static GeneralLineFunc func2(0.25, 1, 1);

            static bool dirty = true;
            static Vec2 P1;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Forma ogólna funkcji 1:");
                dirty |= ImGui::GeneralLineParams(func1);

                ImGui::Text("Forma ogólna funkcji 2:");
                dirty |= ImGui::GeneralLineParams(func2, 1);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Współrzędne punktu styku: [%f, %f]", P1.x, P1.y);
            }
            ImGui::EndChild();

            // Recalculate the collision points only when its needed
            if(dirty) {
                P1 = func1.GetCollisionPoint(func2);
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

                Vec2 canvas_center = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func1), avail, startPos);
                Vec2 dir = Vec2(func1.B, func1.A * avail.y / avail.x) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, LINE_BASE_COLOR, 2);

                canvas_center = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func2), avail, startPos);
                dir = Vec2(func2.B, func2.A * avail.y / avail.x) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, LINE_BASE_COLOR, 2);

                ImGui::DrawPoint(ImGui::Local2Canvas(P1, avail, startPos), "", dl,POINT_BASE_RADIUS, LINE_SPECIAL_COLOR);
            }
            ImGui::EndChild();


            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Punkt przecięcia odcinków"))
        {
            static Vec2 P1{-5, 7}, P2{2, -5};
            static Vec2 P3{2, 1}, P4{-5, -5};

            static GeneralLineFunc func1(P1, P2), func2(P2, P3);

            static Vec2 intersectionPoint = func1.GetCollisionPoint(func2);

            static bool dirty = true;
            static bool isReal = false;

            static Vec2* dragging_point = nullptr;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt A1:");
                dirty |= ImGui::PointParams(P1, 0);
                ImGui::Text("Punkt A2:");
                dirty |= ImGui::PointParams(P2, 1);
                ImGui::Dummy({0, style.ItemSpacing.y});
                ImGui::Text("Punkt B1:");
                dirty |= ImGui::PointParams(P3, 2);
                ImGui::Text("Punkt B2:");
                dirty |= ImGui::PointParams(P4, 3);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Współrzędne punktu styku: [%.2f, %.2f] %s", intersectionPoint.x, intersectionPoint.y, isReal ? "" : "(Projekcja)");
            }
            ImGui::EndChild();

            // Recalculate the collision points only when its needed
            if(dirty) {
                func1 = GeneralLineFunc(P1, P2);
                func2 = GeneralLineFunc(P3, P4);

                intersectionPoint = func1.GetCollisionPoint(func2);

                isReal = PointBoxCheck(P1, P2, P3, P4, intersectionPoint);

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

                Vec2 canvas_center = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func1), avail, startPos);
                Vec2 dir = ImGui::GetGeneralLineDir(func1, avail) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, LINE_INACTIVE_COLOR, 1);

                canvas_center = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func2), avail, startPos);
                dir = ImGui::GetGeneralLineDir(func2, avail) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, LINE_INACTIVE_COLOR, 1);

                Vec2 pos1 = ImGui::Local2Canvas(P1, avail, startPos);
                Vec2 pos2 = ImGui::Local2Canvas(P2, avail, startPos);
                dl->AddLine(pos1, pos2, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                Vec2 pos3 = ImGui::Local2Canvas(P3, avail, startPos);
                Vec2 pos4 = ImGui::Local2Canvas(P4, avail, startPos);
                dl->AddLine(pos3, pos4, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                // Draw Control Points
                if(ImGui::DrawPoint(pos1, "A1", dl) &&
                ImGui::IsMouseClicked(0))
                    dragging_point = &P1;

                if(ImGui::DrawPoint(pos2, "A2", dl) &&
                ImGui::IsMouseClicked(0))
                    dragging_point = &P2;

                if(ImGui::DrawPoint(pos3, "B1", dl) &&
                ImGui::IsMouseClicked(0))
                    dragging_point = &P3;

                if(ImGui::DrawPoint(pos4, "B2", dl) &&
                ImGui::IsMouseClicked(0))
                    dragging_point = &P4;

                // Draw the intersection point
                //pos1 = Vec2(1, -1) * (intersectionPoint / 20) * avail + startPos + (avail/2);
                ImGui::DrawPoint(ImGui::Local2Canvas(intersectionPoint, avail, startPos), "P", dl, POINT_BASE_RADIUS,
                                 POINT_SPECIAL_COLOR - (isReal ? 0 : 0xA0000000));

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
                    dirty = true;
                }
            }
            ImGui::EndChild();


            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Odległość punktu od linii"))
        {
            static Vec2 P1{-5, 7};
            static GeneralLineFunc func1(-2, 4, 1);

            static float dist = fabsf(func1.A * P1.x + func1.B * P1.y + func1.C) / Vec2(func1.A, func1.B).mag();

            static bool dirty = true;
            static Vec2* dragging_point = nullptr;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Punkt A1:");
                dirty |= ImGui::PointParams(P1, 0);

                ImGui::Text("Forma ogólna funkcji:");
                dirty |= ImGui::GeneralLineParams(func1);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Odległość punktu od linii to %.2f", dist);
            }
            ImGui::EndChild();

            // Recalculate the distance if needed
            if(dirty) {
                dist = fabsf(func1.A * P1.x + func1.B * P1.y + func1.C) / Vec2(func1.A, func1.B).mag();
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

                Vec2 pos1 = ImGui::Local2Canvas(P1, avail, startPos);

                GeneralLineFunc connection = LineConnectingPointAndLine(func1, P1);
                //Vec2 funcOffset1 = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(connection), avail, startPos);
                //Vec2 dir1 = ImGui::GetGeneralLineDir(connection, avail) * 100000.f;

                Vec2 pos2 = ImGui::Local2Canvas(connection.GetCollisionPoint(func1),avail, startPos);

                ImGui::DrawDistanceLine(pos1, pos2, dl, dist);

                Vec2 funcOffset = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func1), avail, startPos);
                Vec2 dir = ImGui::GetGeneralLineDir(func1, avail) * 100000.f;
                dl->AddLine(funcOffset + dir, funcOffset - dir, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                if(ImGui::DrawPoint(pos1, "P1", dl) && ImGui::IsMouseClicked(0))
                    dragging_point = &P1;

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
                    dirty = true;
                }
            }
            ImGui::EndChild();


            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Tworzenie Trójkąta"))
        {
            static GeneralLineFunc func1(-2, 4, 10), func2(3, 1, 8), func3(0, 5, -8);
            static Triangle tri(func1, func2, func3);

            static bool dirty = true;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Forma ogólna funkcji 1:");
                dirty |= ImGui::GeneralLineParams(func1);

                ImGui::Text("Forma ogólna funkcji 2:");
                dirty |= ImGui::GeneralLineParams(func2, 1);

                ImGui::Text("Forma ogólna funkcji 3:");
                dirty |= ImGui::GeneralLineParams(func3, 2);

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                for (int i = 0; i < 3; ++i) {
                    ImGui::Text("Punkt %i: [%.2f, %.2f]", i, tri.vtx[i].x, tri.vtx[i].y);
                }
            }
            ImGui::EndChild();

            // Recalculate the triangle if needed
            if(dirty) {
                tri = Triangle(func1, func2, func3);
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

                Vec2 funcOffset = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func1), avail, startPos);
                Vec2 dir = ImGui::GetGeneralLineDir(func1, avail) * 100000.f;
                dl->AddLine(funcOffset + dir, funcOffset - dir, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                funcOffset = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func2), avail, startPos);
                dir = ImGui::GetGeneralLineDir(func2, avail) * 100000.f;
                dl->AddLine(funcOffset + dir, funcOffset - dir, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                funcOffset = ImGui::Local2Canvas(ImGui::GetGeneralFuncOffset(func3), avail, startPos);
                dir = ImGui::GetGeneralLineDir(func3, avail) * 100000.f;
                dl->AddLine(funcOffset + dir, funcOffset - dir, LINE_BASE_COLOR, LINE_BASE_THICKNESS);

                if(tri.is_valid) {

                    for (const auto &i: tri.vtx) {
                        ImGui::DrawPoint(ImGui::Local2Canvas(i, avail, startPos), "", dl);
                    }

                    // Use a second loop for lines to avoid Z clipping
                    for (int i = 0; i < 3; i++) {
                        dl->AddLine(ImGui::Local2Canvas(tri.vtx[i], avail, startPos),
                                    ImGui::Local2Canvas(tri.vtx[(i + 1) % 3], avail, startPos),
                                    LINE_SPECIAL_COLOR, LINE_BASE_THICKNESS);
                    }
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

    while (true) {
        if(GUI::RenderFrame())
            break;
    }

    GUI::ShutDown();
}
