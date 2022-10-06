#ifndef _UTILS_H
#define _UTILS_H


#include "global.h"


struct CharToken{
    char* stream;
    struct  CharToken* next;
}; typedef struct CharToken* charToken_ptr;


struct splitToken{
    char** split;
    int arrayLen;
}; typedef struct splitToken* splitToken_ptr;


//List of functions
void resetFPOS(struct FPOS target);
void resetPOS(struct POS target);
struct FPOS setFPOS(struct FPOS place, struct FPOS disp);
struct POS setPOS(struct POS place, struct POS disp);
struct FPOS posToFPOS(struct POS target);
struct POS fposToPOS(struct FPOS target);
struct POS subtractPOS(struct POS first, struct POS second);
struct FPOS subtractFPOS(struct FPOS first, struct FPOS second);
struct POS pmult(struct POS target, int multiply);
struct FPOS fpmult(struct FPOS target, prec multiply);
struct POS pdiv(struct POS target, int multiply);
struct FPOS fpdiv(struct FPOS target, prec multiply);
unsigned int hash_function(char* name);
charToken_ptr create_charToken(char* stream);
splitToken_ptr split_dash(char* word);
void destroy_token_chain(charToken_ptr rm_token);
splitToken_ptr split_line(char* line);
void destroy_splitToken(splitToken_ptr rm_token);
double stopwatch(struct timeval *begin, struct timeval *finish);


#endif