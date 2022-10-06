#ifndef _DATABASE_H
#define _DATABASE_H


#include "../utils.h"
#include "die.h"
#include "instance.h"
#include "net.h"
#include "tech.h"
#include "port.h"


enum state{
    state_site,         // 0
    state_libCell,      // 1
    state_libPin,       // 2
    state_dieSize,      // 3
    state_die,          // 4
    state_instDB,       // 5
    state_inst,         // 6
    state_netDB,        // 7
    state_net,          // 8
    state_netPin,       // 9
    state_portDB,       // 10
    state_port,         // 11
};


struct DATABASE{
    die_ptr die_data;
    instDB_ptr inst_data;
    netDB_ptr net_data;
    techDB_ptr tech_data;
    portDB_ptr port_data;
}; typedef struct DATABASE* database_ptr;


// List of functions
instance_ptr get_inst(database_ptr data, char* instName);
instance_ptr get_instWithIdx(database_ptr data, int idx);
pin_ptr get_instPin(database_ptr data, char* instname, char* pinName);
void inst_init(instance_ptr inst);
net_ptr get_net(database_ptr data, char* netName);
database_ptr create_database(void);
database_ptr database_init(char* filedir);
void destroy_database(database_ptr data);


void initHPWL(database_ptr data);
unsigned int getHPWL(database_ptr data);


#endif