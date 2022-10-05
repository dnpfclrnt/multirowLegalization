#include "database.h"


instance_ptr get_inst(database_ptr data, char* instName)
{
    return _get_instance(data->inst_data, instName);
}


instance_ptr get_instWithIdx(database_ptr data, int idx)
{
    return data->inst_data->instArray[idx];
}


pin_ptr get_instPin(database_ptr data, char* instname, char* pinName)
{
    instance_ptr inst = get_inst(data, instname);
    for (int i = 0; i < inst->numPins; i++)
    {
        pin_ptr sweep = inst->instPinArray[i];
        if (!strcmp(sweep->pinName, pinName)) return sweep;
    }
    return NULL;
}


port_ptr get_port(database_ptr data, char* portName)
{
    int hashKey = hash_function(portName);
    port_ptr sweep = data->port_data->hashTable[hashKey].start;
    while (sweep)
    {
        if (!strcmp(sweep->portName, portName)) return sweep;
        sweep = sweep->next;
    }
    return NULL;
}


void inst_init(instance_ptr inst)
{
    // This is function after linking instance and tech. 
    techInst_ptr curTech = (techInst_ptr)inst->techInst;

    struct FPOS fsize = posToFPOS(curTech->size);

    inst->size = curTech->size;
    inst->fsize = fsize;

    inst->area = (unsigned long long)inst->size.x * (unsigned long long)inst->size.y;
    inst->farea = inst->fsize.x * inst->fsize.y;

    inst->numPins = curTech->numPins;
    inst->instPinArray = (pin_ptr*)calloc(sizeof(pin_ptr), inst->numPins);
    for (int i = 0; i < curTech->numPins; i++)
    {
        pin_ptr pin = create_pin(curTech->techPinArray[i].pinPos);
        pin->pinName = strdup(curTech->techPinArray[i].pinName);
        pin->inst = inst;
        pin->isPort = false;
        inst->instPinArray[i] = pin;
    }
    place_inst(inst, inst->pmin);
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
    data->port_data = create_portDB();
    return data;
}


database_ptr database_init(char* filedir)
{
    FILE* target_file = fopen(filedir, "r");
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
    char* masterCellName;
    int sizeX, sizeY, numPin;
    struct POS site;
    int cur_state = state_site;
    while(line)
    {
        switch (cur_state)
        {
            case state_site:
                site.x = atoi(splitted[1]);
                site.y = atoi(splitted[2]);
                cur_state = state_libCell;
                break;
            case state_libCell: 
                masterCellName = splitted[1];
                sizeX = atoi(splitted[2]);
                sizeY = atoi(splitted[3]);
                numPin = atoi(splitted[4]);
                curTech = create_techInst(data->tech_data, masterCellName, sizeX, sizeY, numPin);
                cur_state = state_libPin;
                break;
            case state_libPin:
                curTech->techPinArray[curTech->curNumPins].pinName = strdup(splitted[1]);
                curTech->techPinArray[curTech->curNumPins].pinPos.x = atoi(splitted[2]);
                curTech->techPinArray[curTech->curNumPins].pinPos.y = atoi(splitted[3]);
                curTech->curNumPins++;
                break;
            case state_dieSize: {
                int pmin_x = atoi(splitted[1]);
                int pmin_y = atoi(splitted[2]);
                int pmax_x = atoi(splitted[3]);
                int pmax_y = atoi(splitted[4]);
                data->die_data = create_die(pmin_x, pmin_y, pmax_x, pmax_y);
                data->die_data->site = site;
                cur_state = state_die;
                break;
            }
            case state_die: {
                int rowLen = atoi(splitted[3]);
                int rowHeight = atoi(splitted[4]);
                int repeatCount = atoi(splitted[5]);
                update_dieRow(data->die_data, rowLen, rowHeight, repeatCount);
                cur_state = state_instDB;
                break;
            }

            case state_instDB:
                data->inst_data->numInst = atoi(splitted[1]);
                cur_state = state_inst;
                break;
            case state_inst: {
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
                char* rotate_flag = splitted[5];
                instance_ptr curInst = create_instance(data->inst_data, name, libcellName, pmin_x, pmin_y, rotate_flag);
                curInst->techInst = curTech;
                inst_init(curInst);
                break;
            }

            case state_portDB:
                data->port_data->numPorts = atoi(splitted[1]);
                data->port_data->curNumPorts = 0;
                cur_state = state_port;
                break;
            case state_port: {
                char* portName = splitted[1];
                int cent_x = atoi(splitted[2]);
                int cent_y = atoi(splitted[3]);
                char* rotateflag = splitted[4];
                port_ptr port = create_port(data->port_data, portName, cent_x, cent_y, rotateflag);
                port->pin = create_pin(port->fcent);
                port->pin->inst = port;
                port->pin->isPort = true;
                break;
            }

            case state_netDB:
                data->net_data->numNet = atoi(splitted[1]);
                cur_state = state_net;
                break;
            case state_net:
                // printf("%s", line);
                curNet = create_net(data->net_data, splitted[1], atoi(splitted[2]));
                cur_state = state_netPin;
                break;
            case state_netPin: {
                splitToken_ptr cell_pin = split_dash(splitted[1]);
                if (!strcmp(cell_pin->split[0], "PIN"))
                {
                    port_ptr port = get_port(data, cell_pin->split[1]);
                    pin_ptr curPin = port->pin;
                    curPin->net = curNet;
                    curNet->netPinArray[curNet->curNumPins++] = curPin;
                }
                else
                {
                    pin_ptr curPin = get_instPin(data, cell_pin->split[0], cell_pin->split[1]);
                    if (curPin == NULL)
                    {
                        fprintf(stderr, "Failed to find pin\n");
                        exit(EXIT_FAILURE);
                    }
                    curPin->net = curNet;
                    curNet->netPinArray[curNet->curNumPins++] = curPin;
                }
                break;
            }
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
            if (!strcmp(splitted[0], "Inst")) cur_state = state_inst;
            else cur_state = state_portDB;
        }
        if (cur_state == state_port)
        {
            if (!strcmp(splitted[0], "Port")) cur_state = state_port;
            else cur_state = state_netDB;
        }
        if (cur_state == state_netPin)
        {
            if (!strcmp(splitted[0], "Pin")) cur_state = state_netPin;
            else cur_state = state_net;
        }
    }
    // for (int i = 0; i < default_hash_size; i++)
    // {
    //     techInst_ptr sweep = data->tech_data->hashTable[i].start;
    //     while (sweep)
    //     {
    //         printf("MC %s\n", sweep->techName);
    //         for (int j = 0; j < sweep->numPins; j++)
    //         {
    //             struct POS pPos = fposToPOS(sweep->techPinArray[j].pinPos);
    //             printf("Pin %s @(%d,%d)\n", sweep->techPinArray[j].pinName, pPos.x, pPos.y);
    //         }
    //         sweep = sweep->next;
    //     }
    // }
    printf("PROC: Finished data construction. Start data link\n");
    data->inst_data->instArray = (instance_ptr*)calloc(sizeof(instance_ptr), data->inst_data->numInst);
    data->net_data->netArray = (net_ptr*)calloc(sizeof(net_ptr), data->net_data->numNet);
    data->port_data->portArray = (port_ptr*)calloc(sizeof(port_ptr), data->port_data->numPorts);

    data->inst_data->curNumInst = data->net_data->curNumNet = data->port_data->curNumPorts = 0;

    for (int i = 0; i < default_hash_size; i++)
    {
        instance_ptr instSweep = data->inst_data->hashTable[i].start;
        net_ptr netSweep = data->net_data->hashTable[i].start;
        port_ptr portSweep = data->port_data->hashTable[i].start;
        while(instSweep)
        {
            instSweep->instIdx = data->inst_data->curNumInst;
            data->inst_data->instArray[data->inst_data->curNumInst++] = instSweep;
            instSweep = instSweep->next;
        }
        while (netSweep)
        {
            netSweep->netIdx = data->net_data->curNumNet;
            data->net_data->netArray[data->net_data->curNumNet++] = netSweep;
            netSweep = netSweep->next;
        }
        while (portSweep)
        {
            portSweep->portIdx = data->port_data->curNumPorts;
            data->port_data->portArray[data->port_data->curNumPorts++] = portSweep;
            portSweep = portSweep->next;
        }
    }
    printf("PROC: Finished constructing array\n");
    printf("INSTANCE = %d/%d\tNET = %d/%d\tPORT = %d/%d\n", data->inst_data->curNumInst, 
                                                            data->inst_data->numInst, 
                                                            data->net_data->curNumNet, 
                                                            data->net_data->numNet, 
                                                            data->port_data->curNumPorts, data
                                                            ->port_data->numPorts);
    data->inst_data->numInst = data->inst_data->curNumInst;
    data->net_data->numNet = data->net_data->curNumNet;
    data->port_data->numPorts = data->port_data->curNumPorts;
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        instance_ptr curInst = data->inst_data->instArray[i];
        curInst->rowHeight = data->die_data->rowHeight;
        for (int j = 0; j < curInst->numPins; j++)
        {
            pin_ptr curPin = curInst->instPinArray[j];
            curPin->instIdx = i;
            curPin->instPinIdx = j;
        }
    }
    for (int i = 0; i < data->net_data->numNet; i++)
    {
        net_ptr curNet = data->net_data->netArray[i];
        for (int j = 0; j < curNet->numPins; j++)
        {
            pin_ptr curPin = curNet->netPinArray[j];
            curPin->netIdx = i;
            curPin->netPinIdx = j;
        }
    }
    for (int i = 0; i < data->port_data->numPorts; i++)
    {
        port_ptr curPort = data->port_data->portArray[i];
        pin_ptr curPin = curPort->pin;
        if (curPin == NULL)
        {
            fprintf(stderr, "Port %s pin failure\n", curPort->portName);
            exit(EXIT_FAILURE);
        }
        curPin->instIdx = i;
        curPin->instPinIdx = -1;
    }
    printf("PROC: Finished Construction\n");
    return data;
}


void destroy_database(database_ptr data)
{
    printf("PROC: Start destruction\n");
    // printf("PROC: Destroying Die data\n");
    destroy_die(data->die_data);
    // printf("PROC: Destroying Inst data\n");
    destroy_instDB(data->inst_data);
    // printf("PROC: Destroying Net data\n");
    destroy_netDB(data->net_data);
    // printf("PROC: Destroying Tech data\n");
    destroy_techDB(data->tech_data);
    // printf("PROC: Destroying Port data\n");
    destroy_portDB(data->port_data);
    // printf("PROC: Finished destroying data\n");
    free(data);
}