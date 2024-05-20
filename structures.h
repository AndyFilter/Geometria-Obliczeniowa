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
#include <unordered_set>

#define FLOAT_CMP_PRECISION 0.001f

#define clamp(v,mn,mx)  ((v < mn) ? mn : (v > mx) ? mx : v)

#ifndef M_PIf
#define M_PIf ((float)M_PI)
#endif

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

    inline const Vec2 Clamp(Vec2 mn, Vec2 mx) { x = clamp(x, mn.x, mx.x); y = clamp(y, mn.y, mx.y); return *this; };
    inline const Vec2 Clamp(float mn, float mx) { x = clamp(x, mn, mx); y = clamp(y, mn, mx); return *this; };

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
    bool operator!=(const Vec2& other) const { return x != other.x || y != other.y; }

    operator ImVec2() const { return { x,y }; }
};

inline std::istream& operator>>(std::istream& stream, Vec2 &pos) { stream >> pos.x >> pos.y; return stream;};
inline std::ostream& operator<<(std::ostream& stream, Vec2 pos) { stream << pos.x << '\t' << pos.y << '\n'; return stream;};

struct Circle;

struct Edge{
    int start = 0, end = 0;

    Edge() = default;
    Edge(int begin, int end): start(begin), end(end) {};
};

inline std::istream& operator>>(std::istream& stream, Edge &edg) { stream >> edg.start; stream >> edg.end; return stream;};

struct Rect {
    Vec2 min, max;

    Rect(Vec2 min = {0, 0}, Vec2 max = {0, 0}): min(min), max(max) {};

    bool IsPointInside(Vec2 point) const {
        return  point.x >= min.x && point.x <= max.x &&
                point.y >= min.y && point.y <= max.y;
    }

    float GetArea() {return (max.x - min.x) * (max.y - min.y);}
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

struct Triangle
{
    Vec2 vtx[3] {0};
    bool is_valid = false;

    Triangle() = default;

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

    void Scale(float scale) {
        scale -= 1;
        auto m = GetMidPos();

        vtx[0] = (vtx[0] - m) * scale + vtx[0];
        vtx[1] = (vtx[1] - m) * scale + vtx[1];
        vtx[2] = (vtx[2] - m) * scale + vtx[2];
    }
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

    Rect GetBoundingBox(int starting_idx = 0);

    PointCloud(const char* src_file, float scale = 1, Vec2 offset = {0, 0}) {
        PointCloud(src_file, {scale, scale}, offset);
    }

    PointCloud(const char* src_file, Vec2 scale = {1, 1}, Vec2 offset = {0, 0},
               const bool remove_duplicates = false) {
        using namespace std;

        fstream file(src_file);

        if(!file.good())
            return;

        size_t size = 0;
        file >> size;

        points.resize(size);

        //auto cmp_func = [](const Vec2& v1, const Vec2& v2){ return v1.x < v2.x || v1.x == v2.x && v1.y < v2.y; };
        //auto hash_func = [](const Vec2& v1){ return (size_t)(v1.x * 10000 + v1.y*10); };
        //using VecHash = std::unordered_set<Vec2, decltype(hash_func), decltype(cmp_func)>;
        //VecHash point_set = VecHash(size, hash_func, cmp_func);

        CloudPoint point;
        int idx = 0;
        while(file >> point.x && file >> point.y &&
        (!remove_duplicates || std::find(points.begin(), points.end(), point) == points.end())) {
            points[idx++] = point * scale + offset;
            //if(remove_duplicates)
            //    point_set.insert(point);
        }

        file.close();
    }

    PointCloud() = default;

    // Shoelace formula. Might be wrong
    float GetHullArea() {
        if(hull_points.empty()) return 0;
        float sum = 0;
        Vec2& last_p = points[hull_points.back()];
        for(int i : hull_points) {
            Vec2& p = points[i];
            sum += p.x * last_p.y - (last_p.x * p.y);
            last_p = p;
        }
        return sum * 0.5f;
    }

    bool PointTest(Vec2 p);

    // Jarvis
    void UpdateConvexHull_Jarvis();

    void QuickHull();

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
};


struct StructuredPolygon{
    std::vector<Vec2> points;
    std::vector<Edge> edges;

    StructuredPolygon() = default;
    StructuredPolygon(const char* pts_file_name, const char* egs_file_name, Vec2 scale = {1,1}, Vec2 offset = {0,0});
};

template<int n = 3>
struct MeshElement{
    int points[n]{0};

    MeshElement() = default;

    bool Contains(int node_idx) {
        return std::find(points, points + n, node_idx) != points + n;
    }
    bool ContainsAny(std::initializer_list<int> indices) {
        return std::find_first_of(points, points + n, indices.begin(), indices.end()) != points + n;
    }

    MeshElement(std::initializer_list<int> elems) {
        if(elems.size() != n)
            return;
        std::copy(elems.begin(), elems.end(), points);
    }
};

struct MeshStats {
    int buckets = 10;
    float mean_triangle_rating;
    float median_triangle_rating;
    float min_triangle_rating;
    float max_triangle_rating;
    std::vector<float> triangle_ratings;
    std::vector<float> rating_buckets;
};

struct TriangulationMesh {
    enum Triangulate_Method {
        Triangulate_Delaunay,
        Triangulate_DelaunayWeighted
    };

    TriangulationMesh(const char* pts_file_name, Triangulate_Method method, Vec2 scale = {1,1}, Vec2 offset = {0,0}) {
        TriangulateMesh(pts_file_name, method, scale, offset);
    }
    TriangulationMesh() = default;

    void RecalculateMesh(Triangulate_Method method);
    MeshStats GetMeshStats();

    PointCloud pc;
    std::vector<MeshElement<3>> elements;
    Triangle superTriangle;

    // For DelaunayWeighted only!
    bool use_distance = false;
    Rect bb;
    int iter_count = 1;

    void TriangulateMesh(const char* pts_file_name, Triangulate_Method method, Vec2 scale = {1,1}, Vec2 offset = {0,0});

private:
    void TriangulateDelaunay();
    void TriangulateWeighted();
    Vec2 _scale;
    Vec2 _offset;
    MeshStats _mesh_stats;
    bool _are_stats_dirty = true;
    Triangle _GenerateSuperTriangle();
    Triangle _GetTriangleFromElement(MeshElement<3> elem) {
        return {pc.points[elem.points[0]],pc.points[elem.points[1]],pc.points[elem.points[2]]};
    }
};

struct TriangleMesh {

    std::vector<Vec2> nodes;
    std::vector<MeshElement<3>> elements;

    TriangleMesh(const char* pts_file_name, const char* edg_file_name, float r = 10, Vec2 scale = {1,1}, Vec2 offset = {0,0});
    TriangleMesh(const char* pts_file_name, const char* tri_file_name, Vec2 scale = {1,1}, Vec2 offset = {0,0});
    TriangleMesh() = default;

    void Export(const char* pts_file_name, const char* elems_file_name, bool appy_reverse_transform = true, Vec2 scale = {1,1}, Vec2 offset = {0,0});

    void Recalculate(float r, int start_idx = -1);
    void StepCalculations(float r, int& start_idx);
    int idx_cap = -1;

    bool _PointTest(Vec2 p);
    bool _IsInsideMesh(Vec2 p);

    StructuredPolygon poly;
private:
    std::vector<Edge> edges;
    Vec2 A, B;
    Vec2 _scale;
    Vec2 _offset;

    int _CheckPointProximity(Vec2 C, int idx, float radius, float &distance);
    bool _CrossesFront(Vec2 p1, int pi, int idx);
    bool _IsLineOccupied(int pi, int idx);

    bool _PointInsidePolygon(Vec2 p1);
};

inline std::ostream& operator<<(std::ostream& stream, MeshElement<3> &tri) { stream << tri.points[0] << '\t' << tri.points[1] << '\t' << tri.points[2] << '\n'; return stream;};

struct RangeTree1D {
    struct Node {
        float value = 0;
        Node* left = nullptr;
        Node* right = nullptr;

        Node* prev = nullptr;

        bool is_selected = false;

        Node(float v) : value(v) {};
    } *head = nullptr;

    int height = 0;

    std::vector<float> points;

    RangeTree1D(std::initializer_list<float> _points) : RangeTree1D((float*)_points.begin(), (float*)_points.end()) {};
    RangeTree1D(std::vector<float> &_points) : RangeTree1D(_points.data(), _points.data() + _points.size()) {};
    RangeTree1D(float* beg, float* end);

    void Select(float l_bound, float h_bound);

private:
    Node* _Construct(float* beg, float* end, Node* parent);
    int _CalcHeight(Node* node, int level = 0);
    void _Unselect(Node* node);
    void _Select(Node* node, float l_bound, float h_bound);
};

struct RangeTree2D {
    struct Node {
        Vec2 value = 0;
        Node* left = nullptr;
        Node* right = nullptr;

        Node* sub_left = nullptr;
        Node* sub_right = nullptr;

        Node* prev = nullptr;

        bool is_selected = false;

        Node(Vec2 v) : value(v) {};
    } *head = nullptr;

    int height = 0;

    std::vector<Vec2> points_x;
    std::vector<Vec2> points_y;

    RangeTree2D(std::initializer_list<Vec2> _points) : RangeTree2D((Vec2*)_points.begin(), (Vec2*)_points.end()) {};
    RangeTree2D(std::vector<Vec2> &_points) : RangeTree2D(_points.data(), _points.data() + _points.size()) {};
    RangeTree2D(Vec2* beg, Vec2* end);

    void Select(Vec2 l_bound, Vec2 h_bound);

private:
    RangeTree2D::Node* _Construct(Vec2* beg, Vec2* end, Node* node);
    int _CalcHeight(Node* node, int level = 0);
    void _Unselect(Node* node);
    void _Select(Node* node, Vec2 l_bound, Vec2 h_bound);
};

template<typename Ty>
struct EulerObject {
    explicit EulerObject(Ty obj) : obj(obj) {}

    Ty obj;
    Rect bb;
    Vec2 start_pos {0, 0}; // more like an offset
    Vec2 velocity {0, 0};
    float start_time = 0;

    Vec2 GetPosAtTime(float t) {
        return start_pos + (velocity * (t - start_time));
    }
};

struct Polygon
{
    std::vector<Vec2> vtx;

    Polygon() = default;
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

    void AddVertex(Vec2 p) {
        vtx.push_back(p);
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

struct Circle {
    Vec2 pos = {0,0};
    float R = 0;

    bool ContainsPoint(Vec2 p) const {return pos.dist(p) <= R; }
    Circle(Vec2 pos, float radius) : pos(pos), R(radius) {};
    Circle() = default;
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