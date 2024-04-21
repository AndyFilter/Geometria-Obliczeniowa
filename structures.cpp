#include "structures.h"

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

    A = nodes[edges[0].start], B = nodes[edges[0].end];

    next_triangle:
    while(A != B && idx < edges.size() && (idx_cap < 0 || idx < idx_cap)) {

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

        // Third point that creates the equilateral triangle
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
                //merged_idx = _CheckPointProximity(C, idx, 999999, dist);

                if(rot_idx == -1) {
                    idx += 1;
                    printf("Skipping a triangle (Bounds) [idx: {%i}, edges: (%i, %i)]\n", idx, edges[idx].start, edges[idx].end);
                    continue;
                }

                //merged_idx = rot_dist < dist ? rot_idx : merged_idx;
                merged_idx = rot_idx;
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
    return ccw(A, C, D) != ccw(B, C, D) and ccw(A, B, C) != ccw(A, B, D);
}

// line AB
bool isPointOnLine(Vec2 A, Vec2  B, Vec2 C) {
    const float epsilon = 0.001;
    return -epsilon < (A.dist(C) + C.dist(B) - A.dist(B)) < epsilon;
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
    bool is_A_on_front = edges[idx].start < poly.edges.size() and pi < poly.edges.size() and
            (abs(edges[idx].start - pi) == 1 || abs(edges[idx].start - pi) == mod_n_1);
    bool is_B_on_front = edges[idx].end < poly.edges.size() and pi < poly.edges.size() and
            (abs(edges[idx].end - pi) == 1 || abs(edges[idx].end - pi) == mod_n_1);

    // Count the number of triangles each edge makes, if it's more than 2 (1 in case of outline edges)
    // then the line is already occupied
    for(int i = 0; i < elements.size(); i++) {
        auto& e = elements[i];

        if(e.Contains(edges[idx].start) and e.Contains(pi))
            tri_count_A += is_A_on_front ? 2 : 1;
        if(e.Contains(edges[idx].end) and e.Contains(pi))
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
    float best_proxim = 1000000;
    int best_point = -1;

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
        if(dist > radius || dist >= best_proxim)
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

        best_proxim = dist;
        best_point = i;
    }

    distance = best_proxim;
    return best_point;
}

// Check if given point p is inside any already created triangles (we don't want that, because that's overlap)
bool TriangleMesh::_IsInsideMesh(Vec2 p) {
    for(auto & e : elements) {
        // line: nodes[e.points[0]], nodes[e.points[1]]. Point: p
        float d1 = GetOrientationOfPointsAlongLine(nodes[e.points[0]], nodes[e.points[1]], p);
        float d2 = GetOrientationOfPointsAlongLine(nodes[e.points[1]], nodes[e.points[2]], p);
        float d3 = GetOrientationOfPointsAlongLine(nodes[e.points[2]], nodes[e.points[0]], p);

        bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        if(!(has_neg && has_pos))
            return true;
    }

    return false;
}

void TriangleMesh::Export(const char *pts_file_name, const char *elems_file_name, Vec2 scale, Vec2 offset) {
    using namespace std;

    // Write points
    fstream file(pts_file_name, ios_base::out);

    if(!file.good())
        return;

    file << nodes.size() << '\n';

    for(auto& pos : nodes) {
        file << ((pos * scale) + offset);
    }

    file = fstream(elems_file_name, ios_base::out);

    if(!file.good())
        return;

    file << elements.size() << '\n';

    for(auto& e : elements) {
        file << e;
    }
}
