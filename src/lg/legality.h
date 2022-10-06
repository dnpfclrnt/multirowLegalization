#ifndef _LEGALITY_H
#define _LEGALITY_H


#include "../db/database.h"
#include "lgUtil.h"


// List of functions
void placmenet_map_init(database_ptr data);
void reset_placementMap(database_ptr data);
void lgInst_init(database_ptr data);
bool check_legality_local(database_ptr data, instance_ptr inst);
void fill_placementMap(database_ptr data, instance_ptr inst);
#ifdef PROTOTYPE
bool check_legality_global(database_ptr data, intArray_ptr arr);
#else
bool check_legality_global(database_ptr data);
#endif
int check_movable(database_ptr data, instance_ptr inst, int step, int mode);
void move_placementMap(database_ptr data, instance_ptr inst, struct POS start, struct POS dest);
void safe_move_inst(database_ptr data, instance_ptr inst, struct POS dest);


#endif