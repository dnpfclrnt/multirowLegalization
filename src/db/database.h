#ifndef _DATABASE_H
#define _DATABASE_H


#include "die.h"
#include "instance.h"
#include "net.h"


enum state{
    state_libCell,    // 0
    state_libPin,     // 1
    state_die,        // 2
    state_instDB,     // 3
    state_inst,       // 4
    state_netDB,      // 5
    state_net,        // 6
    state_netPin      // 7
};


struct TECHPIN{
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


struct DATABASE{
    die_ptr die_data;
    instDB_ptr inst_data;
    netDB_ptr net_data;
    techDB_ptr tech_data;
}; typedef struct DATABASE* database_ptr;


// List of functions
techDB_ptr create_techDB(void);
techInst_ptr create_techInst(techDB_ptr tech_data, char* techName, int sizeX, int sizeY, int numPins);
techInst_ptr get_techInst(techDB_ptr tech_data, char* techName);
void destroy_techInst(techInst_ptr techInst);
void destroy_techDB(techDB_ptr rmdb);

instance_ptr update_instanceTech(techInst_ptr techInst);

database_ptr create_database(void);
database_ptr database_init(char* filedir);


#endif