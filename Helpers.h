//#include "structures.h"

#define _USE_MATH_DEFINES
#include <math.h>

struct Vec2;
struct GeneralLineFunc;

#ifndef LAB01_HELPERS_H
#define LAB01_HELPERS_H

constexpr float RAD_2_DEG = 180 / M_PI;

template <typename T>
inline int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

bool PointBoxCheck(Vec2 A1, Vec2 B1, Vec2 A2, Vec2 B2, Vec2 P1);
bool IsY_OnAABB(Vec2 min, Vec2 max, float y);
GeneralLineFunc LineConnectingPointAndLine(GeneralLineFunc& func, Vec2 P1);
float AngleBetweenPoints(Vec2 p1, Vec2 m, Vec2 p3);
float AngleBetweenPoints(Vec2* points);
short GetOrientationOfPointsAlongLine(Vec2 p3, Vec2 p1, Vec2 p2);
float GetDistanceFromPointToLine(GeneralLineFunc func, Vec2 p1);
// val < 0 - collinear, val == 0 - clockwise, val > 0 - counter clockwise
float GetPointsOrientation(Vec2 p, Vec2 q, Vec2 r);

#endif //LAB01_HELPERS_H
