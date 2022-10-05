#include "instance.h"


// Construction/Destruction function
instance_ptr create_instance(instDB_ptr data, char* instName, char* techName, 
                             int pmin_x, int pmin_y, char* rotate_flag)
{
    instance_ptr inst = (instance_ptr)malloc(sizeof(struct INSTANCE));
    inst->instName = strdup(instName);
    inst->techName = strdup(techName);
    inst->pmin.x = pmin_x;
    inst->pmin.y = pmin_y;
    resetPOS(inst->cent);
    resetPOS(inst->pmax);
    
    inst->fpmin = posToFPOS(inst->pmin);
    resetFPOS(inst->fcent);
    resetFPOS(inst->fpmax);

    resetPOS(inst->size);
    resetFPOS(inst->fsize);

    inst->numPins = 0;
    inst->instPinArray = NULL;

    inst->rotate_flag = strdup(rotate_flag);

    inst->rowHeight = -1;
    inst->numRows = -1;
    inst->rowIdxArray = NULL;

    inst->instIdx = -1;
    int instHashIdx = hash_function(inst->instName);
    inst->next = data->hashTable[instHashIdx].start;
    data->hashTable[instHashIdx].start = inst;
    return inst;
}


void destroy_instance(instance_ptr inst)
{
    if (inst->instName) free(inst->instName);
    if (inst->techName) free(inst->techName);
    if (inst->instPinArray) free(inst->instPinArray);
    if (inst->rotate_flag) free(inst->rotate_flag);
    if (inst->rowIdxArray) free(inst->rowIdxArray);
    free(inst);
}


instDB_ptr create_instDB(void)
{
    instDB_ptr data = (instDB_ptr)malloc(sizeof(struct INSTDB));
    data->numInst = 0;
    data->curNumInst = 0;

    data->hashTable = (instHash_ptr)calloc(sizeof(struct INSTHASH), default_hash_size);
    for (int i = 0; i < default_hash_size; i++)
    {
        data->hashTable[i].start = NULL;
    }
    data->instArray = NULL;
    return data;
}


void destroy_instDB(instDB_ptr rmdb)
{
    for (int i = 0; i < default_hash_size; i++)
    {
        instance_ptr sweep = rmdb->hashTable[i].start;
        while(sweep)
        {
            instance_ptr sweep_next = sweep->next;
            destroy_instance(sweep);
            sweep = sweep_next;
        }
    }
    free(rmdb->hashTable);
    if (rmdb->instArray) free(rmdb->instArray);
    free(rmdb);
}


// Utility function
instance_ptr _get_instance(instDB_ptr data, char* instName)
{
    int instHashIdx = hash_function(instName);
    instance_ptr sweep = data->hashTable[instHashIdx].start;
    while(sweep)
    {
        if (!strcmp(sweep->instName, instName)) return sweep;
        sweep = sweep->next;
    }
    return NULL;
}


void flip_instance_horizontal(instance_ptr inst)
{
    for (int i = 0; i < inst->numPins; i++)
    {
        pin_ptr pin = inst->instPinArray[i];
        pin->absPos.x = inst->fpmax.x - pin->absPos.x;
        setFPOS(pin->pinPos, inst->fpmin, pin->absPos);
    }
}


void flip_instance_vertical(instance_ptr inst)
{
    for (int i = 0; i < inst->numPins; i++)
    {
        pin_ptr pin = inst->instPinArray[i];
        pin->absPos.y = inst->fpmax.y - pin->absPos.y;
        setFPOS(pin->pinPos, inst->fpmin, pin->absPos);
    }
}


void rotate_instance(instance_ptr inst)
{
    for (int i = 0; i < inst->numPins; i++)
    {
        pin_ptr pin = inst->instPinArray[i];
        pin->absPos.x = inst->fpmax.x - pin->absPos.x;
        pin->absPos.y = inst->fpmax.y - pin->absPos.y;
        setFPOS(pin->pinPos, inst->fpmin, pin->absPos);
    }
}


void print_allInst(instDB_ptr data)
{
    for (int i = 0; i < data->numInst; i++)
    {
        instance_ptr curInst = data->instArray[i];
        printf("Inst %s @(%d, %d)\n", curInst->instName, 
                                      curInst->pmin.x, 
                                      curInst->pmin.y);
    }
}