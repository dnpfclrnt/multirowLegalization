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
    int numRows = data->die_data->repeatCount;
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
        struct POS pmin;
        if (inst->pmax.x > dieWidth)
        {
            pmin.x = dieWidth - inst->size.x;
            pmin.y = inst->pmin.y;
            inst->right_pushable = false;
            place_inst(inst, pmin);
        }
        if (inst->pmax.y > dieHeight)
        {
            pmin.x = inst->pmin.x;
            pmin.y = dieHeight - inst->size.y;
            inst->up_pushable = false;
            place_inst(inst, pmin);
        }
        if (inst->pmin.x < data->die_data->pmin.x)
        {
            pmin.x = data->die_data->pmin.x;
            pmin.y = inst->pmin.y;
            inst->left_pushable = false;
            place_inst(inst, pmin);
        }
        if (inst->pmin.y < data->die_data->pmin.y)
        {
            pmin.x = inst->pmin.x;
            pmin.y = data->die_data->pmin.y;
            inst->down_pushable = false;
            place_inst(inst, pmin);
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
    if (endCol > data->die_data->pmax.x / data->die_data->site.x ||
        endRow > data->die_data->pmax.y / data->die_data->site.y)
    {
        fprintf(stderr, "Instance %s (%d, %d) (%d/%d, %d/%d)Out of index\n", inst->instName, inst->pmin.x, inst->pmin.y,
                                                                    endCol, data->die_data->pmax.x / data->die_data->site.x, 
                                                                    endRow, data->die_data->pmax.y / data->die_data->site.y);
        exit(EXIT_FAILURE);
    }
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


#ifdef PROTOTYPE
bool check_legality_global(database_ptr data, intArray_ptr arr)
#else
bool check_legality_global(database_ptr data)
#endif
{
    reset_placementMap(data);
    printf("PROC: Start checking legality\n");
    struct timeval start_lg_check;
    gettimeofday(&start_lg_check, NULL);
    int notPlaced = 0;
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = data->inst_data->instArray[i];
        int startRow = inst->pmin.y / data->die_data->site.y;
        int endRow   = inst->pmax.y / data->die_data->site.y;

        int startCol = inst->pmin.x / data->die_data->site.x;
        int endCol   = inst->pmax.x / data->die_data->site.x;
        #ifdef PROTOTYPE
        bool check = false;
        for (int j = 0; j < arr->len; j++)
        {
            if (arr->intArray[j] == i)
            {
                check = true;
                break;
            }
        }
        if (check)
        {
            notPlaced++;
            continue;
        }
        #endif
        bool checkLocal = check_legality_local(data, inst);
        if (checkLocal == false) 
        {
            // fprintf(stderr, "PROC: Failed to filter\n");
            notPlaced++;
            continue;
        }
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
                    // printf("\nPROC: Detected not legal zone\n\n");
                    break;
                    #endif
                }
                else data->die_data->placementMap[y][x] = inst;
            }
        }
    }
    struct timeval end_lg_check;
    gettimeofday(&end_lg_check, NULL);
    double check_time = stopwatch(&start_lg_check, &end_lg_check);
    // printf("=======================================\n");
    // printf("Legality check time = %3lf\n", check_time);
    // printf("=======================================\n");
    if (notPlaced > 0) 
    {
        printf("PROC: Not placed = %d\n", notPlaced);
        return false;
    }
    printf("PROC: Passed Legality test\n");
    return true;
}


int check_movable(database_ptr data, instance_ptr inst, int step, int mode)
{
    using namespace std;
    struct POS gridSize = data->die_data->site;
    struct POS maxDieGrid;
    maxDieGrid.x = data->die_data->pmax.x / gridSize.x;
    maxDieGrid.y = data->die_data->pmax.y / gridSize.y;
    struct POS gridStart;
    struct POS gridEnd;
    int movable = 0;
    if (mode == mode_left)
    {
        gridStart.x = inst->pmin.x / gridSize.x;
        gridStart.y = inst->pmin.y / gridSize.y;
        gridEnd.x = max(0, gridStart.x - step);
        gridEnd.y = inst->pmax.y / gridSize.y;
        for (int x = gridStart.x; x > gridEnd.x; x--)
        {
            for (int y = gridStart.y; y < gridEnd.y; y++)
            {
                if (data->die_data->placementMap[y][x]) return movable;
            }
            movable++;
        }
    }
    else if (mode == mode_right)
    {
        gridStart.x = inst->pmax.x / gridSize.x;
        gridStart.y = inst->pmin.y / gridSize.y;
        gridEnd.x = min(gridStart.x + step, maxDieGrid.x);
        gridEnd.y = inst->pmax.y / gridSize.y;
        for (int x = gridStart.x; x < gridEnd.x; x++)
        {
            for (int y = gridStart.y; y < gridEnd.y; y++)
            {
                if (data->die_data->placementMap[y][x]) return movable;
            }
            movable++;
        }
    }
    else if (mode == mode_down)
    {
        gridStart.x = inst->pmin.x / gridSize.x;
        gridStart.y = inst->pmin.y / gridSize.y;
        gridEnd.x = inst->pmax.x / gridSize.x;
        gridEnd.y = max(0, gridStart.y - step);
        for (int y = gridStart.y; y > gridEnd.y; y--)
        {
            for (int x = gridStart.x; x < gridEnd.x; x++)
            {
                if (data->die_data->placementMap[y][x]) return movable;
            }
        }
    }
    else
    {
        gridStart.x = inst->pmin.x / gridSize.x;
        gridStart.y = inst->pmax.y / gridSize.y;
        gridEnd.x = inst->pmax.x / gridSize.x;
        gridEnd.y = min(gridStart.y + step, maxDieGrid.y);
        for (int y = gridStart.y; y < gridEnd.y; y++)
        {
            for (int x= gridStart.x; x < gridEnd.x; x++)
            {
                if (data->die_data->placementMap[y][x]) return movable;
            }
        }
    }
    return movable;
}


void move_placementMap(database_ptr data, instance_ptr inst, struct POS start, struct POS dest)
{
    struct POS gridSize = data->die_data->site;
    struct POS start_grid;
    struct POS dest_grid;
    struct POS inst_grid;
    start_grid.x = start.x / gridSize.x;
    start_grid.y = start.y / gridSize.y;
    dest_grid.x = dest.x / gridSize.x;
    dest_grid.y = dest.y / gridSize.y;
    inst_grid.x = inst->size.x / gridSize.x;
    inst_grid.y = inst->size.y / gridSize.y;

    for (int x = start_grid.x; x < start_grid.x + inst_grid.x; x++)
    {
        for (int y = start_grid.y; y < start_grid.y + inst_grid.y; y++)
        {
            data->die_data->placementMap[y][x] = NULL;
        }
    }
    for (int x = dest_grid.x; x < dest_grid.x + inst_grid.x; x++)
    {
        for (int y = dest_grid.y; y < dest_grid.y + inst_grid.y; y++)
        {
            data->die_data->placementMap[y][x] = inst;
        }
    }
}


void safe_move_inst(database_ptr data, instance_ptr inst, struct POS dest)
{
    int dieWidth = data->die_data->pmax.x;
    int dieHeight = data->die_data->pmax.y;
    struct POS pmin;
    struct POS prevPmin = inst->pmin;
    if (inst->pmax.x > dieWidth)
    {
        pmin.x = dieWidth - inst->size.x;
        pmin.y = inst->pmin.y;
        inst->right_pushable = false;
        place_inst(inst, pmin);
    }
    if (inst->pmax.y > dieHeight)
    {
        pmin.x = inst->pmin.x;
        pmin.y = dieHeight - inst->size.y;
        inst->up_pushable = false;
        place_inst(inst, pmin);
    }
    if (inst->pmin.x < data->die_data->pmin.x)
    {
        pmin.x = data->die_data->pmin.x;
        pmin.y = inst->pmin.y;
        inst->left_pushable = false;
        place_inst(inst, pmin);
    }
    if (inst->pmin.y < data->die_data->pmin.y)
    {
        pmin.x = inst->pmin.x;
        pmin.y = data->die_data->pmin.y;
        inst->down_pushable = false;
        place_inst(inst, pmin);
    }
    if (prevPmin.x != inst->pmin.x) printf("Moved\n");
    else if (prevPmin.y != inst->pmin.y) printf("Moved\n");
}
