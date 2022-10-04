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


struct DATABASE{
    die_ptr die_data;
    instDB_ptr inst_data;
    netDB_ptr net_data;
    techDB_ptr tech_data;
}; typedef struct DATABASE* database_ptr;


// List of functions
instance_ptr update_instanceTech(techInst_ptr techInst);
database_ptr create_database(void);
database_ptr database_init(char* filedir);


#endif