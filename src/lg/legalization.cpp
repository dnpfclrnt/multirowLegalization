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
        if (pivot > get_instWithIdx(data, idxArray[sweep + 1])->area)
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
        printf("screwed up!\n");
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
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = get_instWithIdx(data, instIdxArray[i]);
        printf("%s size = %llu\n", inst->instName, inst->area);
    }
    return instIdxArray;
}


void legalize(database_ptr data)
{
    printf("PROC: Start Legalization\n");
    struct timeval start;
    gettimeofday(&start, NULL);
    lgInst_init(data);
    int* sortedInst = sort_instByArea(data);
    struct POS dieGridDim;
    struct POS gridSize;
    struct POS candidate[4];
    dieGridDim.x = data->die_data->pmax.x / data->die_data->site.x;
    dieGridDim.y = data->die_data->repeatCount;

    gridSize.x = data->die_data->site.x;
    gridSize.y = data->die_data->site.y;

    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr inst = get_instWithIdx(data, sortedInst[i]);
        inst->pmin.x = (inst->pmin.x / gridSize.x) * gridSize.x;
        inst->pmin.y = (inst->pmin.y / gridSize.y) * gridSize.y;
        place_inst(inst, inst->pmin);
        bool isLegal = check_legality_local(data, inst);
        if (isLegal) fill_placementMap(data, inst);
        else
        {
            instance_ptr* ovlpInstList = NULL;
            int numOvlpInst = 0;
            int startRow = inst->pmin.y / data->die_data->site.y;
            int endRow   = inst->pmax.y / data->die_data->site.y;

            int startCol = inst->pmin.x / data->die_data->site.x;
            int endCol   = inst->pmax.x / data->die_data->site.x;
            if (ovlpInstList) free(ovlpInstList);
        }
        place_inst(inst, inst->pmin);
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