#include "legalization.h"


int* _sort_instByArea(database_ptr data, int* idxArray, int start, int end)
{
    int len = end - start;
    // printf("Recurse %d %d\n", start, end);
    if (len < 2) return idxArray;
    unsigned long long pivot = get_instWithIdx(data, idxArray[start])->area;

    int sweep = start;
    int curEnd = end;
    while(sweep != curEnd)
    {
        if (pivot < get_instWithIdx(data, idxArray[sweep + 1])->area)
        {
            int temp = idxArray[sweep + 1];
            idxArray[sweep + 1] = idxArray[sweep];
            idxArray[sweep] = temp;
            sweep++;
        }
        else
        {
            int temp = idxArray[curEnd];
            idxArray[curEnd] = idxArray[sweep + 1];
            idxArray[sweep + 1] = temp;
            curEnd--;
        }
    }
    if (get_instWithIdx(data, idxArray[sweep])->area != pivot)
    {
        fprintf(stderr, "Lost Pivot!\n");
        exit(EXIT_FAILURE);
    }
    idxArray = _sort_instByArea(data, idxArray, start, sweep - 1);
    idxArray = _sort_instByArea(data, idxArray, sweep + 1, end);
    return idxArray;
}


int* _sort_instByPmin(database_ptr data, int* idxArray, int start, int end, int mode)
{
    // Mode == 0 sort x
    // Mode == 1 sort y
    int len = end - start;
    // printf("Recurse %d %d\n", start, end);
    if (len < 2) return idxArray;
    int pivot;
    if (mode == mode_x) pivot = get_instWithIdx(data, idxArray[start])->pmin.x;
    else pivot = get_instWithIdx(data, idxArray[start])->pmin.y;

    int sweep = start;
    int curEnd = end;

    int current;
    while(sweep != curEnd)
    {
        if (mode == mode_x)current = get_instWithIdx(data, idxArray[sweep + 1])->pmin.x;
        else current = get_instWithIdx(data, idxArray[sweep + 1])->pmin.y;
        // printf("%d %d, %d\n", data->inst_data->instArray[idxArray[sweep + 1]]->instIdx, data->inst_data->instArray[idxArray[sweep + 1]]->pmin.x, data->inst_data->instArray[idxArray[sweep + 1]]->pmin.y);
        if (pivot > current)
        {
            int temp = idxArray[sweep + 1];
            idxArray[sweep + 1] = idxArray[sweep];
            idxArray[sweep] = temp;
            sweep++;
        }
        else
        {
            int temp = idxArray[curEnd];
            idxArray[curEnd] = idxArray[sweep + 1];
            idxArray[sweep + 1] = temp;
            curEnd--;
        }
    }
    idxArray = _sort_instByPmin(data, idxArray, start, sweep - 1, mode);
    idxArray = _sort_instByPmin(data, idxArray, sweep + 1, end, mode);
    return idxArray;
}


int* sort_instByArea(database_ptr data)
{
    int* instIdxArray = (int*)malloc(sizeof(int) * data->inst_data->numInst);
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instIdxArray[i] = i;
    }
    instIdxArray = _sort_instByArea(data, instIdxArray, 0, data->inst_data->numInst);
    // for (int i = 0; i < data->inst_data->numInst; i++)
    // {
    //     instance_ptr inst = get_instWithIdx(data, instIdxArray[i]);
    //     printf("%s size = %llu\n", inst->instName, inst->area);
    // }
    return instIdxArray;
}


sortDB_ptr sort_instByPmin(database_ptr data)
{
    sortDB_ptr sort_data = (sortDB_ptr)malloc(sizeof(struct SORTDB));
    int* instIdxArray_X = (int*)malloc(sizeof(int) * data->inst_data->numInst);
    int* instIdxArray_Y = (int*)malloc(sizeof(int) * data->inst_data->numInst);
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instIdxArray_X[i] = i;
        instIdxArray_Y[i] = i;
    }
    instIdxArray_X = _sort_instByPmin(data, instIdxArray_X, 0, data->inst_data->numInst, mode_x);
    instIdxArray_Y = _sort_instByPmin(data, instIdxArray_Y, 0, data->inst_data->numInst, mode_y);
    // for (int i = 0; i < data->inst_data->numInst; i++)
    // {
    //     printf("%d, %d\n", data->inst_data->instArray[i]->pmin.x, data->inst_data->instArray[i]->pmin.y);
    // }
    sort_data->idxArray_x = instIdxArray_X;
    sort_data->idxArray_y = instIdxArray_Y;
    return sort_data;
}


sortDB_ptr create_sizeTable(database_ptr data, int* idxTokenArray)
{
    sizeToken_ptr sizeArray = NULL;
    cellArea curSize, prevSize;
    curSize = prevSize = 0;
    int curLen = 0;

    // Separate the instances according to the size. 
    // Group up same size instances in an array. 
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = get_instWithIdx(data, idxTokenArray[i]);
        curSize = inst->area;
        if (curSize != prevSize)
        {
            curLen++;
            if (curLen == 1) sizeArray = (sizeToken_ptr)malloc(sizeof(struct SIZETOKEN));
            else sizeArray = (sizeToken_ptr)realloc(sizeArray, sizeof(struct SIZETOKEN) * curLen);
            sizeArray[curLen - 1].size = curSize;
            sizeArray[curLen - 1].arrayLen = 1;
        }
        else
        {
            sizeArray[curLen - 1].arrayLen++;
        }
        int subLen = sizeArray[curLen - 1].arrayLen;
        if (subLen == 1)sizeArray[curLen - 1].idx_array = (int*)malloc(sizeof(int));
        else sizeArray[curLen - 1].idx_array = (int*)realloc(sizeArray[curLen - 1].idx_array, sizeof(int) * subLen);
        sizeArray[curLen - 1].idx_array[sizeArray[curLen - 1].arrayLen - 1] = inst->instIdx;
        prevSize = curSize;
    }

    // Copy to a new array to sort both x and y axis. 
    sizeToken_ptr sizeArray_Y = (sizeToken_ptr)malloc(sizeof(struct SIZETOKEN) * curLen);
    for (int i = 0; i < curLen; i++)
    {
        sizeArray_Y[i].arrayLen = sizeArray[i].arrayLen;
        sizeArray_Y[i].idx_array = (int*)malloc(sizeof(int) * sizeArray[i].arrayLen);
        for (int j = 0; j < sizeArray[i].arrayLen; j++)
        {
            int instIdx = sizeArray[i].idx_array[j];
            sizeArray_Y[i].idx_array[j] = instIdx;
        }
    }

    sizeDB_ptr size_data = (sizeDB_ptr)malloc(sizeof(struct SIZEDB));
    size_data->sizeTableX = sizeArray;
    size_data->sizeTableY = sizeArray_Y;
    size_data->numSizes = curLen;

    for (int i = 0; i < curLen; i++)
    {
        _sort_instByPmin(data, size_data->sizeTableX[i].idx_array, 0, size_data->sizeTableX[i].arrayLen, mode_x);
        _sort_instByPmin(data, size_data->sizeTableY[i].idx_array, 0, size_data->sizeTableY[i].arrayLen, mode_y);
    }
    sortDB_ptr ret = (sortDB_ptr)malloc(sizeof(struct SORTDB));
    ret->idxArray_x = (int*)calloc(sizeof(int), data->inst_data->numInst);
    ret->idxArray_y = (int*)calloc(sizeof(int), data->inst_data->numInst);
    int idx = 0;
    for (int i = 0; i < size_data->numSizes; i++)
    {
        int subLen = size_data->sizeTableX[i].arrayLen;
        for (int j = 0; j < subLen; j++)
        {
            ret->idxArray_x[idx] = size_data->sizeTableX[i].idx_array[j];
            ret->idxArray_y[idx] = size_data->sizeTableY[i].idx_array[j];
            idx++;
        }
    }
    for (int i = 0; i < size_data->numSizes; i++)
    {
        free(size_data->sizeTableX[i].idx_array);
        free(size_data->sizeTableY[i].idx_array);
    }
    free(size_data);

    return ret;
}


int checkInstPrevOvlp(ovlp_ptr ovlp_array, int arrayLen, instance_ptr inst)
{
    if (ovlp_array == NULL) return -1;
    for (int i = 0; i < arrayLen; i++)
    {
        if (inst == ovlp_array[i].ovlpInst) return i;
    }
    return -2;
}


void legalize(database_ptr data, char* mode)
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
        int* sortedInst = sort_instByArea(data);
        sortXY = create_sizeTable(data, sortedInst);
    }
    else
    {
        printf("PROC: Sort Coordinate MODE\n");
        sortXY = sort_instByPmin(data);
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
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = get_instWithIdx(data, sortedInst[i]);
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
            place_inst(inst, inst->pmin);
            immPlaced = check_legality_local(data, inst);
            if (immPlaced == true) 
            {
                fill_placementMap(data, inst);
                break;
            }
        }
        if (immPlaced == true) 
        {
            printf("Placed %d/%d Cells\n", i - failCnt, data->inst_data->numInst);
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
        place_inst(inst, inst->pmin);
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
        for (int i = 0; i < overlapLen; i++)
        {
            overlap[i].ovlpGrid.pmax = subtractPOS(overlap[i].abs_ovlpGrid.pmax, minGrid);
            overlap[i].ovlpGrid.pmin = subtractPOS(overlap[i].abs_ovlpGrid.pmin, minGrid);
            // if (overlap[i].ovlpGrid.pmin.x < 0)
            // {
            //     printf("\nInst %s, %s overlap (%d, %d) --> (%d, %d)\n", inst->instName, overlap[i].ovlpInst->instName, 
            //                                                         overlap[i].ovlpGrid.pmin.x, overlap[i].ovlpGrid.pmin.y,
            //                                                         overlap[i].ovlpGrid.pmax.x, overlap[i].ovlpGrid.pmax.y);
            //     printf("Inst %s, %s overlap (%d, %d) --> (%d, %d)\n", inst->instName, overlap[i].ovlpInst->instName, 
            //                                                         overlap[i].abs_ovlpGrid.pmin.x, overlap[i].abs_ovlpGrid.pmin.y,
            //                                                         overlap[i].abs_ovlpGrid.pmax.x, overlap[i].abs_ovlpGrid.pmax.y);
            //     printf("MinGrid = %d, %d\n", minGrid.x, minGrid.y);
            //     // printf("Inst %s (%d, %d) (%d, %d)\n", inst->instName, inst->pmin.x, inst->pmin.y,
            //     //                                     inst->pmax.x, inst->pmax.y);
            //     // printf("Inst %s (%d, %d) (%d, %d)\n", overlap[i].ovlpInst->instName, 
            //     //                                     overlap[i].ovlpInst->pmin.x, overlap[i].ovlpInst->pmin.y,
            //     //                                     overlap[i].ovlpInst->pmax.x, overlap[i].ovlpInst->pmax.y);
            //     printf("Inst %s (%d, %d) (%d, %d)\n", inst->instName, inst->pmin.x / gridSize.x, inst->pmin.y / gridSize.y,
            //                                         inst->pmax.x / gridSize.x, inst->pmax.y / gridSize.y);
            //     printf("Inst %s (%d, %d) (%d, %d)\n", overlap[i].ovlpInst->instName, 
            //                                         overlap[i].ovlpInst->pmin.x / gridSize.x, overlap[i].ovlpInst->pmin.y / gridSize.y,
            //                                         overlap[i].ovlpInst->pmax.x / gridSize.x, overlap[i].ovlpInst->pmax.y / gridSize.y);
            // }
            
            struct POS ovlpArea = subtractPOS(overlap[i].ovlpGrid.pmax, overlap[i].ovlpGrid.pmin);
            if (ovlpArea.x * gridSize.x > ovlpArea.y * gridSize.y)
            {
                // X overlap is larger. Move up/down
            }
            else
            {
                // Y overlap is larger. Move left/right
            }
        }




        // else printf("Proc: %d/%d ### Immediate place failed ###\n", ++failCnt, i);

        // place_inst(inst, inst->pmin);
        // bool isLegal = check_legality_local(data, inst);
        // if (isLegal) fill_placementMap(data, inst);
        // else
        // {
        //     instance_ptr* ovlpInstList = NULL;
        //     int numOvlpInst = 0;
        //     int startRow = inst->pmin.y / data->die_data->site.y;
        //     int endRow   = inst->pmax.y / data->die_data->site.y;

        //     int startCol = inst->pmin.x / data->die_data->site.x;
        //     int endCol   = inst->pmax.x / data->die_data->site.x;
        //     if (ovlpInstList) free(ovlpInstList);
        // }
        // place_inst(inst, inst->pmin);
        if (overlap != NULL)
        {
            free(overlap);
            overlap = NULL;
        }
    }
    free(sortedInst);
    struct timeval end;
    gettimeofday(&end, NULL);
    double runtime = stopwatch(&start, &end);
    printf("=======================================\n");
    printf("Legalization time : %3lf sec\n", runtime);
    printf("=======================================\n");
    
    printf("PROC: Finished Legalization\n");
}