#include "legality.h"


void placmenet_map_init(database_ptr data)
{
    int rowLen = data->die_data->rowLen;
    int numRows = data->die_data->repeatCount;

    unsigned int mem_used = rowLen * numRows * sizeof(instance_ptr);
    mem_used = mem_used / 1024 / 1024;
    printf("PROC: Placement Map Mem Util: %u MiB\n", mem_used);

    data->die_data->placementMap = (instance_ptr**)malloc(sizeof(instance_ptr*) * numRows);
    for (int i = 0; i < numRows; i++)
    {
        data->die_data->placementMap[i] = (instance_ptr*)calloc(sizeof(instance_ptr), rowLen);
        for (int j = 0; j < rowLen; j++)
        {
            data->die_data->placementMap[i][j] = NULL;
        }
    }
    printf("PROC: Placement Map initialized\n");
}


void reset_placementMap(database_ptr data)
{
    int rowLen = data->die_data->rowLen;
    int numRows = data->die_data->repeatCount;
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < rowLen; j++)
        {
            data->die_data->placementMap[i][j] = NULL;
        }
    }
}


bool check_legality_global(database_ptr data)
{
    reset_placementMap(data);
    printf("PROC: Start checking legality\n");
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = data->inst_data->instArray[i];
        int startRow = inst->pmin.y / inst->rowHeight;
        int endRow = inst->pmax.y / inst->rowHeight;
        for (int y = startRow; y < endRow; y++)
        {
            for (int x = inst->pmin.x; x < inst->pmax.x; x++)
            {
                if (data->die_data->placementMap[y][x] != NULL) 
                {
                    instance_ptr prev_inst = data->die_data->placementMap[y][x];
                    printf("PROC: Detected not legal zone\n");
                    printf("Inst %d & Inst %d overlaps @(%d, %d)\n", prev_inst->instIdx,
                                                                     inst->instIdx,
                                                                     x, y);
                    return false;
                }
                else data->die_data->placementMap[y][x] = inst;
            }
        }
    }
    printf("PROC: Passed Legality test\n");
    return true;
}