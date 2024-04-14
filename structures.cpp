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
