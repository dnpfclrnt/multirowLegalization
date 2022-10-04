#ifndef _DIE_H
#define _DIE_H
// #include "../global.h"
#include "../utils.h"


struct ROW{
    int rowIdx;
    int rowHeight;
    int* cellIdx;
};


struct DIE{
    struct FPOS fpmin;
    struct FPOS fpmax;

    struct POS pmin;
    struct POS pmax;

    struct FPOS center;
    int width;
    int height;
    
    int rowLen;
    int rowHeight;
    int repeatCount;

    bool** placementMap;
}; typedef struct DIE* die_ptr;


// List of functions
die_ptr create_die(int pmin_x, int pmin_y, int pmax_x, int pmax_y);
void update_dieRow(die_ptr die, int rowLen, int rowHeight, int repeatCount);
void destroy_die(die_ptr rmdb);


#endif