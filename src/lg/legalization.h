#ifndef _LEGALIZATION_H
#define _LEGALIZATION_H


#include "../db/database.h"
#include "legality.h"


int* sort_instByArea(database_ptr data);
void legalize(database_ptr data);

#endif