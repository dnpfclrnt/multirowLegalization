#include "die.h"


die_ptr create_die(int pmin_x, int pmin_y, int pmax_x, int pmax_y)
{
    die_ptr data = (die_ptr)malloc(sizeof(struct DIE));
    data->pmin.x = pmin_x;
    data->pmin.y = pmin_y;
    data->pmax.x = pmax_x;
    data->pmax.y = pmax_y;

    data->fpmax = posToFPOS(data->pmax);
    data->fpmin = posToFPOS(data->pmin);

    data->width = pmax_x - pmin_x;
    data->height = pmax_y - pmin_y;
    
    return data;
}


void update_dieRow(die_ptr die, int rowLen, int rowHeight, int repeatCount)
{
    die->rowLen = rowLen;
    die->rowHeight = rowHeight;
    die->repeatCount = repeatCount;
}



void destroy_die(die_ptr rmdb)
{
    if (rmdb->placementMap)
    {
        for (int i = 0; i < rmdb->repeatCount; i++)
        {
            free(rmdb->placementMap[i]);
        }
        free(rmdb->placementMap);
    }
    free(rmdb);
}