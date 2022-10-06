#ifndef _LG_UTIL_H
#define _LG_UTIL_H


// #include "../global.h"
#include "../utils.h"
#include "../db/database.h"


struct SIZETOKEN{
    cellArea size;
    int arrayLen;
    int* idx_array;
}; typedef struct SIZETOKEN* sizeToken_ptr;


struct SIZEDB{
    sizeToken_ptr sizeTableX;
    sizeToken_ptr sizeTableY;
    int numSizes;
}; typedef struct SIZEDB* sizeDB_ptr;


struct SORTDB{
    int* idxArray_x;
    int* idxArray_y;
}; typedef struct SORTDB* sortDB_ptr;


struct OVLP{
    instance_ptr ovlpInst;
    struct BBOX ovlpGrid;
    struct BBOX abs_ovlpGrid;
}; typedef struct OVLP* ovlp_ptr;


struct OVLPMAP{
    ovlp_ptr ovlp_array;
    int numOvlp;
}; typedef struct OVLPMAP* ovlpMap_ptr;


// List of functions
int* _sort_instByArea(database_ptr data, int* idxArray, int start, int end);
int* _sort_instByPmin(database_ptr data, int* idxArray, int start, int end, int mode);
int* sort_instByArea(database_ptr data);
sortDB_ptr sort_instByPmin(database_ptr data);
sortDB_ptr create_sizeTable(database_ptr data, int* idxTokenArray);
int checkInstPrevOvlp(ovlp_ptr ovlp_array, int arrayLen, instance_ptr inst);


#endif