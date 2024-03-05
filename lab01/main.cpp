#include <iostream>

#include "../gui/gui.h"
#include "../External/ImGui/imgui_internal.h"
#include "../gui/ImGuiExtensions.h"
#include "../structures.h"
#include "definitions.h"

#define FLOAT_VALUE_THRESHOLD 0.05

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
