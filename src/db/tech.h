#ifndef _TECH_H
#define _TECH_H


// #include "../global.h"
#include "../utils.h"


struct TECHPIN{
    char* pinName;
    struct FPOS pinPos;
}; typedef struct TECHPIN* techPin_ptr;


struct TECHINST{
    char* techName;
    struct POS size;
    int curNumPins;
    int numPins;
    techPin_ptr techPinArray;
    struct TECHINST* next;
}; typedef struct TECHINST* techInst_ptr;


struct TECHHASHTOKEN{
    techInst_ptr start;
}; typedef struct TECHHASHTOKEN* techHashToken_ptr;


struct TECHDB{
    int curHashLen;
    techHashToken_ptr hashTable;
}; typedef struct TECHDB* techDB_ptr;


// List of functions
techDB_ptr create_techDB(void);
techInst_ptr create_techInst(techDB_ptr tech_data, char* techName, int sizeX, int sizeY, int numPins);
techInst_ptr get_techInst(techDB_ptr tech_data, char* techName);
void destroy_techInst(techInst_ptr techInst);
void destroy_techDB(techDB_ptr rmdb);


#endif