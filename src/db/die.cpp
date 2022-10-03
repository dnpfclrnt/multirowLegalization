#include "die.h"


die_ptr create_die(int pmin_x, int pmin_y, int rowLen, int rowHeight, int repeatCount)
{
    die_ptr data = (die_ptr)malloc(sizeof(struct DIE));
    data->pmin.x = pmin_x;
    data->pmin.y = pmin_y;

    data->pmax.x = pmin_x + rowLen;
    data->pmax.y = pmin_y + rowHeight * repeatCount;

    data->fpmin = posToFPOS(data->pmin);
    data->fpmax = posToFPOS(data->pmax);

    data->rowHeight = rowHeight;
    data->repeatCount = repeatCount;

    data->placementMap = NULL;

    return data;
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