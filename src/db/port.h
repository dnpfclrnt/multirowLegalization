#ifndef _PORT_H
#define _PORT_H


#include "net.h"
#include "../utils.h"


struct PORT{
    char* portName;
    struct POS cent;
    struct FPOS fcent;
    pin_ptr pin;
    // char* rotate_flag;
    struct PORT* next;
}; typedef struct PORT* port_ptr;


struct PORTHASH{
    port_ptr start;
}; typedef struct PORTHASH* portHash_ptr;


struct PORTDB{
    int numPorts;
    int curNumPorts;
    portHash_ptr hashTable;
    port_ptr* portArray;
}; typedef struct PORTDB* portDB_ptr;


//List of functions
port_ptr create_port(portDB_ptr data, char* portName, int posX, int posY, ...);
void destroy_port(port_ptr port);
portDB_ptr create_portDB(void);
void destroy_portDB(portDB_ptr rmdb);

#endif