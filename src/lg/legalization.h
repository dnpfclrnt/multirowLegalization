#ifndef _LEGALIZATION_H
#define _LEGALIZATION_H


#include "../db/database.h"
#include "legality.h"


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


int* sort_instByArea(database_ptr data);
sortDB_ptr create_sizeTable(database_ptr data, int* idxTokenArray);
sortDB_ptr sort_instByPmin(database_ptr data);
void legalize(database_ptr data, char* mode);

#endif