#pragma once

#include <cmath>
#include <cfenv>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <fstream>
#include <stack>
#include <set>
#include "External/ImGui/imgui.h"
#include "Helpers.h"

#define FLOAT_CMP_PRECISION 0.001f

#define clamp(v,mn,mx)  ((v < mn) ? mn : (v > mx) ? mx : v)

struct Vec2
{
public:
    float x{}, y{}; // x - horizontal, y - vertical

    Vec2(float x = 0, float y = 0) : x(x), y(y) {};
    Vec2(ImVec2 other) : x(other.x), y(other.y) {};

    inline const float dist(const Vec2& other) const { return std::sqrt(powf(x - other.x, 2) + powf(y - other.y, 2)); };
    inline const float dist(const ImVec2& other) const { return std::sqrt(powf(x - other.x, 2) + powf(y - other.y, 2)); };

    inline const float dotProduct(const Vec2& other) const { return x * other.x + (y * other.y); };

    inline float mag() const { return sqrtf(x*x + (y*y)); }
    inline Vec2 normal() const { float _mag = mag(); return {x/_mag, y/_mag}; }

    inline const void Clamp(Vec2 mn, Vec2 mx) { x = clamp(x, mn.x, mx.x); y = clamp(y, mn.y, mx.y); };

    Vec2 operator=(const Vec2& other) { return { this->x = other.x, this->y = other.y }; }
    Vec2 operator+(const Vec2& other) { return { this->x + other.x, this->y + other.y }; }
    Vec2 operator-(const Vec2& other) { return { this->x - other.x, this->y - other.y }; }
    Vec2 operator+=(const Vec2& other) { return { this->x += other.x, this->y += other.y }; }
    Vec2 operator-=(const Vec2& other) { return { this->x -= other.x, this->y -= other.y }; }
    Vec2 operator*(const float scale) { return { this->x * scale, this->y * scale }; }
    Vec2 operator*(const Vec2& other) { return { this->x * other.x, this->y * other.y }; }
    Vec2 operator/(const float scale) { return { this->x / scale, this->y / scale }; }
    Vec2 operator/(const Vec2& other) { return { this->x / other.x, this->y / other.y }; }
    Vec2 operator-(const float offset) { return { this->x - offset, this->y - offset }; }
    Vec2 operator+(const float offset) { return { this->x + offset, this->y + offset }; }
    ImVec2 operator+(const ImVec2& other) { return { this->x + other.x, this->y + other.y }; }
    ImVec2 operator-(const ImVec2& other) { return { this->x - other.x, this->y - other.y }; }
    bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }

    operator ImVec2() const { return { x,y }; }
};

struct Rect {
    Vec2 min, max;

    Rect(Vec2 min = {0, 0}, Vec2 max = {0, 0}): min(min), max(max) {};
};

struct GeneralLineFunc
{
    float A{}, B{}, C{}; // general

    // Ax + By + C = 0. (y = - (A/B)x - (C/B))
    GeneralLineFunc(float A, float B, float C) : A(A), B(B), C(C) {};
    GeneralLineFunc(const Vec2 P1, const Vec2 P2) :
    A(P2.y - P1.y),
    B(P1.x - P2.x),
    C(P1.y * (P2.x - P1.x) - (P2.y - P1.y) * P1.x) {};

    Vec2 GetCollisionPoint(GeneralLineFunc& other) const { float denominator = B*other.A - A*other.B;
        return Vec2(( C*other.B - B*other.C) / denominator, ( A*other.C - C*other.A) / denominator);
    }

    //float GetPointSize(const Vec2 p) {func.a * Pp.x - p.y + func.b };

    //float GetAngleBetween(GeneralLineFunc& other) const { return fabsf(fmodf(atan2f(other.A, other.B) - atan2f(A, B), M_PIf) ); }
    float GetAngleBetween(GeneralLineFunc& other) const { return atan2f(other.A * B - (A * other.B), A * other.A + (B * other.B)); }
};

struct DirectionalLineFunc
{
    float a{}, b{}; // directional

    explicit DirectionalLineFunc(float a = 0, float b = 0) : a(a), b(b) {};

    DirectionalLineFunc(const Vec2 p1, const Vec2 p2) {
        a = (p2.y - p1.y) / (p2.x - p1.x);
        b = p1.y - (a * p1.x);
    };
};

struct PointCloud {
    struct CloudPoint : public Vec2 {
        bool include_in_hull = false;

        CloudPoint() = default;
        CloudPoint(Vec2 pos, bool include = false) : Vec2(pos), include_in_hull(include) {}

        CloudPoint operator=(const CloudPoint& other) { return { {this->x = other.x, this->y = other.y}, other.include_in_hull }; }
    };

    std::vector<CloudPoint> points;
    std::vector<int> hull_points; // Separately store indexes of points that build the convex hull

    PointCloud(const char* src_file, float scale = 1, Vec2 offset = {0, 0}) {
        using namespace std;

        fstream file(src_file);

        if(!file.good())
            return;

        size_t size = 0;
        file >> size;

        points.resize(size);

        CloudPoint point;
        int idx = 0;
        while(file >> point.x && file >> point.y) {
            points[idx++] = point * scale + offset;
        }
    }

    // Jarvis
    void UpdateConvexHull_Jarvis() {
        hull_points.clear();

        int lowest_point_idx = 0;
        CloudPoint lowest_point_pos = points[lowest_point_idx];
        for(int i = 0; i < points.size(); i++) {
            points[i].include_in_hull = false;
            if(points[i].x <= lowest_point_pos.x) {
                if(points[i].x == lowest_point_pos.x && points[i].y < lowest_point_pos.y) {
                    continue;
                }
                lowest_point_pos = points[i];
                lowest_point_idx = i;
            }
        }

        int best_idx = lowest_point_idx;
        GeneralLineFunc line(0,0,0);
        int last_best;
        //int count = 0;
        do {
            points[best_idx].include_in_hull = true;
            hull_points.push_back(best_idx);

            last_best = best_idx;
            best_idx = 0;
            line = GeneralLineFunc(points[best_idx], points[last_best]);
            for(int i = 0; i < points.size(); i++) {
                //if(GetPointsOrientation(points[last_best], points[i], points[best_idx]) > 0) {
                if(GetOrientationOfPointsAlongLine(line, points[i]) > 0) {
                    line = GeneralLineFunc(points[last_best], points[i]);
                    best_idx = i;
                }
            }
            //count++;
        } while(best_idx != lowest_point_idx);
    }

    void QuickHull() {
        hull_points.clear();

        int min_x = 0, max_x = 0, min_y = 0;
        for (int i=1; i < points.size(); i++)
        {
            if (points[i].x < points[min_x].x)
                min_x = i;
            if (points[i].x > points[max_x].x)
                max_x = i;
            if(points[i].y < points[min_y].y)
                min_y = i;
        }

        QuickHull_rec(points.size(), min_x, max_x, 1);
        QuickHull_rec(points.size(), min_x, max_x, -1);

        Vec2 start = hull_points[0];

        min_x = 0, min_y = 0;
        Vec2 center;
        for(int i : hull_points)
        {
            if (points[i].x < points[min_x].x)
                min_x = i;
            if(points[i].y < points[min_y].y)
                min_y = i;

            center += points[i];
        }

        center = center / hull_points.size();
        start = {(float)min_x, (float)min_y};
        //p0 = lowest_point_pos;

        //qsort(hull_points.data(), hull_points.size(), angle_compare);
        std::sort(hull_points.begin(), hull_points.end(), [center, this](auto A, auto B)
        {
            Vec2 a = points[A], b = points[B];
            if (a.x - center.x >= 0 && b.x - center.x < 0)
                return true;
            if (a.x - center.x < 0 && b.x - center.x >= 0)
                return false;
            if (a.x - center.x == 0 && b.x - center.x == 0) {
                if (a.y - center.y >= 0 || b.y - center.y >= 0)
                    return a.y > b.y;
                return b.y > a.y;
            }

            // compute the cross product of vectors (center -> a) x (center -> b)
            float det = (a.x - center.x) * (b.y - center.y) - (b.x - center.x) * (a.y - center.y);
            if (det < 0)
                return true;
            if (det > 0)
                return false;

            // points a and b are on the same line from the center
            // check which point is closer to the center
            float d1 = (a.x - center.x) * (a.x - center.x) + (a.y - center.y) * (a.y - center.y);
            float d2 = (b.x - center.x) * (b.x - center.x) + (b.y - center.y) * (b.y - center.y);
            return d1 > d2;
        });
    }

    /*
    void UpdateConvexHull_Monotone() {
        hull_points.clear();

        for (auto & point : points)
            point.include_in_hull = false;

        hull_points.resize(points.size() * 2);

        std::sort(points.begin(), points.end(),
                  [](auto p1, auto p2)
                  { return (p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y)); });

        int k = 0;
        // Build lower hull
        for (int i = 0; i < points.size(); ++i) {

            // If the point at K-1 position is not a part
            // of hull as vector from ans[k-2] to ans[k-1]
            // and ans[k-2] to A[i] has a clockwise turn
            while (k >= 2 && orientation(hull_points[k - 2], hull_points[k - 1], points[i]) <= 0)
                k--;

            hull_points[k++] = i;
            points[i].include_in_hull = true;
        }

        // Build upper hull
        for (size_t i = points.size() - 1, t = k + 1; i > 0; --i) {

            // If the point at K-1 position is not a part
            // of hull as vector from ans[k-2] to ans[k-1]
            // and ans[k-2] to A[i] has a clockwise turn
            while (k >= t && GetPointsOrientation(hull_points[k - 2], hull_points[k - 1], points[i - 1]) <= 0)
                k--;

            hull_points[k++] = i - 1;
            points[i - 1].include_in_hull = true;
        }

        hull_points.resize(k-1);
    }
     */

    //void UpdateConvexHull_Graham(){
    //    hull_points.clear();
    //
    //    int lowest_point_idx = 0;
    //    CloudPoint lowest_point_pos = points[lowest_point_idx];
    //    for(int i = 0; i < points.size(); i++) {
    //        points[i].include_in_hull = false;
    //        if(points[i].x <= lowest_point_pos.x) {
    //            if(points[i].x == lowest_point_pos.x && points[i].y < lowest_point_pos.y) {
    //                continue;
    //            }
    //            lowest_point_pos = points[i];
    //            lowest_point_idx = i;
    //        }
    //    }
    //
    //    std::swap(points[0], points[lowest_point_idx]);
    //
    //    // Sort vertices based on polar angle
    //    std::sort(points.begin() + 1, points.end(), [lowest_point_pos](CloudPoint a, CloudPoint b)
    //    {
    //        CloudPoint A = a - lowest_point_pos, B = b - lowest_point_pos;
    //        float angle1 = atan2f(A.y, A.x);
    //        float angle2 = atan2f(B.y, B.x);
    //
    //        //if(angle1 == angle2) {
    //        //    float dist1 = lowest_point_pos.dist(a);
    //        //    float dist2 = lowest_point_pos.dist(b);
    //        //
    //        //    return dist1 < dist2;
    //        //}
    //        //else
    //        return GetPointsOrientation(lowest_point_pos, a, b) > 0;
    //    });
    //
    //    //std::vector<CloudPoint> pts;
    //    //CloudPoint _last_cp = points[0];
    //    //pts[0] = _last_cp;
    //    //std::copy_if(points.begin() + 1, points.end(), pts.begin() + 1, [&_last_cp](CloudPoint a) {
    //    //    return true;
    //    //});
    //
    //    std::stack<int> stack;
    //    stack.push(0);
    //    stack.push(1);
    //    stack.push(2);
    //
    //    for(int i = 1; i < points.size(); i++) {
    //        int point1 = stack.top(); stack.pop();
    //        int point2 = stack.top(); stack.push(point1);
    //        while(stack.size() > 1 && GetPointsOrientation(points[point2], points[point1], points[i]) != 2) {
    //            stack.pop();
    //        }
    //        stack.push(i);
    //    }
    //
    //    hull_points.resize(stack.size());
    //    int idx = 0;
    //    while (!stack.empty()) {
    //        hull_points[idx++] = stack.top();
    //        stack.pop();
    //    }
    //}

private:

    void QuickHull_rec(int n, int p1, int p2, int side)
    {
        int ind = -1;
        float max_dist = 0;

        for (int i=0; i<n; i++)
        {
            float temp = fabsf(GetPointsOrientation(points[p1], points[p2], points[i]));
            if (sgn(GetPointsOrientation(points[p1], points[p2], points[i])) == side && temp > max_dist)
            {
                ind = i;
                max_dist = temp;
            }
        }

        if (ind == -1)
        {
            hull_points.push_back(p1);
            hull_points.push_back(p2);
            points[p1].include_in_hull = points[p2].include_in_hull = true;
            return;
        }

        // Recur for the two parts divided by a[ind]
        QuickHull_rec(n, ind, p1, -sgn(GetPointsOrientation(points[ind], points[p1], points[p2])));
        QuickHull_rec(n, ind, p2, -sgn(GetPointsOrientation(points[ind], points[p2], points[p1])));
    }

    /*
    void UpdateConvexHull_Graham(){
        hull_points.clear();

        int lowest_point_idx = 0;
        CloudPoint lowest_point_pos = points[lowest_point_idx];
        for(int i = 0; i < points.size(); i++) {
            points[i].include_in_hull = false;
            if(points[i].x <= lowest_point_pos.x) {
                if(points[i].x == lowest_point_pos.x && points[i].y < lowest_point_pos.y) {
                    continue;
                }
                lowest_point_pos = points[i];
                lowest_point_idx = i;
            }
        }

        std::swap(points[0], points[lowest_point_idx]);

        // Sort vertices based on polar angle
        std::sort(points.begin() + 1, points.end(), [lowest_point_pos](CloudPoint a, CloudPoint b)
        {
            CloudPoint A = a - lowest_point_pos, B = b - lowest_point_pos;
            float angle1 = atan2f(A.y, A.x);
            float angle2 = atan2f(B.y, B.x);

            //if(angle1 == angle2) {
            //    float dist1 = lowest_point_pos.dist(a);
            //    float dist2 = lowest_point_pos.dist(b);
            //
            //    return dist1 < dist2;
            //}
            //else
            return orientation_glob(lowest_point_pos, a, b) == 2;
        });

        //std::vector<CloudPoint> pts;
        //CloudPoint _last_cp = points[0];
        //pts[0] = _last_cp;
        //std::copy_if(points.begin() + 1, points.end(), pts.begin() + 1, [&_last_cp](CloudPoint a) {
        //    return true;
        //});

        std::stack<int> stack;
        stack.push(0);
        stack.push(1);
        stack.push(2);

        for(int i = 1; i < points.size(); i++) {
            int point1 = stack.top(); stack.pop();
            int point2 = stack.top(); stack.push(point1);
            while(stack.size() > 1 && GetPointsOrientation(points[point2], points[point1], points[i]) != 2) {
                stack.pop();
            }
            stack.push(i);
        }

        hull_points.resize(stack.size());
        int idx = 0;
        while (!stack.empty()) {
            hull_points[idx++] = stack.top();
            stack.pop();
        }
    }
     */
};


struct Polygon
{
    std::vector<Vec2> vtx;

    Polygon(std::initializer_list<Vec2> vertices) {
        vtx.resize(vertices.size());
        std::copy(vertices.begin(), vertices.end(), vtx.begin());
    }

    // Adds a vertex between first and the last point (on a line)
    void AddVertex() {
        if(vtx.size() < 3) {
            vtx.push_back({0, 0});
            return;
        }

        auto start_idx = vtx.size() - 1;
        Vec2 mid_point = (vtx[start_idx] + vtx[0]) / 2;
        vtx.push_back(mid_point);
    }

    bool PointTest(Vec2 p) {
        Vec2 lastPoint = vtx[vtx.size() - 1];

        int left = 0;

        GeneralLineFunc horizontal_line{0, 1, -p.y};

        for (int i = 0; i < vtx.size(); ++i) {
            Vec2 point = vtx[i];

            if(IsY_OnAABB(point, lastPoint, p.y)) {
                auto func = GeneralLineFunc(point, lastPoint);
                //printf("func: %.2fx + %.2fy + %.2f = 0\n", func.A, func.B, func.C);
                auto intersect = GeneralLineFunc(point, lastPoint).GetCollisionPoint(horizontal_line);

                //printf("intersect at [%.2f, %.2f]", intersect.x, intersect.y);

                // Left side
                if(intersect.x <= p.x)
                {
                    //printf(" (LEFT)\n");
                    if(fminf(point.y, lastPoint.y) < p.y && fmaxf(point.y, lastPoint.y)  >= p.y)
                        left++;
                }
            }

            lastPoint = point;
        }

        return left % 2 == 1;
    }
};

struct Triangle
{
    Vec2 vtx[3] {0};
    bool is_valid = false;

    Triangle(GeneralLineFunc f1, GeneralLineFunc f2, GeneralLineFunc f3) {
        feclearexcept(FE_ALL_EXCEPT);

        vtx[0] = f1.GetCollisionPoint(f2);
        vtx[1] = f1.GetCollisionPoint(f3);
        vtx[2] = f2.GetCollisionPoint(f3);

        // If there was a FE_DIVBYZERO (or FE_INVALID) exception the lines don't create a triangle
        is_valid = !fetestexcept(FE_DIVBYZERO);
    }

    Triangle(Vec2 p1, Vec2 p2, Vec2 p3) {
        vtx[0] = p1;
        vtx[1] = p2;
        vtx[2] = p3;
    };

    float GetArea() {
        float a = vtx[0].dist(vtx[1]), b = vtx[1].dist(vtx[2]), c = vtx[2].dist(vtx[0]);
        float p = (a+b+c) * 0.5f;
        return sqrtf(p * (p-a) * (p-b) * (p-c));
    }

    Rect GetRect() {
        Rect bb;
        bb.min.x = std::min_element(vtx, vtx + 3, [](auto a, auto b) {return a.x < b.x;})->x;
        bb.max.x = std::max_element(vtx, vtx + 3, [](auto a, auto b) {return a.x < b.x;})->x;
        bb.min.y = std::min_element(vtx, vtx + 3, [](auto a, auto b) {return a.y < b.y;})->y;
        bb.max.y = std::max_element(vtx, vtx + 3, [](auto a, auto b) {return a.y < b.y;})->y;

        return bb;
    }

    Vec2 GetMidPos() {
        return (vtx[0] + vtx[1] + vtx[2]) / 3;
    }

    inline bool IsContained_Area(Vec2 point) {
        float S = GetArea();
        float S1 = Triangle(vtx[0], vtx[1], point).GetArea();
        float S2 = Triangle(vtx[1], vtx[2], point).GetArea();
        float S3 = Triangle(vtx[2], vtx[0], point).GetArea();
        bool is = fabsf(S - (S1 + S2 + S3)) < FLOAT_CMP_PRECISION;
        return is;
    }

    inline bool IsContained_Angles(Vec2 point) {
        float sum = 0;
        for(int i = 0; i < 3; i++) {
            sum += AngleBetweenPoints(vtx[i], point, vtx[(i+1) % 3]);
        }
        return fabsf(sum - M_PIf * 2) < FLOAT_CMP_PRECISION;
    }
};

struct LineFunc
{
    float a{}, b{}; // directional
    float A{}, B{}, C{}; // general

    // y = ax + b
    LineFunc(float a = 0, float b = 0) : a(a), b(b), B(a), A(1), C(b) {};

    // Ax + By + C = 0. (y = - (A/B)x - (C/B))
    LineFunc(float A, float B, float C) : B(A), A(B), C(C), a(-B / A), b(-C / A) {};
    LineFunc(const Vec2 p1, const Vec2 p2) {
        a = (p2.y - p1.y) / (p2.x - p1.x);
        b = p1.y - (a * p1.x);
    };

    //float eval(float x) {}
};