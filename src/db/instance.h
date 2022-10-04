#ifndef _INSTANCE_H
#define _INSTANCE_H


#include "../global.h"
#include "../utils.h"
#include "net.h"


// Instance structure
struct INSTANCE{
    char* instName;
    char* techName;
    void* techInst;
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

    char* rotate_flag;

    int rowHeight;
    int numRows;
    int* rowIdxArray;

    int instIdx;
    struct INSTANCE* next;
}; typedef struct INSTANCE* instance_ptr;


struct INSTHASH{
    instance_ptr start;
}; typedef struct INSTHASH* instHash_ptr;


struct INSTDB{
    int numInst;
    int curNumInst;

    instHash_ptr hashTable;
    instance_ptr* instArray;
}; typedef struct INSTDB* instDB_ptr;


// List of functions
// Const/Dist functions
instance_ptr create_instance(instDB_ptr data, char* instName, char* techName, 
                             int pmin_x, int pmin_y, char* rotate_flag);
void destroy_instance(instance_ptr inst);
instDB_ptr create_instDB(void);
void destroy_instDB(instDB_ptr rmdb);

// Utility function
instance_ptr _get_instance(instDB_ptr data, char* instName);
void flip_instance_horizontal(instance_ptr inst);
void flip_instance_vertical(instance_ptr inst);
void rotate_instance(instance_ptr inst);

#endif