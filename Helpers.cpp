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