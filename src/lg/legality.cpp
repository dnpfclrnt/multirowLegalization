#include "legality.h"


void placmenet_map_init(database_ptr data)
{
    int rowLen = data->die_data->rowLen;
    int numRows = data->die_data->repeatCount;

    data->die_data->placementMap = (bool**)malloc(sizeof(bool*) * numRows);
    for (int i = 0; i < numRows; i++)
    {
        data->die_data->placementMap[i] = (bool*)calloc(sizeof(bool), rowLen);
        for (int j = 0; j < rowLen; j++)
        {
            data->die_data->placementMap[i][j] = false;
        }
    }
    printf("PROC: Placement Map initialized\n");
    sleep(10);
}