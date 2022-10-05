#include "legality.h"


void placmenet_map_init(database_ptr data)
{
    int rowLen = data->die_data->rowLen / data->die_data->site.x;
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
    int rowLen = data->die_data->rowLen / data->die_data->site.x;
    int numRows = data->die_data->repeatCount / data->die_data->site.y;
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < rowLen; j++)
        {
            data->die_data->placementMap[i][j] = NULL;
        }
    }
}


void lgInst_init(database_ptr data)
{
    printf("PROC: Start LG Init\n");
    int dieWidth = data->die_data->pmax.x;
    int dieHeight = data->die_data->pmax.y;
    if (data->inst_data->initialPOS == NULL)
    {
        data->inst_data->initialPOS = (struct POS*)calloc(sizeof(struct POS), data->inst_data->numInst);
    }
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = data->inst_data->instArray[i];
        data->inst_data->initialPOS[i] = inst->pmin;
        if (inst->pmax.x > dieWidth || inst->pmax.y > dieHeight)
        {
            place_inst(inst, subtractPOS(data->die_data->pmax, inst->size));
        }
        if (inst->pmin.x < data->die_data->pmin.x || inst->pmin.y < data->die_data->pmin.y)
        {
            place_inst(inst, setPOS(data->die_data->pmin, inst->size));
        }
    }
    printf("PROC: Finished LG INIT\n");
}


bool check_legality_local(database_ptr data, instance_ptr inst)
{
    int startRow = inst->pmin.y / data->die_data->site.y;
    int endRow   = inst->pmax.y / data->die_data->site.y;

    int startCol = inst->pmin.x / data->die_data->site.x;
    int endCol   = inst->pmax.x / data->die_data->site.x;
    for (int y = startRow; y < endRow; y++)
    {
        for (int x = startCol; x < endCol; x++)
        {
            if (data->die_data->placementMap[y][x] != NULL) return false;
        }
    }
    return true;
}


void fill_placementMap(database_ptr data, instance_ptr inst)
{
    int startRow = inst->pmin.y / data->die_data->site.y;
    int endRow   = inst->pmax.y / data->die_data->site.y;

    int startCol = inst->pmin.x / data->die_data->site.x;
    int endCol   = inst->pmax.x / data->die_data->site.x;
    for (int y = startRow; y < endRow; y++)
    {
        for (int x = startCol; x < endCol; x++)
        {
            data->die_data->placementMap[y][x] = inst;
        }
    }

}


bool check_legality_global(database_ptr data)
{
    reset_placementMap(data);
    printf("PROC: Start checking legality\n");
    struct timeval start_lg_check;
    gettimeofday(&start_lg_check, NULL);
    int DRV_CNT = 0;
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = data->inst_data->instArray[i];
        int startRow = inst->pmin.y / data->die_data->site.y;
        int endRow   = inst->pmax.y / data->die_data->site.y;

        int startCol = inst->pmin.x / data->die_data->site.x;
        int endCol   = inst->pmax.x / data->die_data->site.x;
        for (int y = startRow; y < endRow; y++)
        {
            for (int x = startCol; x < endCol; x++)
            {
                if (data->die_data->placementMap[y][x] != NULL) 
                {
                    #if DRV_VISUAL == true
                    instance_ptr prev_inst = data->die_data->placementMap[y][x];
                    if (DRV_CNT++ == 0) printf("PROC: Detected not legal zone\n");
                    printf("Inst %d & Inst %d overlaps @(%d, %d)\n", prev_inst->instIdx,
                                                                     inst->instIdx,
                                                                     x, y);
                    #else
                    printf("\nPROC: Detected not legal zone\n\n");
                    return false;
                    #endif
                }
                else data->die_data->placementMap[y][x] = inst;
            }
        }
    }
    struct timeval end_lg_check;
    gettimeofday(&end_lg_check, NULL);
    double check_time = stopwatch(&start_lg_check, &end_lg_check);
    printf("=======================================\n");
    printf("Legality check time = %3lf\n", check_time);
    printf("=======================================\n");
    printf("PROC: Passed Legality test\n");
    if (DRV_CNT > 0) return false;
    return true;
}