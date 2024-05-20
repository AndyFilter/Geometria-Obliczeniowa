#include <iostream>
#include <cstdint>
#ifdef WIN32
#include <chrono>
#endif

#include "../gui/gui.h"
#include "../gui/ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"
#include "../Helpers.h"

uint64_t micros()
{
#ifdef _WIN32
    auto now = std::chrono::steady_clock::now();
    static auto start = now;
    return std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
#else
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    uint64_t us = ((uint64_t)ts.tv_sec * 1000000) + ((uint64_t)ts.tv_nsec / 1000);
    static uint64_t start_time = us;
    return us - start_time;
#endif
}

char* meshes[] = {"../points.dat", "../Export/mesh_points_1.dat", "../Export/mesh_points_2.dat", "../Export/mesh_points_3.dat", "../Export/mesh_points_4.dat"};

int OnGui()
{
    ImGuiStyle& style = ImGui::GetStyle();
    if(ImGui::BeginTabBar("Items")) {

        //if(ImGui::BeginTabItem("Okrąg opisany")) {
        //
        //    static Triangle tri(Vec2{-3, 4}, {3, 8}, {1, -4});
        //
        //    static Vec2* dragging_point = nullptr;
        //    static bool dirty = false;
        //
        //    if(!ImGui::IsMouseDown(0))
        //        dragging_point = nullptr;
        //
        //    // Controls
        //    if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {
        //
        //        ImGui::SeparatorText("Parametry");
        //
        //        ImGui::SeparatorText("Wynik");
        //    }
        //    ImGui::EndChild();
        //
        //    if(dirty) {
        //        dirty = false;
        //    }
        //
        //    ImGui::SameLine();
        //
        //    if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
        //        auto dl = ImGui::GetWindowDrawList();
        //        Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
        //        auto startPos = (Vec2)ImGui::GetWindowPos();
        //
        //        ImGui::DrawCanvas(dl, startPos, avail);
        //
        //        avail += Vec2(-20, -20);
        //        startPos += Vec2(10, 10);
        //
        //        for(auto& p : tri.vtx) {
        //            if(ImGui::DrawPoint(ImGui::Local2Canvas(p, avail, startPos), "", dl) && ImGui::IsMouseClicked(0))
        //                dragging_point = &p;
        //        }
        //
        //        for(auto& p : tri.vtx)
        //            dl->PathLineTo(ImGui::Local2Canvas(p, avail, startPos));
        //        dl->PathFillConvex(SHAPE_FILL_COLOR);
        //
        //        Circle c = GetTriangleCircumscribedCircle(tri);
        //        dl->AddEllipse(ImGui::Local2Canvas(c.pos, avail, startPos), c.R * (avail.x / 20), c.R * (avail.y / 20), LINE_BASE_COLOR);
        //
        //        if(dragging_point) {
        //            *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
        //            dirty = true;
        //        }
        //    }
        //    ImGui::EndChild();
        //
        //    ImGui::EndTabItem();
        //}


        if(ImGui::BeginTabItem("Delaunay")) {

            static TriangulationMesh::Triangulate_Method method = TriangulationMesh::Triangulate_Delaunay;
            static int selected_mesh = 0;
            static TriangulationMesh mesh(meshes[selected_mesh], method);
            static bool show_rating = false;

            static float time_taken = 0;

            static Vec2* dragging_point = nullptr;
            static bool dirty = true;
            static bool full_rebuild = false;

            static bool real_time = false;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                if(ImGui::Combo("Metoda", (int*)&method, "Delaunay\0Delaunay + Wygładzanie\0")) {
                    dirty = true;
                    full_rebuild = true;
                }

                if(ImGui::Combo("Zbiór punktów", &selected_mesh, meshes, sizeof(meshes) / sizeof(char*))) {
                    full_rebuild = true;
                }

                ImGui::BeginDisabled(method == TriangulationMesh::Triangulate_Delaunay);
                if(ImGui::Checkbox("Używaj dystansu", &mesh.use_distance))
                    full_rebuild = true;
                ImGui::EndDisabled();

                if(ImGui::Button("Oblicz ponownie"))
                    dirty = true;
                    //mesh.RecalculateMesh(method);
                if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(0, true) && !ImGui::IsItemClicked(0))
                    dirty = true;

                ImGui::Checkbox("Pokaż Rating", &show_rating);

                ImGui::Checkbox("Realtime", &real_time);

                ImGui::SeparatorText("Wynik");

                auto stats = mesh.GetMeshStats();

                ImGui::Text("Czas obliczeń: %.2fms", time_taken);
                ImGui::Text("Liczba trójkątów: %zu", mesh.elements.size());
                ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.2f));
                ImGui::SeparatorText("Rating trójkątów");
                ImGui::PopStyleColor();
                ImGui::PlotHistogram("##TriangleRatingHistogram", stats.rating_buckets.data(),
                                     stats.buckets, 0, nullptr,
                                     0, FLT_MAX, {-1, 100});
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::Text("Min: %.3f\t Średnia: %.3f\t Mediana: %.3f\t Max: %.3f",
                            stats.min_triangle_rating, stats.mean_triangle_rating,
                            stats.median_triangle_rating, stats.max_triangle_rating);
            }
            ImGui::EndChild();

            if(dirty || full_rebuild) {

                if(!dragging_point || real_time) {
                    if(full_rebuild)
                        mesh.TriangulateMesh(meshes[selected_mesh], method);
                    auto start = micros();
                    mesh.RecalculateMesh(method);
                    time_taken = (micros() - start) / 1000.f;
                }

                full_rebuild = false;

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

                //for(auto& p : mesh.superTriangle.vtx) {
                //    dl->PathLineTo(ImGui::Local2Canvas(p, avail, startPos));
                //}
                //dl->PathFillConvex(SHAPE_FILL_COLOR);

                for(int i = 0; i < mesh.elements.size(); i++) {
                    auto& e = mesh.elements[i];
                    Vec2 pos1 = ImGui::Local2Canvas(mesh.pc.points[e.points[0]], avail, startPos);
                    Vec2 pos2 = ImGui::Local2Canvas(mesh.pc.points[e.points[1]], avail, startPos);
                    Vec2 pos3 = ImGui::Local2Canvas(mesh.pc.points[e.points[2]], avail, startPos);

                    if(!show_rating) {
                        dl->AddLine(pos1, pos2, LINE_BASE_COLOR, 1);
                        dl->AddLine(pos2, pos3, LINE_BASE_COLOR, 1);
                        dl->AddLine(pos3, pos1, LINE_BASE_COLOR, 1);
                    }

                    //Vec2 m = (pos1 + pos2 + pos3) / 3;
                    //char _buf[14];
                    //sprintf(_buf, "%.2f", mesh.GetMeshStats().triangle_ratings[i]);
                    //dl->AddText(m, LINE_SPECIAL_COLOR, _buf);

                    dl->PathLineTo(pos1);
                    dl->PathLineTo(pos2);
                    dl->PathLineTo(pos3);

                    //dl->PathFillConvex(ImColor::HSV((60 + (mesh.GetMeshStats().triangle_ratings[i] * 40))/255, 100/255.f, 87/255.f));
                    //dl->PathFillConvex(ImLerp(ImColor(255, 0, 0, 255), ImColor(0, 255, 0, 255), mesh.GetMeshStats().triangle_ratings[i]));
                    //dl->PathFillConvex(ImColor(mesh.GetMeshStats().triangle_ratings[i], 0.f, 0.f, 0.5f));
                    if(show_rating)
                        //dl->PathFillConvex(ImColor(mesh.GetMeshStats().triangle_ratings[i], 0.f, 0.f, 0.5f));
                        dl->PathFillConvex(ImColor::HSV(0/360.f, 45/100.f, mesh.GetMeshStats().triangle_ratings[i]));
                    else
                        dl->PathFillConvex(SHAPE_FILL_COLOR);
                }

                // Skip first 3 points (super triangle)
                if(!show_rating)
                    for(int i = 2; i < mesh.pc.points.size(); i++) {
                        Vec2& p = mesh.pc.points[i];
                        auto pos = ImGui::Local2Canvas(p, avail, startPos);
                        //char _buf[12];
                        //sprintf(_buf, "%i", i);
                        if(ImGui::DrawPoint(pos, "", dl, POINT_BASE_RADIUS - 2) && ImGui::IsMouseClicked(0))
                            dragging_point = &p;
                        //dl->AddCircleFilled(pos, POINT_BASE_RADIUS, POINT_BASE_COLOR);
                    }

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos).Clamp(-10, 10);
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