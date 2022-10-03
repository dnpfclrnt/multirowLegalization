#ifndef _INSTANCE_H
#define _INSTANCE_H


#include "../global.h"
#include "../utils.h"
#include "net.h"


// Instance structure
struct INSTANCE{
    char* instName;

    struct FPOS fpmin;
    struct FPOS fcent;
    struct FPOS fpmax;

    struct POS pmin;
    struct POS cent;
    struct POS pmax;
    
    struct POS size;
    struct FPOS fsize;

    int numPins;
    pin_ptr* instPinArray;

    int rowHeight;
    int numRows;
    int* rowIdxArray;
}; typedef struct INSTANCE* instance_ptr;


struct INSTDB{
    int numInst;
    int curNumInst;
    instance_ptr* instArray;
}; typedef struct INSTDB* instDB_ptr;


// List of functions
instance_ptr create_instance(char* instName, int sizeX, int sizeY, int numPins, int rowHeight, ...);
void destroy_instance(instance_ptr inst);
instDB_ptr create_instDB(int numInst);
void destroy_instDB(instDB_ptr rmdb);

void place_instance(instance_ptr inst, struct POS lowerLeft);
void flip_instance_horizontal(instance_ptr inst);
void flip_instance_vertical(instance_ptr inst);
void rotate_instance(instance_ptr inst);

#endif