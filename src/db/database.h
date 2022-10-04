#ifndef _DATABASE_H
#define _DATABASE_H


#include "../utils.h"
#include "die.h"
#include "instance.h"
#include "net.h"
#include "tech.h"


enum state{
    state_libCell,    // 0
    state_libPin,     // 1
    state_dieSize,    // 2
    state_die,        // 3
    state_instDB,     // 4
    state_inst,       // 5
    state_netDB,      // 6
    state_net,        // 7
    state_netPin      // 8
};


struct DATABASE{
    die_ptr die_data;
    instDB_ptr inst_data;
    netDB_ptr net_data;
    techDB_ptr tech_data;
}; typedef struct DATABASE* database_ptr;


// List of functions
instance_ptr get_inst(database_ptr data, char* instName);
pin_ptr get_instPin(database_ptr data, char* instname, char* pinName);
void inst_init(instance_ptr inst);
net_ptr get_net(database_ptr data, char* netName);
database_ptr create_database(void);
database_ptr database_init(char* filedir);
void destroy_database(database_ptr data);


#endif