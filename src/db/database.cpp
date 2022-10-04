#include "database.h"


instance_ptr get_inst(database_ptr data, char* instName)
{
    return _get_instance(data->inst_data, instName);
}


net_ptr get_net(database_ptr data, char* netName)
{
    return _get_net(data->net_data, netName);
}


database_ptr create_database(void)
{
    database_ptr data = (database_ptr)malloc(sizeof(struct DATABASE));
    data->inst_data = create_instDB();
    data->net_data = create_netDB();
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