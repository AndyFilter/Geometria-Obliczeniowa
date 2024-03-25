#pragma once

#include <cmath>
#include <cfenv>
#include <algorithm>
#include <cstdio>
#include <vector>
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