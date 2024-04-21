#include <iostream>

#include "../gui/gui.h"
#include "../gui/ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"
#include "../Helpers.h"

#define OUT_FILE_POINTS "../Export/mesh_points.dat"
#define OUT_FILE_ELEMS "../Export/mesh_elements.dat"

int OnGui()
{
    ImGuiStyle& style = ImGui::GetStyle();
    if(ImGui::BeginTabBar("Items")) {

        if(ImGui::BeginTabItem("Twórz siatkę")) {

            static float r = 20;

            //static StructuredPolygon poly = StructuredPolygon("../trudny_wariant/sorted_points.dat", "../trudny_wariant/sorted_edges.es", {0.02, 0.02}, {-11, -10});
            //static StructuredPolygon poly = StructuredPolygon("../trudny_wariant/sorted_points_r80.dat", "../trudny_wariant/sorted_edges_r80.es", {0.02, 0.02}, {-11, -10});
            //static StructuredPolygon poly = StructuredPolygon("../trudny_wariant/mesh_points.dat", "../trudny_wariant/mesh_edges.dat", {0.02, 0.02}, {-11, -10});
            //static StructuredPolygon poly = StructuredPolygon("../trudny_wariant/sorted_points.dat", "../trudny_wariant/sorted_edges.es", {0.009, 0.009}, {0, 0});
            //static StructuredPolygon poly = StructuredPolygon("../trudny_wariant/sorted_points.dat", "../trudny_wariant/sorted_edges.es", {0.02, 0.02}, {-11, -10});
            static StructuredPolygon poly = StructuredPolygon("../wlasny_wariant/sorted_points.dat", "../wlasny_wariant/sorted_edges.es", {0.013, -0.013}, {-9, 9.5});

            //static TriangleMesh mesh = TriangleMesh("../trudny_wariant/mesh_points.dat", "../trudny_wariant/mesh_elements.dat", {0.02, 0.02}, {-11, -10});
            //static TriangleMesh mesh = TriangleMesh("../trudny_wariant/mesh_points.dat", "../trudny_wariant/mesh_elements.dat", {0.02, 0.02}, {-11, -10});
            //static TriangleMesh mesh = TriangleMesh("../trudny_wariant/mesh_points.dat", "../trudny_wariant/mesh_elements.dat", {0.009, 0.009}, {0, 0});
            //static TriangleMesh mesh = TriangleMesh("../latwy_wariant/mesh_points.dat", "../latwy_wariant/mesh_elements.dat", {0.02, 0.02}, {-11, -10});
            static TriangleMesh mesh = TriangleMesh("../wlasny_wariant/mesh_points.dat", "../wlasny_wariant/mesh_elements.dat", {0.013, -0.013}, {-9, 9.5});

            static TriangleMesh constructed_mesh = TriangleMesh("../latwy_wariant/sorted_points.dat", "../latwy_wariant/sorted_edges.es", r, {0.02, 0.02}, {-11, -10});

            static int selected_mesh = 0;

            static Vec2 p1;

            static Vec2* dragging_point = nullptr;
            static bool dirty = false;
            static bool do_full_recalc = false;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                if(ImGui::Combo("Siatka", &selected_mesh, "łatwa\0trudna\0własna\0")) {
                    switch (selected_mesh) {
                        case 0:
                            constructed_mesh = TriangleMesh("../latwy_wariant/sorted_points.dat", "../latwy_wariant/sorted_edges.es", r, {0.02, 0.02}, {-11, -10});
                            break;
                        case 1:
                            constructed_mesh = TriangleMesh("../trudny_wariant/sorted_points.dat", "../trudny_wariant/sorted_edges.es", r, {0.02, 0.02}, {-11, -10});
                            break;
                        case 2:
                            constructed_mesh = TriangleMesh("../wlasny_wariant/sorted_points.dat", "../wlasny_wariant/sorted_edges.es", r, {0.013, -0.013}, {-9, 9.5});
                            break;
                    }

                    dirty = true;
                }

                if(ImGui::DragInt("Step Cap", &constructed_mesh.idx_cap, 1, -1, 1000))
                    dirty = true;

                if(ImGui::DragFloat("R", &r, 0.2, 0, 500)){
                    dirty = true;
                    do_full_recalc = true;
                }

                ImGui::Spacing();

                if(ImGui::Button("Oblicz ponownie", {-1, 0})) {
                    constructed_mesh.Recalculate(r, 0);
                }

                if(ImGui::Button("Wyeksportuj", {-1, 0})) {
                    constructed_mesh.Export(OUT_FILE_POINTS, OUT_FILE_ELEMS, {1/0.013, 1/-0.013}, {9, -9.5});
                }

                //ImGui::SeparatorText("Wynik");
            }
            ImGui::EndChild();

            if(dirty) {

                if(!dragging_point) {
                    if(do_full_recalc)
                        constructed_mesh.Recalculate(r, 0);
                    else
                        constructed_mesh.Recalculate(r);

                    do_full_recalc = false;
                }

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

                //constructed_mesh = mesh;

                for(int i = 0; i < constructed_mesh.elements.size(); i++) {
                    auto& e = constructed_mesh.elements[i];
                    Vec2 pos1 = ImGui::Local2Canvas(constructed_mesh.nodes[e.points[0]], avail, startPos);
                    Vec2 pos2 = ImGui::Local2Canvas(constructed_mesh.nodes[e.points[1]], avail, startPos);
                    Vec2 pos3 = ImGui::Local2Canvas(constructed_mesh.nodes[e.points[2]], avail, startPos);

                    dl->AddLine(pos1, pos2, LINE_BASE_COLOR, 1);
                    dl->AddLine(pos2, pos3, LINE_BASE_COLOR, 1);
                    dl->AddLine(pos3, pos1, LINE_BASE_COLOR, 1);

                    dl->PathLineTo(pos1);
                    dl->PathLineTo(pos2);
                    dl->PathLineTo(pos3);

                    dl->PathFillConvex(SHAPE_FILL_COLOR);
                }

                for(auto& edge : constructed_mesh.poly.edges) {
                    Vec2 pos1 = ImGui::Local2Canvas(constructed_mesh.poly.points[edge.start], avail, startPos);
                    Vec2 pos2 = ImGui::Local2Canvas(constructed_mesh.poly.points[edge.end], avail, startPos);
                    dl->AddLine(pos1, pos2, LINE_SPECIAL_COLOR, 1);

                    char _buf[12];
                    sprintf(_buf, "%i", edge.start);
                    if(ImGui::DrawPoint(pos1, _buf, dl) && ImGui::IsMouseClicked(0)) {
                        dragging_point = &constructed_mesh.poly.points[edge.start];
                        do_full_recalc = true;
                    }
                }

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos).Clamp(-10, 10);
                    dirty = true;
                }
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Otwórz siatkę")) {

            static Vec2* dragging_point = nullptr;
            static bool dirty = false;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                // No result here either );
                //ImGui::SeparatorText("Wynik");
            }
            ImGui::EndChild();

            if(dirty) {
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
