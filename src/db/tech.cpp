#include "tech.h"


techDB_ptr create_techDB(void)
{
    techDB_ptr data = (techDB_ptr)malloc(sizeof(struct TECHDB));
    data->curHashLen = default_hash_size;
    data->hashTable = (techHashToken_ptr)malloc(sizeof(struct TECHHASHTOKEN) * default_hash_size);
    for (int i = 0; i < default_hash_size; i++)
    {
        data->hashTable[i].start = NULL;
    }
    return data;
}


techInst_ptr create_techInst(techDB_ptr tech_data, char* techName, int sizeX, int sizeY, int numPins)
{
    techInst_ptr techInst = (techInst_ptr)malloc(sizeof(struct TECHINST));
    techInst->techName = strdup(techName);
    techInst->size.x = sizeX;
    techInst->size.y = sizeY;
    techInst->curNumPins = 0;
    techInst->numPins = numPins;
    techInst->techPinArray = (techPin_ptr)calloc(sizeof(struct TECHPIN), numPins);
    techInst->next = NULL;

    unsigned int techIdx = hash_function(techName);
    techInst->next = tech_data->hashTable[techIdx].start;
    tech_data->hashTable[techIdx].start = techInst;
    return techInst;
}


techInst_ptr get_techInst(techDB_ptr tech_data, char* techName)
{
    unsigned int techIdx = hash_function(techName);
    techInst_ptr sweep = tech_data->hashTable[techIdx];
    while(sweep)   
    {
        if (!strcmp(techName, sweep->techName)) return sweep;
        sweep = sweep->next;
    }
    return NULL;
}


void destroy_techInst(techInst_ptr techInst)
{
    free(techInst->techName);
    free(techInst->techPinArray);
    free(techInst);
}


void destroy_techDB(techDB_ptr rmdb)
{
    for (int i = 0; i < default_hash_size; i++)
    {
        techInst_ptr start = rmdb->hashTable[i];
        while(start)
        {
            techInst_ptr start_next = start->next;
            destroy_techInst(start);
            start = start_next;
        }
    }
    free(rmdb->hashTable);
    free(rmdb);
}