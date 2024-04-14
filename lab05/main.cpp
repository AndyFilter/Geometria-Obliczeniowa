#include <iostream>

#include "../gui/gui.h"
#include "../gui/ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"
#include "../Helpers.h"

#define RANGE_TREE_VALUE_RANGE 100

std::vector<Vec2> random_points(16);
std::vector<float> random_values(15);

int OnGui()
{
    ImGuiStyle& style = ImGui::GetStyle();

    if(ImGui::BeginTabBar("Items")) {

        if(ImGui::BeginTabItem("1D Range Tree")) {

            //static std::vector<float> points1 {43, 12, 4, -4, 0, 1, 96, 400, 13, 312, 39, 500};
            static Vec2 points_bounds = {0, 0};
            static Vec2 SelectionBounds = {0, 0};
            //static std::vector<float> points1 {43, 12, 4, -4, 0, 1, 96};
            static RangeTree1D* rt = new RangeTree1D(random_values);

            static Vec2* dragging_point = nullptr;
            static bool dirty = true;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");
                dirty |= ImGui::SliderFloat("Lower Bound", &SelectionBounds.x, points_bounds.x, points_bounds.y);
                dirty |= ImGui::SliderFloat("Upper Bound", &SelectionBounds.y, points_bounds.x, points_bounds.y);

                //ImGui::SeparatorText("Wynik");
            }
            ImGui::EndChild();

            if(dirty) {
                //rt->head->left->right->right->left = new RangeTree1D::Node(15);
                //rt->head->left->right->right->right = new RangeTree1D::Node(16);
                points_bounds = Vec2(rt->points.front(), rt->points.back());
                rt->Select(SelectionBounds.x, SelectionBounds.y);
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

                ImGui::DrawTree1D(rt, startPos + Vec2((avail.x / 2), avail.y / 2 - 150));

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
                    dirty = true;
                }
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("2D Range Tree")) {

            static Vec2 points_bounds = {0, RANGE_TREE_VALUE_RANGE};
            static Rect selectionBounds = {{10, 10}, {40, 40}};
            //static std::vector<float> points1 {43, 12, 4, -4, 0, 1, 96};
            static RangeTree2D rt =  RangeTree2D(random_points);

            static bool show_tree = true;
            static bool show_points = false;

            static Vec2* dragging_point = nullptr;
            static bool dirty = true;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");
                dirty |= ImGui::SliderFloat2("Lower Bound", &selectionBounds.min.x, points_bounds.x, points_bounds.y);
                dirty |= ImGui::SliderFloat2("Upper Bound", &selectionBounds.max.x, points_bounds.x, points_bounds.y);

                ImGui::Checkbox("Pokaż drzewo", &show_tree);
                ImGui::Checkbox("Pokaż punkty", &show_points);

                //ImGui::SeparatorText("Wynik");
            }
            ImGui::EndChild();

            if(dirty) {
                //rt->head->left->right->right->left = new RangeTree1D::Node(15);
                //rt->head->left->right->right->right = new RangeTree1D::Node(16);
                //points_bounds = Vec2(rt->points.front(), rt->points.back());
                //selectionBounds.min.x = std::min(selectionBounds.min.x, selectionBounds.max.x);
                if(selectionBounds.min.x > selectionBounds.max.x)
                    std::swap(selectionBounds.min.x, selectionBounds.max.x);
                if(selectionBounds.min.y > selectionBounds.max.y)
                    std::swap(selectionBounds.min.y, selectionBounds.max.y);
                rt.Select(selectionBounds.min, selectionBounds.max);
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

                // Draw points making up the tree
                if(show_points)
                    for(auto& p : rt.points_x)
                        dl->AddCircleFilled(ImGui::Local2Canvas(p/5 - 10, avail, startPos), 5, selectionBounds.IsPointInside(p) ? POINT_SPECIAL_COLOR : POINT_BASE_COLOR);

                auto p1 = ImGui::Local2Canvas(selectionBounds.min/5 - 10, avail, startPos);
                auto p2 = ImGui::Local2Canvas(selectionBounds.max/5 - 10, avail, startPos);

                // Draw selection rectangle
                dl->AddRectFilled(p1, p2, SHAPE_FILL_COLOR);

                // Draw the tree itself
                if(show_tree)
                    ImGui::DrawTree2D(&rt, startPos + Vec2((avail.x / 2), 50));

                // Draw selection points
                if(ImGui::DrawPoint(p1, "", dl) && ImGui::IsMouseClicked(0))
                    dragging_point = &selectionBounds.min;

                if(ImGui::DrawPoint(p2, "", dl) && ImGui::IsMouseClicked(0))
                    dragging_point = &selectionBounds.max;

                if(dragging_point) {
                    *dragging_point = (ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos) + 10) * 5;
                    dragging_point->Clamp(0, 100);
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

    for(auto & random_point : random_points) {
        random_point = Vec2(rand() % RANGE_TREE_VALUE_RANGE, rand() % RANGE_TREE_VALUE_RANGE);
    }


    printf("random values:\n");
    for(auto & random_v : random_values) {
        random_v = rand() % RANGE_TREE_VALUE_RANGE;
        printf("%.2f, ", random_v);
    }
    printf("\n");

    GUI::SetWindowSize(1440, 940);

    while (true) {
        if(GUI::RenderFrame())
            break;
    }

    GUI::ShutDown();
}
