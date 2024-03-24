//#include "structures.h"

#include <cmath>

struct Vec2;
struct GeneralLineFunc;

#ifndef LAB01_HELPERS_H
#define LAB01_HELPERS_H

constexpr float RAD_2_DEG = 180 / M_PI;

bool PointBoxCheck(Vec2 A1, Vec2 B1, Vec2 A2, Vec2 B2, Vec2 P1);
bool IsY_OnAABB(Vec2 min, Vec2 max, float y);
GeneralLineFunc LineConnectingPointAndLine(GeneralLineFunc& func, Vec2 P1);
float AngleBetweenPoints(Vec2 p1, Vec2 m, Vec2 p3);
float AngleBetweenPoints(Vec2* points);

#endif //LAB01_HELPERS_H
