#include "lgUtil.h"


int* _sort_instByArea(database_ptr data, int* idxArray, int start, int end)
{
    int len = end - start;
    // printf("Recurse %d %d\n", start, end);
    if (len < 2) return idxArray;
    unsigned long long pivot = get_instWithIdx(data, idxArray[start])->area;

    int sweep = start;
    int curEnd = end - 1;
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
    int curEnd = end - 1;

    int current;
    while(sweep < curEnd)
    {
        if (mode == mode_x)current = get_instWithIdx(data, idxArray[sweep + 1])->pmin.x;
        else current = get_instWithIdx(data, idxArray[sweep + 1])->pmin.y;
        // printf("%d %d, %d\n", data->inst_data->instArray[idxArray[sweep + 1]]->instIdx, data->inst_data->instArray[idxArray[sweep + 1]]->pmin.x, data->inst_data->instArray[idxArray[sweep + 1]]->pmin.y);
        if (pivot > current)
        {
            // printf("Hit a\n");
            int temp = idxArray[sweep + 1];
            idxArray[sweep + 1] = idxArray[sweep];
            idxArray[sweep] = temp;
            sweep++;
        }
        else
        {
            // printf("Hit b\n");
            int temp = idxArray[curEnd];
            idxArray[curEnd] = idxArray[sweep + 1];
            idxArray[sweep + 1] = temp;
            curEnd--;
        }
    }
    if (start != sweep) idxArray = _sort_instByPmin(data, idxArray, start, sweep - 1, mode);
    if (end != sweep)   idxArray = _sort_instByPmin(data, idxArray, sweep + 1, end, mode);
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