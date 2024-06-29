#include <unordered_set>
#include <unordered_map>
#include <map>

#include "structures.h"

bool PointCloud::PointTest(Vec2 p) {
    Vec2 lastPoint = points[hull_points[hull_points.size() - 1]];

    int left = 0;

    GeneralLineFunc horizontal_line{0, 1, -p.y};

    //for (int i = 0; i < hull_points.size(); ++i) {
    for(int i : hull_points) {
        Vec2 point = points[i];

        if(IsY_OnAABB(point, lastPoint, p.y)) {
            auto intersect = GeneralLineFunc(point, lastPoint).GetCollisionPoint(horizontal_line);

            // Left side
            if(intersect.x <= p.x)
            {
                if(fminf(point.y, lastPoint.y) < p.y && fmaxf(point.y, lastPoint.y)  >= p.y)
                    left++;
            }
        }

        lastPoint = point;
    }

    return left % 2 == 1;
}

void PointCloud::UpdateConvexHull_Jarvis() {
    if(points.size() < 3)
        return;

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
        best_idx = (best_idx + 1) % points.size();
        //line = GeneralLineFunc(points[best_idx], points[last_best]);
        for(int i = 0; i < points.size(); i++) {
            if(GetPointsOrientation(points[last_best], points[i], points[best_idx]) > 0) {
                //if(GetDistanceFromPointToLine(line, points[i]) > 0) {
                //line = GeneralLineFunc(points[last_best], points[i]);
                best_idx = i;
            }
        }
        //count++;
    } while(best_idx != lowest_point_idx);
}

void PointCloud::QuickHull() {
    if(points.size() < 3)
        return;

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


Rect PointCloud::GetBoundingBox(int starting_idx) {
    Rect res { {FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX}};
    for(int i = starting_idx; i < points.size(); i++) {
        Vec2& p = points[i];
        res.min.x = std::fmin(res.min.x, p.x);
        res.min.y = std::fmin(res.min.y, p.y);
        res.max.x = std::fmax(res.max.x, p.x);
        res.max.y = std::fmax(res.max.y, p.y);
    }
    return res;
}

RangeTree1D::RangeTree1D(float *beg, float *end) {
    auto n = (end - beg);
    points.resize(n);
    std::copy(beg, end, points.data());

    std::sort(points.begin(), points.end());
    beg = points.data();
    end = points.data() + n;

    head = new Node(*(beg + (n - 1) / 2));

    head->left = _Construct(beg, beg + ((n + 1) / 2), head);
    head->right = _Construct(beg + ((n + 1) / 2), end, head);

    height = _CalcHeight(head);
}

void RangeTree1D::Select(float l_bound, float h_bound) {
    _Unselect(head);

    _Select(head, l_bound, h_bound);
}

RangeTree1D::Node* RangeTree1D::_Construct(float *beg, float *end, Node* parent) {
    auto n = end - beg;
    Node* node;
    if(n == 1) {
        node = new Node(*beg);
        node->prev = parent;
        return node;
    }
    float *mid_point = beg + ((n - 1) / 2);
    //printf("for mid = %f\n", *mid_point);
    node = new Node(*mid_point);

    Node* left = _Construct(beg, beg + ((n + 1) / 2), node);
    Node* right = _Construct(beg + ((n + 1) / 2), end, node);

    node->left = left;
    node->right = right;
    node->prev = parent;

    return node;
}

int RangeTree1D::_CalcHeight(RangeTree1D::Node *node, int level) {
    if(!node)
        return level;

    return std::max(_CalcHeight(node->left, level + 1), _CalcHeight(node->right, level + 1));
}

void RangeTree1D::_Select(RangeTree1D::Node *node, float l_bound, float h_bound) {
    if(!node) return;

    if(node->value >= l_bound && node->value <= h_bound) {
        node->is_selected = true;
    }

    _Select(node->left, l_bound, h_bound);
    _Select(node->right, l_bound, h_bound);
}

void RangeTree1D::_Unselect(RangeTree1D::Node *node) {
    if(!node)
        return;

    node->is_selected = false;

    _Unselect(node->left);
    _Unselect(node->right);
}

//RangeTree2D::RangeTree2D(Vec2* beg, Vec2* end) {
//    points.resize((end - beg));
//    std::copy(beg, end, points.data());
//
//    std::sort(points.begin(), points.end());
//}
RangeTree2D::RangeTree2D(Vec2 *beg, Vec2 *end) {
    auto n = (end - beg);
    points_x.resize(n);
    points_y.resize(n);
    std::copy(beg, end, points_x.data());
    std::copy(beg, end, points_y.data());

    std::sort(points_x.begin(), points_x.end(), [](Vec2& a, Vec2& b) {return a.x < b.x;});
    std::sort(points_y.begin(), points_y.end(), [](Vec2& a, Vec2& b) {return a.y < b.y;});
    beg = points_x.data();
    end = points_x.data() + n;

    head = _Construct(beg, end, nullptr);//new Node(*(beg + (n - 1) / 2));

    //head->left = _Construct(beg, beg + ((n + 1) / 2), head);
    //head->right = _Construct(beg + ((n + 1) / 2), end, head);

    height = _CalcHeight(head);
}

void RangeTree2D::Select(Vec2 l_bound, Vec2 h_bound) {
    _Unselect(head);

    _Select(head, l_bound, h_bound);
}

RangeTree2D::Node* RangeTree2D::_Construct(Vec2 *beg, Vec2 *end, RangeTree2D::Node *parent) {
    auto n = end - beg;
    Node* node;
    if(n == 1) {
        node = new Node(*beg);
        node->prev = parent;
        return node;
    }
    Vec2 *mid_point = beg + ((n - 1) / 2);
    //printf("for mid = (%f, %f)\n", mid_point->x, mid_point->y);
    node = new Node(*mid_point);

    Node* left = _Construct(beg, beg + ((n + 1) / 2), node);
    Node* right = _Construct(beg + ((n + 1) / 2), end, node);

    node->left = left;
    node->right = right;
    node->prev = parent;

    //std::vector<Vec2> _tmp(n);
    //std::copy(beg, end, _tmp.data());
    std::sort(beg, end, [](Vec2 a, Vec2 b) { return a.y < b.y; });
    //std::sort(_tmp.begin(), _tmp.end(), [](Vec2 &a, Vec2 &b) { return a.y < b.y; });
    //Vec2 *_beg = _tmp.begin().base(), *_end = _tmp.end().base();
    Vec2 *_beg = beg, *_end = end;

    //node->sub_left = new Node(*mid_point);
    left = _Construct(_beg, _beg + ((n + 1) / 2), node);
    right = _Construct(_beg + ((n + 1) / 2), _end, node);

    node->sub_left = left;
    node->sub_right = right;

    return node;
}

int RangeTree2D::_CalcHeight(RangeTree2D::Node *node, int level) {
    if(!node)
        return level;

    return std::max(_CalcHeight(node->left, level + 1), _CalcHeight(node->right, level + 1));
}

void RangeTree2D::_Unselect(RangeTree2D::Node *node) {
    if(!node)
        return;

    node->is_selected = false;

    _Unselect(node->left);
    _Unselect(node->right);
    _Unselect(node->sub_left);
    _Unselect(node->sub_right);
}

void RangeTree2D::_Select(RangeTree2D::Node *node, Vec2 l_bound, Vec2 h_bound) {
    if(!node) return;

    if(node->value.x >= l_bound.x && node->value.x <= h_bound.x &&
        node->value.y >= l_bound.y && node->value.y <= h_bound.y) {

        node->is_selected = true;
    }

    _Select(node->left, l_bound, h_bound);
    _Select(node->right, l_bound, h_bound);
    _Select(node->sub_left, l_bound, h_bound);
    _Select(node->sub_right, l_bound, h_bound);
}

StructuredPolygon::StructuredPolygon(const char* pts_file_name, const char* egs_file_name, Vec2 scale, Vec2 offset) {
    using namespace std;

    // Read points
    fstream file(pts_file_name);

    if(!file.good())
        return;

    size_t size = 0;
    file >> size;

    points.resize(size);

    Vec2 point;
    int idx = 0;
    while(file >> point.x && file >> point.y) {
        points[idx++] = point * scale + offset;
    }

    file.close();

    // Read edges
    file = fstream(egs_file_name);
    if(!file.good())
        return;

    file >> size;
    edges.resize(size);

    Edge edge;
    idx = 0;
    while(file >> edge.start && file >> edge.end) {
        edges[idx++] = edge;
    }
}

TriangleMesh::TriangleMesh(const char *pts_file_name, const char *tri_file_name, Vec2 scale, Vec2 offset) {
    using namespace std;

    // Read points
    fstream file(pts_file_name);

    if(!file.good())
        return;

    size_t size = 0;
    file >> size;

    nodes.resize(size);

    Vec2 point;
    int idx = 0;
    while(file >> point.x && file >> point.y) {
        nodes[idx++] = point * scale + offset;
    }

    file.close();

    // Read edges
    file = fstream(tri_file_name);
    if(!file.good())
        return;

    file >> size;
    elements.resize(size);

    MeshElement elem;
    idx = 0;
    while(file >> elem.points[0] && file >> elem.points[1] && file >> elem.points[2]) {
        elements[idx++] = elem;
    }
}


TriangleMesh::TriangleMesh(const char *pts_file_name, const char *edg_file_name, float r, Vec2 scale, Vec2 offset) {
    // Create a polygon from points and edges first
    poly = StructuredPolygon(pts_file_name, edg_file_name, scale, offset);

    _scale = scale;
    _offset = offset;

    // Do the algorithm (almost)
    Recalculate(r);
}


// Ignore all this tbh. just go to "StepCalculation"
// this is here so I can show the mesh generation process step-by-step
void TriangleMesh::Recalculate(float r, int start_idx) {
    r *= fabs(_scale.x);

    static int idx = 0;

    if(start_idx != -1)
        idx = start_idx;

    if(idx_cap < idx || start_idx != -1) {
        nodes.clear();
        edges.clear();
        elements.clear();

        nodes.resize(poly.points.size());
        std::copy(poly.points.begin(), poly.points.end(), nodes.begin());

        edges.resize(poly.edges.size());
        std::copy(poly.edges.begin(), poly.edges.end(), edges.begin());

        idx = 0;
    }

    StepCalculations(r, idx);
}

// Advancing front algorithm
void TriangleMesh::StepCalculations(float r, int& idx) {
    // Define some constants used in the program
    const float s60 = sinf(60 * M_PI / 180.0);
    const float c60 = cosf(60 * M_PI / 180.0);
    const auto front_edges_count = poly.edges.size();

    next_triangle:
    while(idx < edges.size() && (idx_cap < 0 || idx < idx_cap)) {

        // Check if current points are not already building some other triangle
        // If AB is part of 2 or more triangles (or 1 triangle and the original front)
        // then you cannot create more triangles using it
        int tri_count = 0;
        int mod_n_1 = front_edges_count - 1;
        bool is_front = edges[idx].start < front_edges_count && edges[idx].end < front_edges_count && (abs(edges[idx].start - edges[idx].end) == 1 || abs(edges[idx].start - edges[idx].end) == mod_n_1);
        for(auto& elem : elements) {
            if(elem.Contains(edges[idx].start) && elem.Contains(edges[idx].end)){
                tri_count += is_front ? 2 : 1;
                if(tri_count >= 2) {
                    // Skip this triangle
                    printf("Duplicate (idx: %i, edges: [%i, %i])\n", idx, edges[idx].start, edges[idx].end);
                    idx += 1;
                    goto next_triangle;
                }
            }
        }

        // Get 2 consecutive points creating the next edge
        A = nodes[edges[idx].start];
        B = nodes[edges[idx].end];

        // Third point that creates the equilateral triangle ABC (or rather ACB I guess...)
        Vec2 C = {c60 * (A.x - B.x) - s60 * (A.y - B.y) + B.x,
                  s60 * (A.x - B.x) + c60 * (A.y - B.y) + B.y };

        // Check if any points exist that are within the radius "r" of the point C
        float dist = 0;
        int merged_idx = _CheckPointProximity(C, idx, r, dist);
        bool was_merged = merged_idx != -1;
        C = was_merged ? nodes[merged_idx] : C;

        // Point C is outside the mesh
        if(!was_merged && !_PointTest(C)) {

            // Get the other point making the equilateral triangle, using a different method, cuz why not
            Vec2 mid_point = (A+B)/2; //center point
            Vec2 ov = Vec2(A.y - B.y, B.x - A.x); //orthogonal vector
            Vec2 C_rot = mid_point + (ov * sqrtf(3.f/4.f) );

            if(!_PointTest(C_rot)) {
                float rot_dist = 0;
                int rot_idx = _CheckPointProximity(C_rot, idx, 999999, rot_dist);
                if(rot_idx == -1) {
                    idx += 1;
                    printf("Skipping a triangle (Bounds) [idx: {%i}, edges: (%i, %i)]\n", idx, edges[idx].start, edges[idx].end);
                    continue;
                }

                merged_idx = _CheckPointProximity(C, idx, 999999, dist);
                merged_idx = rot_dist < dist ? rot_idx : merged_idx; // Choose the "better" triangle
                //merged_idx = rot_idx;
            }
            else {
                // New (rotated) point C is within the bounds of the mesh
                merged_idx = _CheckPointProximity(C_rot, idx, r, dist);
                if(merged_idx == -1)
                    C = C_rot;
            }

            was_merged = merged_idx != -1;
            C = was_merged ? nodes[merged_idx] : C;
        }

        // Colliding with other elements
        if(!was_merged && _IsInsideMesh(C)) {
            merged_idx = _CheckPointProximity(C, idx, 999999, dist);
            if(merged_idx == -1) {
                idx += 1;
                printf("Skipping a triangle (Mesh) [idx: {%i}, edges: (%i, %i)]\n", idx, edges[idx].start, edges[idx].end);
                continue;
            }

            was_merged = true;
            C = nodes[merged_idx];
        }

        // Crosses front
        if(!was_merged && _CrossesFront(C, -1, idx)) {
            merged_idx = _CheckPointProximity(C, idx, 999999, dist);
            if(merged_idx == -1) {
                idx += 1;
                printf("Skipping a triangle (Front) [idx: {%i}, edges: (%i, %i)]\n", idx, edges[idx].start, edges[idx].end);
                continue;
            }

            was_merged = true;
            C = nodes[merged_idx];
        }

        int C_idx = was_merged ? merged_idx : nodes.size();

        // Push new triangle
        elements.push_back({edges[idx].start, C_idx, edges[idx].end});

        // Push 2 new edges going to the point C
        edges.emplace_back(edges[idx].start, C_idx);
        edges.emplace_back(C_idx, edges[idx].end);

        //edges[idx] = Edge(-1, -1);

        // We don't want to push a new point if it's not really "new" (it was merged)
        if(!was_merged)
            nodes.push_back(C);

        idx++;
    }
}

// From lab04 I think, Polygon-Point test
bool TriangleMesh::_PointTest(Vec2 p) {
    //Vec2 lastPoint = poly.points[poly.points.size() - 1];

    int left = 0;

    GeneralLineFunc horizontal_line{0, 1, -p.y};

    for (int i = 0; i < poly.edges.size(); ++i) {
        auto& e = poly.edges[i];
        Vec2 start = poly.points[e.start];
        Vec2 end = poly.points[e.end];

        if(IsY_OnAABB(end, start, p.y)) {
            //auto func = GeneralLineFunc(point, lastPoint);
            auto intersect = GeneralLineFunc(start, end).GetCollisionPoint(horizontal_line);

            // Left side
            if(intersect.x <= p.x)
            {
                //printf(" (LEFT)\n");
                if(fminf(end.y, start.y) < p.y && fmaxf(end.y, start.y) >= p.y)
                    left++;
            }
        }

        //lastPoint = end;
    }

    return left % 2 == 1;
}

// orientation of points
bool ccw(Vec2 A, Vec2  B, Vec2 C) {
    return (((C.y - A.y) * (B.x - A.x )) > ((B.y - A.y) * (C.x - A.x)));
}

// Check if line segment AB intersects line segment CD
bool intersect(Vec2 A, Vec2  B, Vec2 C, Vec2 D) {
    return ccw(A, C, D) != ccw(B, C, D) && ccw(A, B, C) != ccw(A, B, D);
}

// line AB
bool isPointOnLine(Vec2 A, Vec2  B, Vec2 C) {
    const float epsilon = 0.0001;
    float val = (A.dist(C) + C.dist(B) - A.dist(B));
    return -epsilon < val && val < epsilon;
}

bool TriangleMesh::_CrossesFront(Vec2 p1, int pi, int idx) {

    // Check if the line form p1 to A and B don't create intersections with already existing edges
    // And the line AB itself (that would be unusual)
    for(int i = 0; i < edges.size(); i++) {
        auto& e = edges[i];

        if(i == idx)
            continue;

        Vec2 start = nodes[e.start], end = nodes[e.end];

        if(start != A && end != A && start != B && end != B && intersect(A, B, start, end))
            return true;

        if(start != A && end != A && e.start != pi && e.end != pi && intersect(A, p1, start, end))
            return true;

        if(start != B && end != B && e.start != pi && e.end != pi && intersect(B, p1, start, end))
            return true;
    }

    // Same but for already existing triangles
    for(int i = 0; i < elements.size(); i++) {
        auto& e = elements[i];
        Vec2 last_p = nodes[e.points[2]];
        for(int j = 0; j < 3; j++) {
            if(e.points[j] == pi)
                continue;

            Vec2 m_point = nodes[e.points[j]];

            if(last_p != A && m_point != A && intersect(p1, A, last_p, m_point))
                return true;

            if(last_p != B && m_point != B && intersect(p1, B, last_p, m_point))
                return true;

            if(m_point != A && isPointOnLine(A, p1, m_point))
                return true;

            if(m_point != B && isPointOnLine(B, p1, m_point))
                return true;

            last_p = m_point;
        }
    }

    return false;
}

bool TriangleMesh::_IsLineOccupied(int pi, int idx) {
    int tri_count_A = 0, tri_count_B = 0;

    int mod_n_1 = poly.edges.size() - 1;

    // These 2 variables denote (A and p) or (B and p) are part of the outline (front)
    bool is_A_on_front = edges[idx].start < poly.edges.size() && pi < poly.edges.size() &&
            (abs(edges[idx].start - pi) == 1 || abs(edges[idx].start - pi) == mod_n_1);
    if(is_A_on_front)
        is_A_on_front &= std::any_of(poly.edges.begin(), poly.edges.end(), [&](const Edge &e) {
            return (e.start == edges[idx].start && e.end == pi) || (e.start == pi && e.end == edges[idx].start);
        });

    bool is_B_on_front = edges[idx].end < poly.edges.size() && pi < poly.edges.size() &&
            (abs(edges[idx].end - pi) == 1 || abs(edges[idx].end - pi) == mod_n_1);
    if(is_B_on_front)
        is_B_on_front &= std::any_of(poly.edges.begin(), poly.edges.end(), [&](const Edge &e) {
            return (e.start == edges[idx].end && e.end == pi) || (e.start == pi && e.end == edges[idx].end);
        });

    // Count the number of triangles each edge makes, if it's more than 2 (1 in case of outline edges)
    // then the line is already occupied
    for(int i = 0; i < elements.size(); i++) {
        auto& e = elements[i];

        if(e.Contains(edges[idx].start) && e.Contains(pi))
            tri_count_A += is_A_on_front ? 2 : 1;
        if(e.Contains(edges[idx].end) && e.Contains(pi))
            tri_count_B += is_B_on_front ? 2 : 1;

        // Check if the entire triangle is a duplicate of some other already existing triangle
        if(e.Contains(edges[idx].start) && e.Contains(edges[idx].end) && e.Contains(pi))
            return true;

        if(tri_count_A >= 2 || tri_count_B >= 2) {
            return true;
        }
    }

    return false;
}


int TriangleMesh::_CheckPointProximity(Vec2 C, int idx, float radius, float &distance) {
    float best_rating = 1000000; // triangle rating (rates its shape)
    int best_point = -1;

    float AB_dist = A.dist(B);

    // true if at least one A or B is part of the outline
    bool do_front_check = edges[idx].start < poly.edges.size() || edges[idx].end < poly.edges.size();
    int mod_n_1 = poly.edges.size() - 1;
    GeneralLineFunc AB_line(A, B);

    // Each "subfunction" is defined within its definition
    for(int i = 0; i < nodes.size(); i++) {
        Vec2 p = nodes[i];
        if(p == A || p == B)
            continue;

        float dist = p.dist(C);
        float rating = fabs(AB_dist-A.dist(p)); // used to calculate how "good" the triangle is (how close to equilateral it is)
        if(dist > radius || rating >= best_rating)
            continue;

        if(abs(GetDistanceFromPointToLine(AB_line, p)) < 0.0001)
            continue;

        if(_CrossesFront(p, i, idx))
            continue;

        if(_IsLineOccupied(i, idx))
            continue;

        if(do_front_check) {
            // Check if middle points of edges Ap and Bp are within the polygon
            // This holds because we are looking for the closest point, meaning there won't be another point that's
            // in the middle of that edge and nowhere else, because then we would take it instead of the current one
            // Mostly a sanity check tbh.
            bool c1 = (abs(edges[idx].start - i) == 1 || abs(edges[idx].start - i) == mod_n_1) || _PointTest((p + A) / 2);
            if(!c1) continue;

            bool c2 = (abs(edges[idx].end - i) == 1 || abs(edges[idx].end - i) == mod_n_1) || _PointTest((p + B) / 2);
            if(!c2) continue;
        }

        best_rating = rating;
        best_point = i;
    }

    distance = best_rating;
    return best_point;
}

// Check if given point p is inside any already created triangles (we don't want that, because that's overlap)
bool TriangleMesh::_IsInsideMesh(Vec2 p) {
    for(auto & e : elements) {
        // line: nodes[e.points[0]], nodes[e.points[1]]. Point: p
        float d1, d2, d3;
        d1 = GetOrientationOfPointsAlongLine(nodes[e.points[1]], nodes[e.points[0]], p);
        d2 = GetOrientationOfPointsAlongLine(nodes[e.points[2]], nodes[e.points[1]], p);
        d3 = GetOrientationOfPointsAlongLine(nodes[e.points[0]], nodes[e.points[2]], p);

        bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        if(!(has_neg && has_pos))
            return true;
    }

    return false;
}

void TriangleMesh::Export(const char *pts_file_name, const char *elems_file_name, bool appy_reverse_transform, Vec2 scale, Vec2 offset) {
    using namespace std;

    // Write points
    fstream file(pts_file_name, ios_base::out);

    if(!file.good())
        return;

    file << nodes.size() << '\n';

    for(auto& pos : nodes) {
        if(appy_reverse_transform)
            file << (((pos - _offset) / _scale + offset) * scale);
        else
            file << ((pos + offset) * scale);
    }

    file = fstream(elems_file_name, ios_base::out);

    if(!file.good())
        return;

    file << elements.size() << '\n';

    for(auto& e : elements) {
        file << e;
    }
}

void TriangulationMesh::TriangulateMesh(const char *pts_file_name, TriangulationMesh::Triangulate_Method method,
                                   Vec2 scale, Vec2 offset) {

    pc = PointCloud(pts_file_name, scale, offset, false);

    // Generate the super triangle
    superTriangle = _GenerateSuperTriangle();

    // Add the super triangle points to the points array
    pc.points.insert(pc.points.begin(), superTriangle.vtx, superTriangle.vtx + 3);
    elements.push_back({0,1,2});

    _scale = scale;
    _offset = offset;

    _are_stats_dirty = true;

    RecalculateMesh(method);
}

void TriangulationMesh::RecalculateMesh(Triangulate_Method method) {
    elements.clear();

    if(method == Triangulate_Delaunay) {
        // First 3 points are the super triangle
        superTriangle = _GenerateSuperTriangle();
        std::copy(pc.points.begin(), pc.points.begin() + 3, superTriangle.vtx);
        elements.push_back({0,1,2});

        TriangulateDelaunay();
    }
    else if(method == Triangulate_DelaunayWeighted) {
        bb = pc.GetBoundingBox(3);
        TriangulateWeighted();
    }
}

template<typename Ty>
Ty lerp(Ty a, Ty b, float f)
{
    return a * (1.0 - f) + (b * f);
}

// [< O(n^3)]
void TriangulationMesh::TriangulateDelaunay() {
    // [O(n)]
    for(int i = 0; i < pc.points.size(); i++) {
        Vec2& p = pc.points[i];

        std::unordered_set<int> bad_tris;

        // Check if point is inside the circles of other triangles [O(n)]
        for(int j = 0; j < elements.size(); j++) {
            auto& elem = elements[j];
            auto tri = _GetTriangleFromElement(elem); // convert element to a triangle
            auto c = GetTriangleCircumscribedCircle(tri); // get the circle built on that triangle
            if(c.ContainsPoint(p)) {
                bad_tris.insert(j);
            }
        }

        //StructuredPolygon bad_poly;
        std::vector<Edge> bad_poly;

        for(int tri_idx : bad_tris) {       // [O(n)]
            //if(elements[tri_idx].points[0] == -1) continue;
            int last_idx = elements[tri_idx].points[2]; // take the last point to make an edges from the triangle
            for(int idx : elements[tri_idx].points) {       // [O(3)]
                bool is_occupied = false;
                // Check if any other "bad" triangles contain this edge (idx - last_idx)
                for(int other_idx : bad_tris) {     // [O(n)]
                    if(tri_idx == other_idx) continue;
                    if(elements[other_idx].Contains(idx) && elements[other_idx].Contains(last_idx)) {
                        is_occupied = true;
                        break;
                    }
                }
                // No "bad" triangle contains this edge
                if(!is_occupied) {
                    bad_poly.emplace_back(idx, last_idx);
                }

                last_idx = idx;
            }
        }

        // Remove triangles that are "bad" from the end vector
        // this is really bad...
        int _t_idx = 0;
        elements.erase(std::remove_if(elements.begin(), elements.end(),
                                      [&bad_tris, &_t_idx] (MeshElement<3> _)
                                      { return bad_tris.find(_t_idx++) != bad_tris.end(); }),
                       elements.end());

        // Construct triangles from the bad polys and the new point
        //elements.resize(elements.size() + bad_poly.size());
        //std::transform(bad_poly.end(), bad_poly.end(), std::back_inserter(elements),
        //               [i](const auto& e) {return MeshElement<3>({e.start, i, e.end});});
        for(auto& e : bad_poly)
            elements.push_back( { e.start, i, e.end } );
    }

    // Remove all the triangles that connect to the super triangle (0,1,2)
    elements.erase(std::remove_if(elements.begin(), elements.end(),
                                  [] (MeshElement<3>& elem)
                                  { return elem.ContainsAny({0,1,2}); }
    ), elements.end());

    _are_stats_dirty = true;

    // Recalculate mesh stats
    //GetMeshStats();
}

Triangle TriangulationMesh::_GenerateSuperTriangle() {
    Triangle tri;

    /*
    //Vec2 top_most = {-FLT_MAX, -FLT_MAX}, bottom_most = {FLT_MAX, FLT_MAX}, left_most = {FLT_MAX, FLT_MAX}, right_most = {-FLT_MAX, -FLT_MAX};
    Vec2 M; // middle point

    for(const auto & p : pc.points) {
    //    if(p.y > top_most.y)
    //        top_most = p;
    //    if(p.y < bottom_most.y)
    //        bottom_most = p;
    //    if(p.x > right_most.x)
    //        right_most = p;
    //    if(p.x < left_most.x)
    //        left_most = p;

        M += p;
    }

    M = M / (float)pc.points.size();

    // Use only the furthest :3 points
    //Vec2 points[4] = {top_most, bottom_most, left_most, right_most};
    //std::sort(points, points + 4, [M](Vec2 e1, Vec2 e2) {return M.dist(e1) > M.dist(e2);});
    //std::copy(points, points + 3, tri.vtx);
     */

    // Create triangle from the bounding box (from the middle points of 2 edges and the bottom-left corner)
    auto bb = pc.GetBoundingBox();
    tri.vtx[0] = Vec2((bb.max.x + bb.min.x) / 2, bb.max.y);
    tri.vtx[1] = bb.min;
    tri.vtx[2] = Vec2(bb.max.x, (bb.max.y + bb.min.y) / 2);

RescaleSuperTri:
    for(const auto & p : pc.points) {
        if(!tri.IsContained_Angles(p)) {
            tri.Scale(1.5);
            goto RescaleSuperTri;
        }
    }

    tri.Scale(1.1);

    return tri;
}

MeshStats TriangulationMesh::GetMeshStats() {
    if(!_are_stats_dirty || elements.empty())
        return _mesh_stats;

    //_mesh_stats.triangle_ratings.clear();
    _mesh_stats.triangle_ratings.resize(elements.size());
    _mesh_stats.rating_buckets = std::vector<float>(_mesh_stats.buckets, 0);
    double rating_sum = 0;

    // Recalculate mesh stats
    for(int idx = 0; idx < elements.size(); idx++) {
        auto& tri = elements[idx];
        float longest_side = 0;
        float shortest_side = FLT_MAX;

        // Shortest edge divided by the longest edge
        float rating = 0;
        for(int i = 0; i < 3; i++) {
            float dst = pc.points[tri.points[i]].dist(pc.points[tri.points[(i + 1) % 3]]);
            //if(dst > longest_side) {
            //    rating -= longest_side;
            //    longest_side = dst;
            //}
            shortest_side = fmin(shortest_side, dst);
            longest_side = fmax(longest_side, dst);
            //rating += dst;
        }

        //float d1, d2, d3;
        //d1 = pc.points[tri.points[0]].dist(pc.points[tri.points[1]]);
        //d2 = pc.points[tri.points[1]].dist(pc.points[tri.points[2]]);
        //d3 = pc.points[tri.points[2]].dist(pc.points[tri.points[0]]);

        rating = shortest_side / longest_side;

        int bucket_idx = (int)(_mesh_stats.buckets * rating);
        _mesh_stats.rating_buckets[bucket_idx]++;
        rating_sum += rating;

        //printf("Rating = %.2f (%.2f, %.2f, %.2f)\n", rating, d1, d2, d3);

        //_mesh_stats.triangle_ratings.push_back(rating);
        _mesh_stats.triangle_ratings[idx] = rating;
    }

    // Use for relative data
    //for(auto& r : _mesh_stats.rating_buckets)
    //    r /= elements.size();

    std::vector<float> ratings(_mesh_stats.triangle_ratings);
    std::sort(ratings.begin(), ratings.end());
    _mesh_stats.median_triangle_rating = ratings[elements.size()/2];
    _mesh_stats.mean_triangle_rating = rating_sum / elements.size();
    _mesh_stats.min_triangle_rating = _mesh_stats.triangle_ratings[0];
    _mesh_stats.max_triangle_rating = _mesh_stats.triangle_ratings[_mesh_stats.triangle_ratings.size()-1];

    _are_stats_dirty = false;

    return _mesh_stats;
}
void TriangulationMesh::TriangulateWeighted() {

    // Do the triangulation
    RecalculateMesh(Triangulate_Delaunay);

    std::map<std::pair<int, int>, int> map;

    for(int elem_idx = 0; elem_idx < elements.size(); elem_idx++) {
        int last_idx = elements[elem_idx].points[2];
        for(int e_idx : elements[elem_idx].points) {
            ++map[std::pair<int, int>(std::min(last_idx, e_idx), std::max(last_idx, e_idx))];

            last_idx = e_idx;
        }
    }

    // Find edges points part of the outline (front)
    std::unordered_set<int> front;
    for(auto& m : map) {
        if(m.second == 1)
            front.insert({m.first.first, m.first.second});
    }

    const auto n = pc.points.size();
    const float density = n / sqrtf(bb.GetArea());
    const float min_dist = 5.f / density;

    // "Relax" mesh with laplace smoothing
    for(int i = 3; i < n; i++) {
        if(front.find(i) != front.end()) continue;

        Vec2& p = pc.points[i];

        // retry with a new bounding box
        if(!bb.IsPointInside(p)) {
            bb = pc.GetBoundingBox();
            TriangulateWeighted();
            return;
        }

        std::vector<int> connected_points;

        for(auto& elem : elements) {
            if(elem.Contains(i)) {
                for(int e : elem.points)
                    if(e != i)
                    connected_points.push_back(e);
            }
        }

        if(!connected_points.empty()) {
            float distance_sum = 0;
            Vec2 mid_point = {0, 0};
            for (int cp: connected_points) {
                float dist = p.dist(pc.points[cp]) - (use_distance ? min_dist : 0);
                if(use_distance)
                    distance_sum += dist;
                mid_point += pc.points[cp] * (use_distance ? dist : 1);
            }
            mid_point = mid_point / (use_distance ? distance_sum : connected_points.size());
            p = mid_point;// - (global_vec / n * 10);//lerp(p, mid_point, 1);
        }
    }

    GetMeshStats();
    _are_stats_dirty = true;
}
