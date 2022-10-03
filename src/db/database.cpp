#include "database.h"


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


instance_ptr update_instanceTech(char* name, techInst_ptr techInst, int rowHeight)
{
    instance_ptr inst = create_instance(name, techInst->size.x, techInst->size.y, techInst->numPins, rowHeight);
    for (int i = 0; i < inst->numPins; i++)
    {
        
    }
}


database_ptr create_database(void)
{
    database_ptr data = (database_ptr)malloc(sizeof(struct DATABASE));
    data->tech_data = create_techDB();
    return data;
}


database_ptr database_init(char* filedir)
{
    FILE* target_file = fopen(filename, "r");
    if (target_file == NULL) return NULL;
    char line_array[max_line_length];
    char* line = &line_array[0];

    //Get first line
    line = fgets(line, max_line_length, target_file);
    while (line && line[0] < 32) line = fgets(line, max_line_length, target_file); // Skip Enters. 
    splitToken_ptr Token = split_line(line);
    char** splitted = Token->split;

    techInst_ptr curTech = NULL;

    database_ptr data = create_database();
    int cur_state = state_libCell;
    while(line)
    {
        switch (cur_state)
        {
        case state_libCell:
            masterCellName = splitted[1];
            int sizeX = atoi(splitted[2]);
            int sizeY = atoi(splitted[3]);
            int numPin = atoi(splitted[4]);
            curTech = create_techInst(data->tech_data, masterCellName, sizeX, sizeY, numPin);
            cur_state = state_libPin;
            break;
        case state_libPin:
            curTech->techPinArray[curTech->curNumPins].pinPos.x = atoi(splitted[2]);
            curTech->techPinArray[curTech->curNumPins].pinPos.y = atoi(splitted[3]);
            break;
        case state_die:
            int pmin_x = atoi(splitted[1]);
            int pmin_y = atoi(splitted[2]);
            int rowLen = atoi(splitted[3]);
            int rowHeight = atoi(splitted[4]);
            int repeatCount = atoi(splitted[5]);
            data->die_data = create_die(pmin_x, pmin_y, rowLen, rowHeight, repeatCount);
            cur_state = state_instDB;
            break;
        case state_inst:
            char* name = splitted[1];
            char* libcellName = splitted[2];
            techInst_ptr curTech = get_techInst(data->tech_data, libcellName);
            if (curTech == NULL)
            {
                fprintf(stderr, "Failed to find tech\n");
                exit(EXIT_FAILURE);
            }
            int pmin_x = atoi(splitted[3]);
            int pmin_y = atoi(splitted[4]);
            int rotate_flag = atoi(splitted[5]);


        default:
            break;
        }
        destroy_splitToken(Token);
        line = fgets(line, max_line_length, target_file);
        if(line == NULL) break;
        while (line[0] < 32) line = fgets(line, max_line_length, target_file);
        Token = split_line(line);
        splitted = Token->split;
        if (cur_state == state_libPin)
        {
            if (!strcmp(splitted[0], "LibCell")) cur_state = state_libCell;
            else if (!strcmp(splitted[0], "Pin")) cur_state = state_libPin;
            else cur_state = state_die;
        }
        if (cur_state == state_inst)
        {
            if (!strcmp(splitted[0], "inst")) cur_state = state_inst;
            else cur_state = state_netDB;
        }

    }

    return data;
}