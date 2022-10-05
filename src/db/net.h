#ifndef _NET_H
#define _NET_H

// #include "../global.h"
#include "../utils.h"


struct PIN{
    char* pinName;
    int netIdx;
    int netPinIdx;
    int instIdx;
    int instPinIdx;
    bool isPort;
    void* inst;
    void* net;
    
    struct FPOS pinPos;
    struct FPOS absPos;
}; typedef struct PIN* pin_ptr;


struct NET{
    char* netName;
    int numInst;
    int* instArray;

    int numPins;
    int curNumPins;
    pin_ptr* netPinArray;

    struct FPOS pmax;
    struct FPOS pmin;
    prec HPWL;
    struct NET* next;
}; typedef struct NET* net_ptr;


struct NETHASH{
    net_ptr start;
}; typedef struct NETHASH* netHash_ptr;


struct NETDB{
    int numNet;
    int curNumNet;
    netHash_ptr hashTable;
    net_ptr* netArray;
}; typedef struct NETDB* netDB_ptr;


// List of functions
// Const/Dist functions
pin_ptr create_pin(struct FPOS pinPos);
net_ptr create_net(netDB_ptr data, char* netName, int numPins);
void destroy_net(net_ptr net);
netDB_ptr create_netDB(void);
void  destroy_netDB(netDB_ptr rmdb);
net_ptr _get_net(netDB_ptr data, char* netName);
void print_allNets(netDB_ptr data);

#endif