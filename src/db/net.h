#ifndef _NET_H
#define _NET_H

#include "../global.h"
#include "../utils.h"


struct PIN{
    int netIdx;
    int netPinIdx;
    int instIdx;
    int instPinIdx;
    struct FPOS pinPos;
    struct FPOS absPos;
}; typedef struct PIN* pin_ptr;


struct NET{
    char* netName;
    int numInst;
    int* instArray;

    int numPins;
    pin_ptr* netPinArray;

    struct FPOS pmax;
    struct FPOS pmin;
    prec HPWL;

}; typedef struct NET* net_ptr;


struct NETDB{
    int numNet;
    net_ptr* netArray;
}; typedef struct NETDB* netDB_ptr;


// List of functions
pin_ptr create_pin(int netIdx, int netPinIdx, int instIdx, int instPinIdx, 
                   struct FPOS pinPos);
void destroy_pin(pin_ptr pin);
net_ptr create_net(char* netName, int numPins);
void destroy_net(net_ptr net);
netDB_ptr create_netDB(int numNet);
void destroy_netDB(netDB_ptr rmdb);


#endif