#include "database.h"


instance_ptr get_inst(database_ptr data, char* instName)
{
    return _get_instance(data->inst_data, instName);
}


pin_ptr get_instPin(database_ptr data, char* instname, char* pinName)
{
    instance_ptr inst = get_inst(data, instname);
    for (int i = 0; i < inst->numPins; i++)
    {
        pin_ptr sweep = inst->instPinArray[i];
        if (!strcmp(sweep->pinName), pinName) return sweep;
    }
    return NULL;
}


void inst_init(instance_ptr inst)
{
    // This is function after linking instance and tech. 
    techInst_ptr curTech = (techInst_ptr)inst->techInst;

    struct FPOS fsize = posToFPOS(curTech->size);
    setFPOS(inst->fcent, inst->fpmin, fpdiv(fsize, 2.0));
    setFPOS(inst->fpmax, inst->fpmin, fsize);
    
    setPOS(inst->cent, inst->pmin, pdiv(curTech->size, 2));
    setPOS(inst->pmax, inst->pmin, curTech->size);

    inst->size = curTech->szie;
    inst->fsize = fsize;

    inst->numPins = curTech->numPins;
    for (int i = 0; i < curTech->numPins; i++)
    {
        pin_ptr pin = create_pin(curTech->techPinArray[i].pinPos);
        pin->pinName = strdup(curTech->techPinArray[i].pinName);
        pin->inst = inst;
        inst->instPinArray[i] = pin;
    }
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
    net_ptr curNet = NULL;

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
            curTech->techPinArray[curTech->curNumPins].pinName = strdup(splitted[1]);
            curTech->techPinArray[curTech->curNumPins].pinPos.x = atoi(splitted[2]);
            curTech->techPinArray[curTech->curNumPins].pinPos.y = atoi(splitted[3]);
            break;
        case state_dieSize:
            int pmin_x = atoi(splitted[1]);
            int pmin_y = atoi(splitted[2]);
            int pmax_x = atoi(splitted[3]);
            int pmax_y = atoi(splitted[4]);
            data->die_data = create_die(pmin_x, pmin_y, pmax_x, pmax_y);
            cur_state = state_die;
            break;

        case state_die:
            int rowLen = atoi(splitted[3]);
            int rowHeight = atoi(splitted[4]);
            int repeatCount = atoi(splitted[5]);
            update_dieRow(data->die_data, rowLen, rowHeight, repeatCount);
            cur_state = state_instDB;
            break;
        case state_instDB:
            data->inst_data->numInst = atoi(splitted[1]);
            cur_state = state_inst;
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
            instance_ptr curInst = create_instance(data->inst_data, name, libcellName, pmin_x, pmin_y, rotate_flag);
            inst_init(curInst);
            curInst->techInst = curTech;
            break;
        case state_netDB:
            data->net_data->numNet = atoi(splitted[1]);
            cur_state = state_net;
            break;
        case state_net:
            curNet = create_net(data->net_data, splitted[1], atoi(splitted[2]));
            cur_state = state_netPin;
            break;
        case state_netPin:
            splitToken_ptr cell_pin = split_dash(splitted[1]);
            pin_ptr curPin = get_instPin(data, cell_pin->split[0], cell_pin->split[1]);
            curPin->net = curNet;
            break;

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
            else cur_state = state_dieSize;
        }
        if (cur_state == state_inst)
        {
            if (!strcmp(splitted[0], "inst")) cur_state = state_inst;
            else cur_state = state_netDB;
        }
        if (cur_state == state_netPin)
        {
            if (!strcmp(splitted[0], "Pin")) cur_state = state_netPin;
            else cur_state = state_net;
        }
    }
    return data;
}


void destroy_database(database_ptr data)
{
    destroy_die(data->die_data);
    destroy_instDB(data->inst_data);
    destroy_netDB(data->net_data);
    destroy_techDB(data->tech_data);
    free(data);
}