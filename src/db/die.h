#ifndef _DIE_H
#define _DIE_H
#include "../global.h"
#include "../utils.h"


struct ROW{
    int rowIdx;
    int rowHeight;
    int* cellIdx;
};


struct DIE{
    struct FPOS lowerLeft;
    struct FPOS upperRight;
    
    int rowHeight;
    int repeatCount;

    bool** placementMap;
}; typedef struct DIE* die_ptr;


#endif