#include "net.h"


pin_ptr create_pin(int netIdx, int netPinIdx, int instIdx, int instPinIdx, 
                   struct FPOS pinPos)
{
    pin_ptr pin = (pin_ptr)malloc(sizeof(struct PIN));
    pin->netIdx = netIdx;
    pin->netPinIdx = netPinIdx;
    pin->instIdx = instIdx;
    pin->instPinIdx = instPinIdx;
    pin->pinPos = pinPos;
    return pin;
}


void destroy_pin(pin_ptr pin)
{
    free(pin);
}


net_ptr create_net(char* netName, int numPins)
{
    net_ptr net = (net_ptr)malloc(sizeof(struct NET));
    net->netName = strdup(netName);

    net->numInst = 0;
    net->instArray = NULL;
    net->numPins = numPins;
    net->netPinArray = (pin_ptr*)calloc(sizeof(pin_ptr), numPins);

    resetFPOS(net->pmax);
    resetFPOS(net->pmin);
    net->HPWL = -1;
    return net;
}


void destroy_net(net_ptr net)
{
    if (net->instArray)
    {
        free(net->instArray);
    }
    for (int i = 0; i < net->numPins; i++)
    {
        if (net->netPinArray[i])
        {
            destroy_pin(net->netPinArray[i]);
        }
    }
    free(net->netPinArray);
    free(net);
}


netDB_ptr create_netDB(int numNet)
{
    netDB_ptr data = (netDB_ptr)malloc(sizeof(struct NETDB));
    data->numNet = numNet;
    data->netArray = (net_ptr*)calloc(sizeof(net_ptr), numNet);
    return data;
}


void destroy_netDB(netDB_ptr rmdb)
{
    for (int i = 0; i < rmdb->numNet; ++i) 
    {
        if (rmdb->netArray[i])
        {
            destroy_net(rmdb->netArray[i]);
        }
    }
    free(rmdb);
}