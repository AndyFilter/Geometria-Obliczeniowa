#include <iostream>

#include "../gui/gui.h"
#include "../External/ImGui/imgui_internal.h"
#include "../gui/ImGuiExtensions.h"
#include "../structures.h"
#include "definitions.h"

#define FLOAT_VALUE_THRESHOLD 0.05

#define sign(x) (x >= 0 ? 1 : -1)

int OnGui()
{
    auto& style = ImGui::GetStyle();

    if(ImGui::BeginTabBar("Items")) {

        if(ImGui::BeginTabItem("Równanie Prostej")) {

            static Vec2 P1{7, 0}, P2{2, -5};
            static LineFunc func(P1, P2);
            bool isDirty = false;
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

                ImGui::Text("y = %.2fx + %.2f", func.a, func.b);
            }
            ImGui::EndChild();

            if(isDirty) {
                func = LineFunc(P1, P2);
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
                Vec2 pos1 = Vec2(1, -1) * (P1 / 20) * avail + startPos + (avail/2);
                Vec2 pos2 = Vec2(1, -1) * (P2 / 20) * avail + startPos + (avail/2);

                Vec2 dir_vec = pos1 - pos2;
                Vec2 mid_point = (pos1 + pos2)/2;// startPos + (avail/2);

                dl->AddLine(pos1, pos2, ImGui::GetColorU32(ImGuiCol_Button), 2);
                dl->AddLine((dir_vec * -10000) + mid_point, (dir_vec * 10000) + mid_point, ImGui::GetColorU32(ImGuiCol_FrameBg), 1);

                ImGui::DrawPoint(pos1, "P1", dl);
                ImGui::DrawPoint(pos2, "P2", dl);
            }
            ImGui::EndChild();


            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Punkt na prostej")) {
            static Vec2 P1{6, 4};
            static LineFunc func(0.5, 1);

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

                ImGui::Text("y ="); ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2, 2});
                ImGui::DragFloat("##LineFunc_a_001", &func.a, 0.05, -10, 10, "%.2fx");
                ImGui::SameLine();
                ImGui::Text("+");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(50);
                ImGui::DragFloat("##LineFunc_b_001", &func.b, 0.05, -10, 10, "%.2f");
                ImGui::PopStyleVar();

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Dystans punktu od prostej = %.2f", P1.x * func.a + func.b - P1.y);
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

                Vec2 pos1 = Vec2(1, -1) * (P1 / 20) * avail + startPos + (avail/2);
                // Change the point's color based on whether it is on the line or not
                ImGui::DrawPoint(pos1, "P1", dl, 8,
                                 fabs(P1.x * func.a + func.b - P1.y) < FLOAT_VALUE_THRESHOLD ?
                                 ImGui::GetColorU32(ImGuiCol_PlotHistogram) :
                                 ImGui::GetColorU32(ImGuiCol_Button));

                Vec2 canvas_center = startPos + (avail/2) + Vec2(0, -func.b / 20 * avail.y);
                // y = ax + b
                Vec2 dir(1,  -func.a * avail.y / avail.x);
                dl->AddLine((dir * -1000) + canvas_center, (dir * 1000) + canvas_center, ImGui::GetColorU32(ImGuiCol_Button));
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Punkt na linii")) {
            static Vec2 P1{-3.5, 4};
            static Vec2 A {-3, -2}, B{5, 9}; // Points making up the line

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
                ImGui::Text("Dystans punktu od linii: %.2f", (P1.y - A.y) / (P1.x - A.x) - (P1.y - B.y) / (P1.x - B.x));
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

                ImColor col = fabs((P1.y - A.y) / (P1.x - A.x) - (P1.y - B.y) / (P1.x - B.x)) < FLOAT_VALUE_THRESHOLD ?
                        ImGui::GetColorU32(ImGuiCol_PlotHistogram) :
                        ImGui::GetColorU32(ImGuiCol_Button);

                Vec2 pos1 = Vec2(1, -1) * (P1 / 20) * avail + startPos + (avail/2);

                Vec2 posA = Vec2(1, -1) * (A / 20) * avail + startPos + (avail/2);
                Vec2 posB = Vec2(1, -1) * (B / 20) * avail + startPos + (avail/2);
                dl->AddLine(posA, posB, ImGui::GetColorU32(ImGuiCol_Button), 2);

                ImGui::DrawPoint(pos1, "P1", dl, 8, col);

                ImGui::DrawPoint(posA, "A", dl, 8, col);
                ImGui::DrawPoint(posB, "B", dl, 8, col);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Punkt wzgledem linii")) {
            static Vec2 P1{-3.5, 4};
            static LineFunc func(0.5, 1);

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

                ImGui::Text("y ="); ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2, 2});
                ImGui::DragFloat("##LineFunc_a_001", &func.a, 0.05, -10, 10, "%.2fx");
                ImGui::SameLine();
                ImGui::Text("+");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(50);
                ImGui::DragFloat("##LineFunc_b_001", &func.b, 0.05, -10, 10, "%.2f");
                ImGui::PopStyleVar();

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Dystans punktu od linii: %s", func.a * P1.x - P1.y + func.b < 0 ? "Prawej" : "Lewej");
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

                Vec2 pos1 = Vec2(1, -1) * (P1 / 20) * avail + startPos + (avail/2);

                Vec2 canvas_center = startPos + (avail/2) + Vec2(0, -func.b / 20 * avail.y);
                Vec2 dir(1,  -func.a * avail.y / avail.x);
                dl->AddLine((dir * -1000) + canvas_center, (dir * 1000) + canvas_center, ImGui::GetColorU32(ImGuiCol_Button));

                ImGui::DrawPoint(pos1, "P1", dl, 8, ImGui::GetColorU32(ImGuiCol_Button));
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("translacja linii o wektor")) {
            static Vec2 vec{5, 3};
            static LineFunc func(1, 2, 0);
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
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(80);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2, 2});
                ImGui::DragFloat("##LineFunc_A_001", &func.A, 0.05, -10, 10, "%.2fx");
                ImGui::Unindent(4);
                ImGui::SameLine();
                ImGui::Text("+");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::DragFloat("##LineFunc_B_001", &func.B, 0.05, -10, 10, "%.2fy");
                ImGui::SameLine();
                ImGui::Text("+");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(50);
                ImGui::DragFloat("##LineFunc_C_001", &func.C, 0.05, -10, 10, "%.2f");
                ImGui::SameLine();
                ImGui::Text("= 0");
                ImGui::PopStyleVar();

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");

                ImGui::Text("Przesuniecie: [%.2f, %.2f]", vec.x, vec.y);
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

                Vec2 free_offset = fabsf(func.B) < FLOAT_VALUE_THRESHOLD ? Vec2(-func.C / func.A / 20 * avail.x, 0) : Vec2(0, func.C / func.B / 20 * avail.y);
                Vec2 canvas_center = startPos + (avail/2) + free_offset;
                Vec2 dir = Vec2(func.B, func.A * avail.y / avail.x) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, ImGui::GetColorU32(ImGuiCol_Button), 2);

                // Translated line
                Vec2 shift_vec = Vec2(vec.x / 20 * avail.x, - vec.y / 20 * avail.y);
                dl->AddLine(dir + canvas_center + shift_vec, canvas_center - dir + shift_vec, ImGui::GetColorU32(ImGuiCol_PlotHistogram), 3);

                ImGui::DrawArrow(dl, startPos + (avail/2), startPos + (avail/2) + shift_vec, ImGui::GetColorU32(ImGuiCol_Button), 1);
                //dl->AddLine(startPos + (avail/2), startPos + (avail/2) + shift_vec, ImGui::GetColorU32(ImGuiCol_Button), 1);
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Odbicie punktu od linii")) {
            static Vec2 P1{6, 4};
            static LineFunc func(1, 3, 0);
            static Vec2 offset {0,0};

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

                ImGui::Text("Forma ogólna funkcji:");
                ImGui::Indent(4);
                ImGui::SetNextItemWidth(80);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2, 2});
                ImGui::DragFloat("##LineFunc_A_001", &func.A, 0.05, -10, 10, "%.2fx");
                ImGui::Unindent(4);
                ImGui::SameLine();
                ImGui::Text("+");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80);
                ImGui::DragFloat("##LineFunc_B_001", &func.B, 0.05, -10, 10, "%.2fy");
                ImGui::SameLine();
                ImGui::Text("+");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(50);
                ImGui::DragFloat("##LineFunc_C_001", &func.C, 0.05, -10, 10, "%.2f");
                ImGui::SameLine();
                ImGui::Text("= 0");
                ImGui::PopStyleVar();

                ImGui::SetNextItemWidth(avail.x);
                ImGui::SliderFloat2("##P2", &offset.x, -10, 10,"%.2f");

                ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::SeparatorText("Wynik");
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

                Vec2 pos1 = Vec2(1, -1) * (P1 / 20) * avail + startPos + (avail/2);
                ImGui::DrawPoint(pos1, "P", dl, 8, ImGui::GetColorU32(ImGuiCol_Button));

                // OLD METHOD. Works only for A and B != 0
                //float a = - (func.A / func.B);
                //float b = - (func.C/ func.B);
                //float intersection_x_num = P1.y + (P1.x / a) - b;
                //float intersection_x_denom = a + 1/a;

                Vec2 reflected_pos = { (func.B * func.B - (func.A * func.A)) * P1.x - 2*(func.A * func.B * P1.y) - 2 * func.A * func.C,
                                       (func.A * func.A - (func.B * func.B)) * P1.y - 2*(func.A * func.B * P1.x) - 2 * func.B * func.C};
                reflected_pos = reflected_pos * 1/(func.A * func.A + (func.B * func.B));

                Vec2 pos2 = Vec2(1, -1) * (reflected_pos / 20) * avail + startPos + (avail/2);
                ImGui::DrawPoint(pos2,"P'", dl, 8, ImGui::GetColorU32(ImGuiCol_PlotHistogram));

                Vec2 free_offset = Vec2(func.A == 0 ? 0 : (-func.C / func.A), func.B == 0 ? 0 : (func.C / func.B)) / 40 * avail.x;//offset / 20 * avail;//fabsf(func.B) < FLOAT_VALUE_THRESHOLD ? Vec2(-func.C / func.A / 20 * avail.x, 0) : Vec2(0, func.C / func.B / 20 * avail.y);
                Vec2 canvas_center = startPos + (avail/2) + free_offset;
                //ImGui::Text("Func Shift: [%f, %f]", free_offset.x / avail.x * 20, free_offset.y / avail.y * 20);
                Vec2 dir = Vec2(func.B, func.A * avail.y / avail.x) * 100000.f;
                dl->AddLine(dir + canvas_center, canvas_center - dir, ImGui::GetColorU32(ImGuiCol_Button), 2);
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
