#ifndef _DATABASE_H
#define _DATABASE_H


#include "die.h"
#include "instance.h"
#include "net.h"


struct DATABASE{
    die_ptr die_data;
    instDB_ptr inst_data;
    netDB_ptr net_data;
}; typedef struct DATABASE* database_ptr;


#endif