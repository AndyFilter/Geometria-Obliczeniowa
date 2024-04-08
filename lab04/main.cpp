#include <iostream>
#include <cstdint>
#include <string>
#ifdef WIN32
#include <chrono>
#endif

#include "../gui/gui.h"
#include "../gui/ImGuiExtensions.h"
#include "../External/ImGui/imgui_internal.h"
#include "../Helpers.h"

#define ZAD2_SCALE_FACTOR 0.02f
#define ZAD2_OFFSET Vec2(-10, -10)

std::vector<std::vector<EulerObject<PointCloud>>> missiles(3);
std::vector<EulerObject<PointCloud>> crafts(3, EulerObject<PointCloud>(PointCloud()));

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

int OnGui()
{
    ImGuiStyle& style = ImGui::GetStyle();

    if(ImGui::BeginTabBar("Items")) {

        if(ImGui::BeginTabItem("Otoczka wypukła")) {

            static PointCloud pc("../zad1/ksztalt_2.txt", 0.1, {-5, -5});

            static Vec2* dragging_point = nullptr;
            static bool dirty = false;

            static bool first_method = true;
            static bool first_model = true;
            static int selected_model = 0;
            static char models[] = "kształt 1\0kształt 2\0kształt 3";

            static float time_taken = 0;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Text("Wybór metody:");
                ImGui::Indent(4);
                if(ImGui::RadioButton("Jarvis", first_method)){
                    first_method = true;
                    dirty = true;
                }
                if(ImGui::RadioButton("Quick Hull", !first_method)){
                    first_method = false;
                    dirty = true;
                }
                ImGui::Unindent(4);

                ImGui::Text("Wybór metody:");
                ImGui::Indent(4);

                if(ImGui::Combo("Model", &selected_model, models))
                {
                    switch (selected_model) {
                        case 0:
                            pc = PointCloud("../zad1/ksztalt_2.txt", 0.1, {-5, -5});
                            break;
                        case 1:
                            pc = PointCloud("../zad1/ksztalt_3.txt", 0.1, {-5, -5});
                            break;
                        case 2:
                            pc = PointCloud("../zad1/PointCloud.dat", 0.03, {-9});
                            break;
                    }
                }
                ImGui::Unindent(4);

                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                if(ImGui::Button("Oblicz Powłoczkę", {-1, 0}))
                    dirty = true;

                ImGui::SeparatorText("Wynik");

                ImGui::Text("Obliczanie powłoczki zajęło %.3fms", time_taken);
            }
            ImGui::EndChild();

            if(dirty) {
                auto start = micros();
                if(first_method)
                    pc.UpdateConvexHull_Jarvis();
                else
                    pc.QuickHull();
                time_taken = (micros() - start) / 1000.f;
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


                if(!pc.hull_points.empty()) {
                    Vec2 lastPos = ImGui::Local2Canvas(pc.points[pc.hull_points[pc.hull_points.size() - 1]],
                                                       avail, startPos);
                    for (int i: pc.hull_points) {
                        auto pos = ImGui::Local2Canvas(pc.points[i], avail, startPos);
                        dl->AddLine(lastPos, ImGui::Local2Canvas(pc.points[i], avail, startPos),
                                    LINE_BASE_COLOR, LINE_BASE_THICKNESS);
                        lastPos = pos;
                    }
                    //dl->AddConvexPolyFilled(reinterpret_cast<const ImVec2 *>(pc.hull_pos_points.data()), pc.hull_pos_points.size(), LINE_BASE_COLOR);
                    //dl->AddLine(startPos, avail, LINE_BASE_COLOR);
                }

                for(auto & point : pc.points) {
                    Vec2 pos = ImGui::Local2Canvas(point, avail, startPos);
                    dl->AddCircleFilled(pos, POINT_CLOUD_BASE_RADIUS,
                                        point.include_in_hull ? POINT_SPECIAL_COLOR : POINT_BASE_COLOR);
                }

                if(dragging_point) {
                    *dragging_point = ImGui::Canvas2Local(ImGui::GetMousePos(), avail, startPos);
                    dirty = true;
                }
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }


        if(ImGui::BeginTabItem("Gwiezdna Flota")) {

            //static EulerObject<PointCloud> craft(PointCloud("../zad2/craft1_ksztalt.txt", ZAD2_SCALE_FACTOR, ZAD2_OFFSET));

            static float cur_time = 0.0f;
            static float speed = 0.5;

            static bool show_bb = false;
            static bool show_hull = false;
            static int craft_idx = 1;
            static int missile_idx = 0;

            static char sz_missiles[] = "Rakiety 1\0Rakiety 2\0Rakiety 3";
            static char sz_crafts[] = "Statek 1\0Statek 2\0Statek 3";
            static Vec2* dragging_point = nullptr;
            static bool dirty = true;
            static bool is_playing = false;

            static bool is_bb_col = false;
            static bool is_hull_col = false;
            static std::set<int> colliding_points;

            if(!ImGui::IsMouseDown(0))
                dragging_point = nullptr;

            // Controls
            if(ImGui::BeginChild("Parameters", {500, -1}, ImGuiChildFlags_Border)) {

                ImGui::SeparatorText("Parametry");

                ImGui::Checkbox("Pokaż \"Bounding Box\"", &show_bb);
                ImGui::Checkbox("Pokaż otoczkę wypukłą", &show_hull);

                ImGui::Text("Rakiety:");
                if(ImGui::Combo("##Missiles_idx", &missile_idx, sz_missiles)) {
                    cur_time = 0;
                }

                ImGui::Text("Statek:");
                if(ImGui::Combo("##Craft_idx", &craft_idx, sz_crafts)) {
                    cur_time = 0;
                }
                //if(ImGui::BeginCombo("Scena", "Scena 0")) {
                //    ImGui::EndCombo();
                //}

                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

                dirty |= ImGui::SliderFloat("Czas", &cur_time, 0.0f, 2.0f, "%.2fs");

                ImGui::SliderFloat("Prędkość", &speed, -1.5f, 1.5f, "%.2f");

                if(is_playing) {
                    if (ImGui::Button("Stop", {ImGui::CalcItemWidth(), 0})) {
                        is_playing = false;
                    }
                }
                else {
                    if (ImGui::Button("Play", {ImGui::CalcItemWidth(), 0})) {
                        if (cur_time >= 2) cur_time = 0;
                        is_playing = true;
                    }
                }

                //ImGui::Spacing();
                //
                //ImGui::SeparatorText("Wynik");

            }
            ImGui::EndChild();

            if(is_playing) {
                cur_time += ImGui::GetIO().DeltaTime * speed;

                if(cur_time >= 2) {
                    cur_time = 2;
                    is_playing = false;
                }
            }

            if(dirty || is_playing) {
                //craft.obj.UpdateConvexHull_Jarvis();
                //craft.bb = craft.obj.GetBoundingBox();
                is_bb_col = false;
                is_hull_col = false;
                colliding_points.clear();

                Vec2 craft_pos = crafts[craft_idx].GetPosAtTime(cur_time);
                for(int i = 0; i < missiles[missile_idx].size(); i++) {
                    auto missile = missiles[missile_idx][i];
                    if(!is_bb_col) {
                        is_bb_col |= crafts[craft_idx].bb.IsPointInside(missile.GetPosAtTime(cur_time) - craft_pos);
                        if(is_bb_col)
                            colliding_points.insert(i);
                    }
                    if(!is_hull_col) {
                        is_hull_col |= crafts[craft_idx].obj.PointTest(missile.GetPosAtTime(cur_time) - craft_pos);
                        if(is_hull_col)
                            colliding_points.insert(i);
                    }
                }

                dirty = false;
            }

            ImGui::SameLine();

            if(ImGui::BeginChild("Canvas", {-1, -1}, ImGuiChildFlags_Border)) {
                auto dl = ImGui::GetWindowDrawList();
                Vec2 avail = (Vec2)ImGui::GetContentRegionAvail() + (Vec2)style.WindowPadding * 2;
                auto startPos = (Vec2)ImGui::GetWindowPos();

                ImGui::DrawCanvas(dl, startPos, avail);

                //avail += Vec2(-8, 0);
                //startPos += Vec2(4, 0);

                Vec2 craft_offset = crafts[craft_idx].start_pos + (crafts[craft_idx].velocity * cur_time);

                ImGui::DrawConvexHull(crafts[craft_idx].obj, dl, show_hull, 1, avail,
                                      startPos, is_hull_col ? POINT_SPECIAL_COLOR : LINE_BASE_COLOR, craft_offset);

                if(show_bb)
                    dl->AddRect(ImGui::Local2Canvas(crafts[craft_idx].bb.min + craft_offset, avail, startPos),
                                ImGui::Local2Canvas(crafts[craft_idx].bb.max + craft_offset, avail, startPos),
                                is_bb_col ? LINE_SPECIAL_COLOR : LINE_BASE_COLOR, 0, 0, LINE_BASE_THICKNESS);

                //for(auto& missile : missiles[missile_idx]) {
                for(int i = 0; i < missiles[missile_idx].size(); i++) {
                    auto missile = missiles[missile_idx][i];
                    float time_alive = cur_time - missile.start_time;
                    if(time_alive < -0.05)
                        continue;
                    auto pos = ImGui::Local2Canvas(missile.start_pos + (missile.velocity * time_alive), avail, startPos);
                    //char buff[10];
                    //sprintf(buff, "%i", i);
                    //ImGui::DrawPoint(pos, buff, dl, POINT_BASE_RADIUS,
                    //                 (colliding_points.find(i) != colliding_points.end()) ?
                    //                 POINT_SPECIAL_COLOR : POINT_BASE_COLOR);
                    dl->AddCircleFilled(pos, POINT_BASE_RADIUS,
                                        (colliding_points.find(i) != colliding_points.end()) ?
                                        POINT_SPECIAL_COLOR : POINT_BASE_COLOR);
                }

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

    micros();

    using namespace std;

    // Set up the missiles
    for(int i = 1; i <= 3; i++) {
        fstream file(string("../zad2/missiles") + to_string(i) + string(".txt"));

        if (!file.good())
            continue;

        EulerObject<PointCloud> obj = EulerObject<PointCloud>(PointCloud());
        while (file >> obj.start_time && file >> obj.start_pos && file >> obj.velocity) {
            obj.start_pos = obj.start_pos * ZAD2_SCALE_FACTOR + ZAD2_OFFSET; // transform to local coordinates
            obj.velocity = obj.velocity * ZAD2_SCALE_FACTOR; // transform to local coordinates
            //obj.start_time *= 1000; // convert to millis
            missiles[i-1].push_back(obj);
        }
    }

    PointCloud craft_pc(PointCloud("../zad2/craft1_ksztalt.txt", ZAD2_SCALE_FACTOR));
    auto bb = craft_pc.GetBoundingBox();
    craft_pc.UpdateConvexHull_Jarvis();

    // Set up the crafts
    for(int i = 1; i <= 3; i++) {
        fstream file(string("../zad2/space_craft") + to_string(i) + string(".txt"));

        if (!file.good())
            continue;

        EulerObject<PointCloud> obj = EulerObject<PointCloud>(PointCloud());
        obj.obj = craft_pc;
        obj.bb = bb;
        while (file >> obj.start_pos && file >> obj.velocity) {
            obj.start_pos = obj.start_pos * ZAD2_SCALE_FACTOR + ZAD2_OFFSET; // transform to local coordinates
            obj.velocity = obj.velocity * ZAD2_SCALE_FACTOR; // transform to local coordinates
            crafts[i-1] = obj;
        }
    }

    while (true) {
        if(GUI::RenderFrame())
            break;
    }

    GUI::ShutDown();
}
