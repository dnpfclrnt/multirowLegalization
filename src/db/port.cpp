#include "port.h"


port_ptr create_port(portDB_ptr data, char* portName, int posX, int posY, ...)
{
    port_ptr port = (port_ptr)malloc(sizeof(struct PORT));
    port->portName = strdup(portName);
    port->cent.x = posX;
    port->cent.y = posY;
    port->fcent = posToFPOS(port->cent);
    port->pin = NULL;

    int portHashIdx = hash_function(portName);
    port->next = data->hashTable[portHashIdx].start;
    data->hashTable[portHashIdx].start = port;

    return port;
}


void destroy_port(port_ptr port)
{
    if (port->portName) free(port->portName);
    // free(port->rotate_flag);
    free(port);
}


portDB_ptr create_portDB(void)
{
    portDB_ptr data = (portDB_ptr)malloc(sizeof(struct PORTDB));
    data->numPorts = 0;
    data->hashTable = (portHash_ptr)calloc(sizeof(struct PORTHASH), default_hash_size);
    for (int i = 0; i < default_hash_size; i++)
    {
        data->hashTable[i].start = NULL;
    }
    data->portArray = NULL;
    return data;
}


void destroy_portDB(portDB_ptr rmdb)
{
    for (int i = 0; i < default_hash_size; i++)
    {
        port_ptr sweep = rmdb->hashTable[i].start;
        while(sweep)
        {
            port_ptr sweep_next = sweep->next;
            destroy_port(sweep);
            sweep = sweep_next;
        }
    }
    free(rmdb->hashTable);
    if (rmdb->portArray) free(rmdb->portArray);
    free(rmdb);
}


// Utility function
void print_allPorts(portDB_ptr data)
{
    for (int i = 0; i < data->numPorts; i++)
    {
        port_ptr curPort = data->portArray[i];
        printf("Port %s @(%d, %d)\n", curPort->portName,
                                      curPort->cent.x,
                                      curPort->cent.y);
    }
}
