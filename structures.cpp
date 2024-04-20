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

// Advancing front algorithm
TriangleMesh::TriangleMesh(const char *pts_file_name, const char *edg_file_name, float r, Vec2 scale, Vec2 offset) {
    poly = StructuredPolygon(pts_file_name, edg_file_name, scale, offset);
    auto front_edges_count = poly.edges.size();

    // Define some constants used in the program
    const float s60 = sinf(60 * M_PI / 180.0);
    const float c60 = cosf(60 * M_PI / 180.0);
    const float sn60 = sinf(-60 * M_PI / 180.0);
    const float cn60 = cosf(-60 * M_PI / 180.0);

    nodes.resize(poly.points.size());
    std::copy(poly.points.begin(), poly.points.end(), nodes.begin());

    edges.resize(poly.edges.size());
    std::copy(poly.edges.begin(), poly.edges.end(), edges.begin());

    int idx = 0;
    A = nodes[poly.edges[0].start], B = nodes[poly.edges[0].end];

    while(A != B && idx < edges.size()) {
next_triangle:
        int tri_count = 0;
        for(auto& elem : elements) {
            if(elem.Contains(edges[idx].start) && elem.Contains(edges[idx].end)){
                tri_count += edges[idx].start < front_edges_count && edges[idx].end < front_edges_count ? 2 : 1;
                if(tri_count >= 2) {
                    // Skip this triangle
                    printf("Duplicate");
                    idx += 1;
                    goto next_triangle;
                }
            }
        }

        A = nodes[edges[idx].start];
        B = nodes[edges[idx].end];

        // Third point that creates the equilateral triangle
        Vec2 C = {c60 * (A.x - B.x) - s60 * (A.y - B.y) + B.x,
            s60 * (A.x - B.x) + c60 * (A.y - B.y) + B.y };

        float dist = 0;
        int merged_idx = _IsPointColliding(C, idx, r, dist);
        bool was_merged = merged_idx != -1;
        C = was_merged ? nodes[merged_idx] : C;

        if(!was_merged && _PointTest(C)) {
            Vec2 C_rot = {cn60 * (A.x - B.x) - sn60 * (A.y - B.y) + B.x,
                          sn60 * (A.x - B.x) + cn60 * (A.y - B.y) + B.y };

            if(!_PointTest(C_rot)) {
                float rot_dist = 0;
                int rot_idx = _IsPointColliding(C_rot, idx, 999999, rot_dist);
                merged_idx = _IsPointColliding(C, idx, 999999, dist);

                if(rot_idx == -1 && merged_idx == -1) {
                    idx += 1;
                    continue;
                }

                C = rot_dist < dist ? C_rot : C;
            }
            else
                merged_idx = _IsPointColliding(C, idx, r, dist);

            was_merged = merged_idx != -1;
            C = was_merged ? nodes[merged_idx] : C;
        }

        // Colliding with other elements
        if(!was_merged && _IsInsideMesh(C)) {
            merged_idx = _IsPointColliding(C, idx, 999999, dist);
            if(merged_idx == -1) {
                idx += 1;
                continue;
            }

            was_merged = true;
            C = nodes[merged_idx];
        }

        if(!was_merged && _CrossesFront(C, -1, idx)) {
            merged_idx = _IsPointColliding(C, idx, 999999, dist);
            if(merged_idx == -1) {
                idx += 1;
                continue;
            }

            was_merged = true;
            C = nodes[merged_idx];
        }
    }
}

bool TriangleMesh::_PointTest(Vec2 p) {
    Vec2 lastPoint = nodes[nodes.size() - 1];

    int left = 0;

    GeneralLineFunc horizontal_line{0, 1, -p.y};

    for (int i = 0; i < nodes.size(); ++i) {
        Vec2 point = nodes[i];

        if(IsY_OnAABB(point, lastPoint, p.y)) {
            //auto func = GeneralLineFunc(point, lastPoint);
            auto intersect = GeneralLineFunc(point, lastPoint).GetCollisionPoint(horizontal_line);

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

bool ccw(Vec2 A, Vec2  B, Vec2 C) {
    return (((C.y - A.y) * (B.x - A.x )) > ((B.y - A.y) * (C.x - A.x)));
}

bool intersect(Vec2 A, Vec2  B, Vec2 C, Vec2 D) {
    return ccw(A, C, D) != ccw(B, C, D) and ccw(A, B, C) != ccw(A, B, D);
}

bool isPointOnLine(Vec2 A, Vec2  B, Vec2 C) {
    const float epsilon = 0.1;
    return -epsilon < (A.dist(C) + C.dist(B) - A.dist(B));
}

bool TriangleMesh::_CrossesFront(Vec2 p1, int pi, int idx) {
    for(int i = 0; i < elements.size(); i++) {
        auto& e = elements[i];
        Vec2 last_p = nodes[e.points[2]];
        for(int j = 0; j < 3; j++) {
            if(nodes[e.points[j]] == p1)
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

            if(m_point != A && m_point != B && m_point != p1 && edges[idx].start < poly.edges.size() && edges[idx].end >= poly.edges.size() && p1 > 0 && pi < poly.edges.size() && abs(pi - edges[idx].start) < 3) {
                bool ApX_ccw = ccw(A, m_point, nodes[edges[(idx + 1) % edges.size()].end]);
                bool ABX_ccw = ccw(A, B, nodes[edges[(idx + 1) % edges.size()].end]);

                if(ApX_ccw != ABX_ccw)
                    return true;
            }

            last_p = m_point;
        }
    }
}

bool TriangleMesh::_IsLineOccupied(int pi, int idx) {
    int tri_count_A = 0, tri_count_B = 0;

    bool is_A_on_front = edges[idx].start < poly.edges.size() and pi < poly.edges.size() and abs(edges[idx].start - pi) == 1;
    bool is_B_on_front = edges[idx].end < poly.edges.size() and pi < poly.edges.size() and abs(edges[idx].end - pi) == 1;

    for(int i = 0; i < elements.size(); i++) {
        auto& e = elements[i];

        if(e.Contains(edges[idx].start) and e.Contains(pi))
            tri_count_A += is_A_on_front ? 2 : 1;
        if(e.Contains(edges[idx].end) and e.Contains(pi))
            tri_count_B += is_B_on_front ? 2 : 1;

        if(tri_count_A >= 2 || tri_count_B >= 2) {
            return true;
        }
    }

    return false;
}

int TriangleMesh::_IsPointColliding(Vec2 C, int idx, float radius, float &distance) {
    float best_proxim = 1000000;
    int best_point = -1;
    bool do_front_check = edges[idx].start < poly.edges.size() and edges[idx].end < poly.edges.size();

    for(int i = 0; i < nodes.size(); i++) {
        Vec2 p = nodes[i];
        if(p == A || p == B)
            continue;

        if(_CrossesFront(p, i))
            continue;

        if(_IsLineOccupied(i))
            continue;

        if(do_front_check) {
            bool c1 = _PointTest((p + A) / 2);
            bool c2 = _PointTest((p + B) / 2);
            if(!(c1 && c2)) {
                continue;
            }
        }

        float dist = p.dist(C);
        if(dist < radius && dist < best_proxim) {
            best_proxim = dist;
            best_point = i;
        }
    }

    return best_point;
}

bool TriangleMesh::_IsInsideMesh(Vec2 p) {
    for(auto & e : elements) {
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
