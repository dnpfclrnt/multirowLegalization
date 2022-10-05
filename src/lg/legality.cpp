#include "legality.h"


void placmenet_map_init(database_ptr data)
{
    int rowLen = data->die_data->rowLen;
    int numRows = data->die_data->repeatCount;

    unsigned int mem_used = rowLen * numRows * sizeof(bool);
    mem_used = mem_used / 1024;
    printf("PROC: Using = %u kB\n", mem_used);

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
}