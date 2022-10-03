#include "global.h"


void resetFPOS(struct FPOS target);
void resetPOS(struct POS target);
void setFPOS(struct FPOS target, struct FPOS place, struct FPOS disp);
void setPOS(struct POS target, struct POS place, struct POS disp);
struct FPOS posToFPOS(struct POS target);
struct POS fposToPOS(struct FPOS target);
struct POS pmult(struct POS target, int multiply);
struct FPOS fpmult(struct FPOS target, prec multiply);
struct POS pdiv(struct POS target, int multiply);
struct FPOS fpdiv(struct FPOS target, prec multiply);
