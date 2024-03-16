#include "Helpers.h"

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

GeneralLineFunc LineConnectingPointAndLine(GeneralLineFunc &func, Vec2 P1) {
    return {func.B, -func.A, (P1.y*func.A) - (func.B*P1.x)};
}
