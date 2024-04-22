#include "Helpers.h"
#include "structures.h"

bool PointBoxCheck(Vec2 A1, Vec2 A2, Vec2 B1, Vec2 B2, Vec2 P1) {

    if(P1.x < fminf(A1.x,A2.x))
        return false;

    if(P1.x < fminf(B1.x,B2.x))
        return false;

    if(P1.y < fminf(A1.y, A2.y))
        return false;

    if(P1.y < fminf(B1.y, B2.y))
        return false;


    if(P1.x > fmaxf(A1.x,A2.x))
        return false;

    if(P1.x > fmaxf(B1.x,B2.x))
        return false;

    if(P1.y > fmaxf(A1.y, A2.y))
        return false;

    if(P1.y > fmaxf(B1.y, B2.y))
        return false;

    return true;
}

// Inclusive range
bool IsY_OnAABB(Vec2 min, Vec2 max, float y) {
    return y <= fmaxf(max.y, min.y) && y >= fminf(min.y, max.y);
}

GeneralLineFunc LineConnectingPointAndLine(GeneralLineFunc &func, Vec2 P1) {
    return {func.B, -func.A, (P1.y*func.A) - (func.B*P1.x)};
}

float AngleBetweenPoints(Vec2 p1, Vec2 p2, Vec2 p3) {
    p1 -= p2;
    p3 -= p2;

    return acosf(p1.dotProduct(p3) / (p1.mag() * p3.mag()));
}

float AngleBetweenPoints(Vec2* points) {
    return AngleBetweenPoints(points[0], points[1], points[2]);
}

float GetOrientationOfPointsAlongLine(Vec2 l1, Vec2 l2, Vec2 p1) {
    //return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)
    return (l2.x - l1.x)*(p1.y - l1.y) - (l2.y - l1.y)*(p1.x - l1.x);
    //return (p1.x - p3.x)*(p2.y - p3.y) - (p2.x - p3.x)*(p1.y - p3.y);
}

//short GetDistanceFromPointToLine(Vec2 p3, Vec2 p1, Vec2 p2) {
//    return (p1.x - p3.x) * (p2.y - p3.y) - (p1.y - p3.y) * (p2.x - p3.x);
//}

float GetDistanceFromPointToLine(GeneralLineFunc func, Vec2 p1) {
    return (func.B * p1.y) + (func.A * p1.x) + func.C;
}

float GetPointsOrientation(Vec2 p, Vec2 q, Vec2 r) {
    float val = (q.y - p.y) * (r.x - q.x) -
                (q.x - p.x) * (r.y - q.y);

    return val;
}
