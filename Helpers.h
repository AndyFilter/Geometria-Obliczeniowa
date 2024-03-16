#include "structures.h"

#ifndef LAB01_HELPERS_H
#define LAB01_HELPERS_H

bool PointBoxCheck(Vec2 A1, Vec2 B1, Vec2 A2, Vec2 B2, Vec2 P1);
GeneralLineFunc LineConnectingPointAndLine(GeneralLineFunc& func, Vec2 P1);

#endif //LAB01_HELPERS_H
