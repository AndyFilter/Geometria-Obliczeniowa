#include "structures.h"

RangeTree1D::RangeTree1D(float *beg, float *end) {
    auto n = (end - beg);
    points.resize(n);
    std::copy(beg, end, points.data());

    std::sort(points.begin(), points.end());
    beg = points.data();
    end = points.data() + n;

    head = new Node(*(beg + (n - 1) / 2));

    _Construct(beg, beg + n / 2, head);
    _Construct(beg + n / 2, end, head);

    height = _CalcHeight(head);
}

void RangeTree1D::_Construct(float *beg, float *end, Node* node) {
    if (beg >= end)
        return;

    auto n = end - beg;
    float *mid_point = beg + ((n - 1) / 2);
    printf("for mid = %f\n", *mid_point);
    Node *&child_node = *mid_point <= node->value ? node->left : node->right;
    child_node = new Node(*mid_point);
    child_node->prev = node;

    if (n == 1)
        return;

    _Construct(beg, beg + ((n + 1) / 2), child_node);
    _Construct(beg + ((n + 1) / 2), end, child_node);
}

int RangeTree1D::_CalcHeight(RangeTree1D::Node *node, int level) {
    if(!node)
        return level;

    return std::max(_CalcHeight(node->left, level + 1), _CalcHeight(node->right, level + 1));
}

void RangeTree1D::Select(float l_bound, float h_bound) {

}

//RangeTree2D::RangeTree2D(Vec2* beg, Vec2* end) {
//    points.resize((end - beg));
//    std::copy(beg, end, points.data());
//
//    std::sort(points.begin(), points.end());
//}
