#include <iostream>

#include "../gui/gui.h"
#include "../gui/ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"
#include "../Helpers.h"

int OnGui()
{
    ImGuiStyle& style = ImGui::GetStyle();

    if(ImGui::BeginTabBar("Items")) {

        if(ImGui::BeginTabItem("NAZWA ZADANIA")) {

            static std::vector<float> points2 {43, 12, 4, -4, 0, 1, 96, 400, 13, 312, 39, 500};
            //static std::vector<float> points2 {43, 12, 4, -4, 0, 1, 96};
            static RangeTree1D* rt = new RangeTree1D(points2);

            static Vec2* dragging_point = nullptr;
            static bool dirty = true;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");
                ImGui::DragFloat("a", &a, 0.1f, 0, 4);
                ImGui::DragFloat("b", &b, 0.1f, 0, 100);
                ImGui::DragFloat("c", &c, 0.1f, -50, 50);

                ImGui::SeparatorText("Wynik");
            }
            ImGui::EndChild();

            if(dirty) {
                //rt->head->left->right->right->left = new RangeTree1D::Node(15);
                //rt->head->left->right->right->right = new RangeTree1D::Node(16);
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

                ImGui::DrawTree1D(rt, startPos + Vec2((avail.x / 2), 50));

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
