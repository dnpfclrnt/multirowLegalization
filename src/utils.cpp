#include "utils.h"


void resetFPOS(struct FPOS target)
{
    target.x = target.y = -1;
}


void resetPOS(struct POS target)
{
    target.x = target.y = -1;
}


void setFPOS(struct FPOS target, struct FPOS place, struct FPOS disp)
{
    target.x = place.x + disp.x;
    target.y = place.y + disp.y;
}


void setPOS(struct POS target, struct POS place, struct POS disp)
{
    target.x = place.x + disp.x;
    target.y = place.y + disp.y;
}


struct FPOS posToFPOS(struct POS target)
{
    struct FPOS ret;
    ret.x = (prec)target.x;
    ret.y = (prec)target.y;
    return ret;
}


struct POS fposToPOS(struct FPOS target)
{
    struct POS ret;
    ret.x = (int)target.x;
    ret.y = (int)target.y;
    return ret;
}


struct POS pmult(struct POS target, int multiply)
{
    struct POS ret;
    ret.x = target.x * multiply;
    ret.y = target.y * multiply;
    return ret;
}


struct FPOS fpmult(struct FPOS target, prec multiply)
{
    struct FPOS ret;
    ret.x = target.x * multiply;
    ret.y = target.y * multiply;
    return ret;
}


struct POS pdiv(struct POS target, int multiply)
{
    struct POS ret;
    ret.x = target.x / multiply;
    ret.y = target.y / multiply;
    return ret;
}


struct FPOS fpdiv(struct FPOS target, prec multiply)
{
    struct FPOS ret;
    ret.x = target.x / multiply;
    ret.y = target.y / multiply;
    return ret;
}
