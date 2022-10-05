#ifndef _LEGALITY_H
#define _LEGALITY_H


#include "../db/database.h"


// List of functions
void placmenet_map_init(database_ptr data);
void reset_placementMap(database_ptr data);
void lgInst_init(database_ptr data);
bool check_legality_local(database_ptr data, instance_ptr inst);
void fill_placementMap(database_ptr data, instance_ptr inst);
bool check_legality_global(database_ptr data);


#endif