#pragma once

#include <cmath>
#include "External/ImGui/imgui.h"


#define clamp(v,mn,mx)  ((v < mn) ? mn : (v > mx) ? mx : v)

struct Vec2
{
public:
    float x{}, y{}; // x - horizontal, y - vertical

    Vec2(float x = 0, float y = 0) : x(x), y(y) {};
    Vec2(ImVec2 other) : x(other.x), y(other.y) {};

    inline const float dist(const Vec2& other) const { return std::sqrt(powf(x - other.x, 2) + powf(y - other.y, 2)); };
    inline const float dist(const ImVec2& other) const { return std::sqrt(powf(x - other.x, 2) + powf(y - other.y, 2)); };

    inline float mag() const { return sqrtf(x*x + (y*y)); }
    inline Vec2 normal() const { float _mag = mag(); return {x/_mag, y/_mag}; }

    inline const void Clamp(Vec2 mn, Vec2 mx) { x = clamp(x, mn.x, mx.x); y = clamp(y, mn.y, mx.y); };

    Vec2 operator=(const Vec2& other) { return { this->x = other.x, this->y = other.y }; }
    Vec2 operator+(const Vec2& other) { return { this->x + other.x, this->y + other.y }; }
    Vec2 operator-(const Vec2& other) { return { this->x - other.x, this->y - other.y }; }
    Vec2 operator+=(const Vec2& other) { return { this->x += other.x, this->y += other.y }; }
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

struct GeneralLineFunc
{
    float A{}, B{}, C{}; // general

    // Ax + By + C = 0. (y = - (A/B)x - (C/B))
    GeneralLineFunc(float A, float B, float C) : A(A), B(B), C(C) {};
    GeneralLineFunc(const Vec2 P1, const Vec2 P2) :
    A(P2.y - P1.y),
    B(P1.x - P2.x),
    C(P1.y * (P2.x - P1.x) - (P2.y - P1.y) * P1.x) {};

    Vec2 GetCollisionPoint(GeneralLineFunc& other) { float denominator = B*other.A - A*other.B;
        return Vec2(( C*other.B - B*other.C) / denominator, ( A*other.C - C*other.A) / denominator); }
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

struct Triangle
{
    Vec2 vtx[3] {0};

    Triangle(GeneralLineFunc f1, GeneralLineFunc f2, GeneralLineFunc f3) {
        vtx[0] = f1.GetCollisionPoint(f2);
        vtx[1] = f1.GetCollisionPoint(f3);
        vtx[2] = f2.GetCollisionPoint(f3);
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