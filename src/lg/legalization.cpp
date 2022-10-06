#include "legalization.h"


bool checkInstOvlp(instance_ptr inst1, instance_ptr inst2)
{
    if (inst1->pmax.x > inst2->pmin.x) 
    {
        if (inst1->pmin.x < inst2->pmax.x) return true;
    }
    if (inst2->pmax.x > inst1->pmin.x)
    {
        if (inst2->pmin.x < inst1->pmax.x) return true;
    }
    if (inst1->pmax.y > inst2->pmin.y)
    {
        if (inst1->pmin.y < inst2->pmax.y) return true;
    }
    if (inst2->pmax.y > inst1->pmin.y)
    {
        if (inst2->pmin.y < inst2->pmax.y) return true;
    }

    return false;
}


ovlpMap_ptr create_ovlpMap(database_ptr data, instance_ptr inst)
{
    using namespace std;
    struct POS gridSize = data->die_data->site;
    ovlp_ptr overlap = NULL;
    struct POS minGrid;
    struct POS maxGrid;
    struct POS sweepGrid;
    minGrid.x = inst->pmin.x / gridSize.x;
    minGrid.y = inst->pmin.y / gridSize.y;
    maxGrid.x = inst->pmax.x / gridSize.x;
    maxGrid.y = inst->pmax.y / gridSize.y;
    sweepGrid.x = sweepGrid.y = 0;
    int overlapLen = 0;

    for (int x = minGrid.x; x < maxGrid.x; x++)
    {
        for (int y = minGrid.y; y < maxGrid.y; y++)
        {
            if (data->die_data->placementMap[y][x] != NULL)
            {
                instance_ptr prevInst = data->die_data->placementMap[y][x];
                int prevInstFlag = checkInstPrevOvlp(overlap, overlapLen, prevInst);
                if (prevInstFlag == -1)
                {
                    // Case1 First time to declare overlap inst
                    overlap = (ovlp_ptr)malloc(sizeof(struct OVLP));
                    overlap[0].ovlpInst = prevInst;
                    overlap[0].abs_ovlpGrid.pmax.x = overlap[0].abs_ovlpGrid.pmax.y = -1;
                    overlap[0].abs_ovlpGrid.pmin.x = overlap[0].abs_ovlpGrid.pmin.y = 1<<30;
                    overlap[0].abs_ovlpGrid.pmax.x = max(overlap[0].abs_ovlpGrid.pmax.x, x);
                    overlap[0].abs_ovlpGrid.pmax.y = max(overlap[0].abs_ovlpGrid.pmax.y, y);
                    overlap[0].abs_ovlpGrid.pmin.x = min(overlap[0].abs_ovlpGrid.pmin.x, x);
                    overlap[0].abs_ovlpGrid.pmin.y = min(overlap[0].abs_ovlpGrid.pmin.y, y);
                    overlapLen = 1;
                }
                else if (prevInstFlag == -2)
                {
                    // Case2 This instance was not searched previously
                    overlapLen++;
                    int ovIdx = overlapLen - 1;
                    overlap = (ovlp_ptr)realloc(overlap, sizeof(struct OVLP) * overlapLen);
                    overlap[ovIdx].ovlpInst = prevInst;
                    overlap[ovIdx].abs_ovlpGrid.pmax.x = overlap[ovIdx].abs_ovlpGrid.pmax.y = -1;
                    overlap[ovIdx].abs_ovlpGrid.pmin.x = overlap[ovIdx].abs_ovlpGrid.pmin.y = 1<<30;
                    overlap[ovIdx].abs_ovlpGrid.pmax.x = max(overlap[ovIdx].abs_ovlpGrid.pmax.x, x);
                    overlap[ovIdx].abs_ovlpGrid.pmax.y = max(overlap[ovIdx].abs_ovlpGrid.pmax.y, y);
                    overlap[ovIdx].abs_ovlpGrid.pmin.x = min(overlap[ovIdx].abs_ovlpGrid.pmin.x, x);
                    overlap[ovIdx].abs_ovlpGrid.pmin.y = min(overlap[ovIdx].abs_ovlpGrid.pmin.y, y);
                }
                else
                {
                    // Case3 Flag has prev index. 
                    int ovIdx = prevInstFlag;
                    overlap[ovIdx].abs_ovlpGrid.pmax.x = max(overlap[ovIdx].abs_ovlpGrid.pmax.x, x);
                    overlap[ovIdx].abs_ovlpGrid.pmax.y = max(overlap[ovIdx].abs_ovlpGrid.pmax.y, y);
                    overlap[ovIdx].abs_ovlpGrid.pmin.x = min(overlap[ovIdx].abs_ovlpGrid.pmin.x, x);
                    overlap[ovIdx].abs_ovlpGrid.pmin.y = min(overlap[ovIdx].abs_ovlpGrid.pmin.y, y);
                }
            }
        }
    }
    ovlpMap_ptr map = (ovlpMap_ptr)malloc(sizeof(struct OVLPMAP));
    map->ovlp_array = overlap;
    map->numOvlp = overlapLen;
    return map;
}


void moveLeftPossible(database_ptr data, instance_ptr inst, int leftEnd)
{
    struct POS gridSize = data->die_data->site;
    struct POS start;
    struct POS end;
    start.x = inst->pmin.x / gridSize.x;
    start.y = inst->pmin.y / gridSize.y;
    end.x = start.x;
    end.y = inst->pmax.y / gridSize.y;
    int steps = 0;
    bool cont = true;
    while(cont)
    {
        for (int i = start.y; i < end.y; i++)
        {
            if (data->die_data->placementMap[i][start.x - steps - 1] ||
                start.x - steps > leftEnd) 
            {
                cont = false;
                break;
            }
        }
        steps++;
    }
    if (steps > 0)
    {
        struct POS dest = inst->pmin;
        inst->leftInterval = steps;
        dest.x -= steps;
        move_placementMap(data, inst, inst->pmin, dest);
        safe_move_inst(data, inst, dest);
    }
}


void moveRightPossible(database_ptr data, instance_ptr inst, int rightEnd)
{
    struct POS gridSize = data->die_data->site;
    struct POS start;
    struct POS end;
    end.x = inst->pmax.x / gridSize.x;
    end.y = inst->pmax.y / gridSize.y;
    start.x = end.x;
    start.y = inst->pmin.y / gridSize.y;
    int steps = 0;
    bool cont = true;
    while(cont)
    {
        for (int i = start.y; i < end.y; i++)
        {
            if (data->die_data->placementMap[i][start.x + steps + 1] ||
                start.x + steps < rightEnd) 
            {
                cont = false;
                break;
            }
        }
        steps++;
    }
    if (steps > 0)
    {
        struct POS dest = inst->pmin;
        inst->rightInterval = steps;
        dest.x += steps;
        move_placementMap(data, inst, inst->pmin, dest);
        safe_move_inst(data, inst, dest);
    }   
}

#ifdef PROTOTYPE
intArray_ptr legalize(database_ptr data, char* mode)
#else
void legalize(database_ptr data, char* mode)
#endif
{
    using namespace std;
    printf("PROC: Start Legalization\n");
    struct timeval start;
    gettimeofday(&start, NULL);
    lgInst_init(data);
    int* sortedInst = sort_instByArea(data);
    sortDB_ptr sortXY;
    if (!strcmp(mode, "SIZE"))   
    {
        printf("PROC: Sort Size MODE\n");
        // int* sortedInst = sort_instByArea(data);
        sortXY = create_sizeTable(data, sortedInst);
    }
    else
    {
        printf("PROC: Sort Coordinate MODE\n");
        sortXY = create_sizeTable(data, sortedInst);
        sortXY->idxArray_x = _sort_instByPmin(data, sortXY->idxArray_x, 0, data->inst_data->numInst, mode_x);
        sortXY->idxArray_y = _sort_instByPmin(data, sortXY->idxArray_y, 0, data->inst_data->numInst, mode_y);
        // sortXY = sort_instByPmin(data);
    }
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst_x, inst_y;
        int index_X = sortXY->idxArray_x[i];
        int index_Y = sortXY->idxArray_y[i];
        inst_x = data->inst_data->instArray[index_X];
        inst_y = data->inst_data->instArray[index_Y];
        inst_x->sortedIdx.x = i;
        inst_y->sortedIdx.y = i;
    }
    
    struct POS dieGridDim;
    struct POS gridSize;

    dieGridDim.x = data->die_data->pmax.x / data->die_data->site.x;
    dieGridDim.y = data->die_data->repeatCount;

    gridSize.x = data->die_data->site.x;
    gridSize.y = data->die_data->site.y;
    int failCnt = 0;

    bool* placed = (bool*)malloc(sizeof(bool) * data->inst_data->numInst);
    int notPlaced = data->inst_data->numInst;
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        placed[i] = false;
    }

    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = get_instWithIdx(data, sortXY->idxArray_x[i]);
        struct POS maxPos = subtractPOS(data->die_data->pmax, inst->size);

        // Get initial candidates
        int displacements[2][2];
        displacements[0][0] = displacements[0][1] = 0;
        displacements[1][0] = displacements[1][1] = 0;

        displacements[0][0] += inst->pmin.x - (inst->pmin.x / gridSize.x) * gridSize.x;
        displacements[0][0] += inst->pmin.y - (inst->pmin.y / gridSize.y) * gridSize.y;

        displacements[0][1] += min(maxPos.x, ((inst->pmin.x / gridSize.x) + 1) * gridSize.x) - inst->pmin.x;
        displacements[0][1] += inst->pmin.y - (inst->pmin.y / gridSize.y) * gridSize.y;

        displacements[1][0] += inst->pmin.x - (inst->pmin.x / gridSize.x) * gridSize.x;
        displacements[1][0] += min(maxPos.y, ((inst->pmin.y / gridSize.y) + 1) * gridSize.y) - inst->pmin.y;

        displacements[1][1] += min(maxPos.x, ((inst->pmin.x / gridSize.x) + 1) * gridSize.x) - inst->pmin.x;
        displacements[1][1] += min(maxPos.y, ((inst->pmin.y / gridSize.y) + 1) * gridSize.y) - inst->pmin.y;
        
        int disp_rank[4];
        int disp_list[4];

        for (int a = 0; a < 4; a++)
        {
            disp_rank[a] = a;
            disp_list[a] = displacements[a / 2][a % 2];
        }
        for (int a = 0; a < 3; a++)
        {
            for (int b = 0; b < 3 - a; b++)
            {
                if (disp_list[b] > disp_list[b + 1])
                {
                    int temp = disp_rank[b + 1];
                    disp_rank[b + 1] = disp_rank[b];
                    disp_rank[b] = temp;
                    int temp_disp = disp_list[b + 1];
                    disp_list[b + 1] = disp_list[b];
                    disp_list[b] = temp_disp;
                }
            }
        }

        bool immPlaced = false;
        for (int rank = 0; rank < 4; rank++)
        {
            if (disp_rank[rank] == 0)
            {
                inst->pmin.x = (inst->pmin.x / gridSize.x) * gridSize.x;
                inst->pmin.y = (inst->pmin.y / gridSize.y) * gridSize.y;
            }
            else if (disp_rank[rank] == 1)
            {
                inst->pmin.x = min(maxPos.x, ((inst->pmin.x / gridSize.x) + 1) * gridSize.x);
                inst->pmin.y = (inst->pmin.y / gridSize.y) * gridSize.y;
            }
            else if (disp_rank[rank] == 2)
            {
                inst->pmin.x = (inst->pmin.x / gridSize.x) * gridSize.x;
                inst->pmin.y = min(maxPos.y, ((inst->pmin.y / gridSize.y) + 1) * gridSize.y);
            }
            else
            {
                inst->pmin.x = min(maxPos.x, ((inst->pmin.x / gridSize.x) + 1) * gridSize.x);
                inst->pmin.y = min(maxPos.y, ((inst->pmin.y / gridSize.y) + 1) * gridSize.y);
            }
            safe_move_inst(data, inst, inst->pmin);
            // place_inst(inst, inst->pmin);
            immPlaced = check_legality_local(data, inst);
            if (immPlaced == true) 
            {
                fill_placementMap(data, inst);
                break;
            }
        }
        if (immPlaced == true) 
        {
            placed[i] = true;
            notPlaced--;
            // printf("Placed %d/%d Cells\n", i - failCnt, data->inst_data->numInst);
            continue;
        }
        else failCnt++;
        
        if (disp_rank[0] == 0)
        {
            inst->pmin.x = (inst->pmin.x / gridSize.x) * gridSize.x;
            inst->pmin.y = (inst->pmin.y / gridSize.y) * gridSize.y;
        }
        else if (disp_rank[0] == 1)
        {
            inst->pmin.x = min(maxPos.x, ((inst->pmin.x / gridSize.x) + 1) * gridSize.x);
            inst->pmin.y = (inst->pmin.y / gridSize.y) * gridSize.y;
        }
        else if (disp_rank[0] == 2)
        {
            inst->pmin.x = (inst->pmin.x / gridSize.x) * gridSize.x;
            inst->pmin.y = min(maxPos.y, ((inst->pmin.y / gridSize.y) + 1) * gridSize.y);
        }
        else
        {
            inst->pmin.x = min(maxPos.x, ((inst->pmin.x / gridSize.x) + 1) * gridSize.x);
            inst->pmin.y = min(maxPos.y, ((inst->pmin.y / gridSize.y) + 1) * gridSize.y);
        }
        safe_move_inst(data, inst, inst->pmin);
        // place_inst(inst, inst->pmin);
    }
    printf("PROC: Placed %d/%d\n", data->inst_data->numInst - notPlaced, data->inst_data->numInst);
    // Initial legalization finished.
    int numPlace = data->inst_data->numInst;
    int R_x = 30;
    int R_y = 5;
    int k = 1;
    while(notPlaced > 0)
    {
        printf("\nNot placed %d\n", notPlaced);
        int* notPlacedIdx_arr = (int*)malloc(sizeof(int) * notPlaced);
        cellArea maxArea = 0;
        int cnt = 0;
        for (int i = 0; i < numPlace; i++)
        {
            if (placed[i] == false)
            {
                maxArea = max(data->inst_data->instArray[i]->area, maxArea);
                notPlacedIdx_arr[cnt++] = i;
            }
        }
        #ifdef PROTOTYPE
        free(placed);
        intArray_ptr arr = (intArray_ptr)malloc(sizeof(struct INTARRAY));
        arr->intArray = notPlacedIdx_arr;
        arr->len = notPlaced;
        return arr;
        #endif
        free(placed);
        placed = (bool*)malloc(sizeof(bool) * notPlaced);
        for (int i = 0; i < notPlaced; i++)
        {
            placed[i] = false;
        }

        cellArea total_case = 0;
        for (int i = 0; i < notPlaced; i++)
        {
            instance_ptr inst = get_instWithIdx(data, notPlacedIdx_arr[i]);
            struct BBOX window;
            struct BBOX windowGrid;
            window.pmax.x = min(inst->pmax.x + gridSize.x * R_x * k, data->die_data->pmax.x);
            window.pmax.y = min(inst->pmax.y + gridSize.y * R_y * k,  data->die_data->pmax.y);
            window.pmin.x = max(inst->pmin.x - gridSize.x * R_x * k, data->die_data->pmin.x);
            window.pmin.y = max(inst->pmin.y - gridSize.x * R_y * k, data->die_data->pmin.y);
            if (window.pmax.x == data->die_data->pmax.x &&
                window.pmax.y == data->die_data->pmax.y &&
                window.pmin.x == data->die_data->pmin.x &&
                window.pmin.y == data->die_data->pmin.y)
            {
                free(notPlacedIdx_arr);
                free(placed);
                free(sortedInst);
                fprintf(stderr, "Failed to place all inst\n");
                exit(EXIT_FAILURE);
            }
            // printf("Window (%d, %d) (%d, %d)\n", window.pmin.x, window.pmin.y, window.pmax.x, window.pmax.y);
            windowGrid.pmax.x = window.pmax.x / gridSize.x;
            windowGrid.pmax.y = window.pmax.y / gridSize.y;
            windowGrid.pmin.x = window.pmin.x / gridSize.x;
            windowGrid.pmin.y = window.pmin.y / gridSize.y;
            // int* windowInstIdx = (int*)malloc(sizeof(int));
            // windowInstIdx[0] = 0;
            int* windowInstIdx = NULL;
            int numInst = 0;
            // Window construction
            for (int x = windowGrid.pmin.x; x < windowGrid.pmax.x; x++)
            {
                for (int y = windowGrid.pmin.y; y < windowGrid.pmax.y; y++)
                {
                    if (data->die_data->placementMap[y][x] != NULL)
                    {
                        bool isSkip = false;
                        for (int check = 0; check < numInst; check++)
                        {
                            if (windowInstIdx[check] == data->die_data->placementMap[y][x]->instIdx) 
                            {
                                // printf("%d %d\n", windowInstIdx[i], data->die_data->placementMap[y][x]->instIdx);
                                isSkip = true;
                                break;
                            }
                        }
                        if (isSkip) continue;
                        if (data->die_data->placementMap[y][x]->pmax.x <= window.pmax.x &&
                            data->die_data->placementMap[y][x]->pmax.y <= window.pmax.y &&
                            data->die_data->placementMap[y][x]->pmin.x >= window.pmin.x &&
                            data->die_data->placementMap[y][x]->pmin.y >= window.pmin.y)
                        {
                            if (numInst == 0) 
                            {
                                windowInstIdx = (int*)malloc(sizeof(int));
                                windowInstIdx[0] = data->die_data->placementMap[y][x]->instIdx;
                                numInst = 1;
                            }
                            else
                            {
                                numInst++;
                                windowInstIdx = (int*)realloc(windowInstIdx, numInst * sizeof(int));
                                windowInstIdx[numInst - 1] = data->die_data->placementMap[y][x]->instIdx;
                            }
                        }
                    }
                }
            }
            // interval calculation
            if (numInst > 0)
            {
                // printf("%d\n", numInst);
                windowInstIdx = _sort_instByPmin(data, windowInstIdx, 0, numInst, mode_x);
                struct POS* originalPos = (struct POS*)malloc(sizeof(struct POS) * numInst);
                for (int idx = 0; idx < numInst; idx++)
                {
                    instance_ptr pushInst = get_instWithIdx(data, windowInstIdx[idx]);
                    originalPos[idx] = pushInst->pmin;
                }
                for (int idx = 0; idx < numInst; idx++)
                {
                    instance_ptr pushInst = get_instWithIdx(data, windowInstIdx[idx]);
                    moveLeftPossible(data, pushInst, windowGrid.pmin.x);
                }
                for (int idx = 0; idx < numInst; idx++)
                {
                    instance_ptr pushInst = get_instWithIdx(data, windowInstIdx[idx]);
                    move_placementMap(data, pushInst, pushInst->pmin, originalPos[idx]);
                    safe_move_inst(data, pushInst, originalPos[idx]);
                }
                int possible_cases = 1;
                for (int idx = numInst -1; idx >= 0; idx--)
                {
                    instance_ptr pushInst = get_instWithIdx(data, windowInstIdx[idx]);
                    moveRightPossible(data, pushInst, windowGrid.pmax.x);
                    possible_cases *= (pushInst->left_pushable + pushInst->right_pushable + 1);
                }
                // printf("%d Possible cases\n", possible_cases);
                total_case += possible_cases;
                for (int idx = numInst -1; idx >= 0; idx--)
                {
                    instance_ptr pushInst = get_instWithIdx(data, windowInstIdx[idx]);
                    move_placementMap(data, pushInst, pushInst->pmin, originalPos[idx]);
                    safe_move_inst(data, pushInst, originalPos[idx]);
                }
                ovlpMap_ptr map = create_ovlpMap(data, inst);
                for (int ovIdx = 0; ovIdx < map->numOvlp; ovIdx++)
                {
                    instance_ptr ovlpInst = map->ovlp_array[ovIdx].ovlpInst;
                    bool isInWindow = false;
                    for (int a = 0; a < numInst; a++)
                    {
                        if (windowInstIdx[a] == ovlpInst->instIdx) isInWindow = true;
                    }
                    if (isInWindow == false) break;
                    if (ovlpInst->pmax.x - ovlpInst->leftInterval * gridSize.x > inst->pmin.x &&
                        ovlpInst->pmin.x + ovlpInst->rightInterval * gridSize.x < inst->pmax.x)
                    {
                        break;
                    }
                    // else printf("great\n");
                }
                free(map->ovlp_array);
                free(map);
            }
            free(windowInstIdx);
        }
        printf("Total cases = %llu\n", total_case);

        numPlace = notPlaced;
        free(notPlacedIdx_arr);
        if (notPlaced > 0) break;
        k++;
    }
    free(placed);
    free(sortedInst);
    struct timeval end;
    gettimeofday(&end, NULL);
    double runtime = stopwatch(&start, &end);
    printf("=======================================\n");
    printf("Legalization time : %3lf sec\n", runtime);
    printf("=======================================\n");
    
    printf("PROC: Finished Legalization\n");
}