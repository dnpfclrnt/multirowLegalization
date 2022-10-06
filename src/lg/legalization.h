#ifndef _LEGALIZATION_H
#define _LEGALIZATION_H


#include "../db/database.h"
#include "legality.h"


// struct INSTMVTOKEN{
//     instance_ptr inst;
//     int move;
//     int direction;
// }; typedef struct INSTMVTOKEN* instMoveToken_ptr;


// struct MOVEDATA{
//     instMoveToken_ptr inst_arr;
//     int numInst;
//     int remainder;
// }; typedef struct MOVEDATA* moveData_ptr;


// struct ADJINST{
//     int* instArr;
//     int numInst;
// }; typedef struct ADJINST* adjInst_ptr;


int* sort_instByArea(database_ptr data);
sortDB_ptr create_sizeTable(database_ptr data, int* idxTokenArray);
sortDB_ptr sort_instByPmin(database_ptr data);
bool checkInstOvlp(instance_ptr inst1, instance_ptr inst2);
#ifdef PROTOTYPE
intArray_ptr legalize(database_ptr data, char* mode);
#else
void legalize(database_ptr data, char* mode);
#endif

#endif