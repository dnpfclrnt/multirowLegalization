#include "net.h"


// Construction/Destruction function
pin_ptr create_pin(struct FPOS pinPos)
{
    pin_ptr pin = (pin_ptr)malloc(sizeof(struct PIN));
    pin->netIdx = pin->netPinIdx = -1;
    pin->instIdx = pin->instPinIdx = -1;
    pin->inst = pin->net = NULL;
    pin->pinPos = pinPos;
    return pin;
}


net_ptr create_net(netDB_ptr data, char* netName, int numPins)
{
    net_ptr net = (net_ptr)malloc(sizeof(struct NET));
    net->netName = strdup(netName);

    net->numInst = -1;
    net->instArray = NULL;

    net->numPins = numPins;
    net->curNumPins = 0;
    net->netPinArray = (pin_ptr*)calloc(sizeof(pin_ptr), numPins);

    net->pmax.x = net->pmax.y = -1;
    net->pmin.x = net->pmin.y = -1;
    net->HPWL = -1;

    int netHashIdx = hash_function(net->netName);
    net->next = data->hashTable[netHashIdx].start;
    data->hashTable[netHashIdx].start = net;
    return net;
}


void destroy_net(net_ptr net)
{
    if (net->netName) free(net->netName);
    if (net->instArray) free(net->instArray);
    if (net->netPinArray) free(net->netPinArray);
    free(net);
}


netDB_ptr create_netDB(void)
{
    netDB_ptr data = (netDB_ptr)malloc(sizeof(struct NETDB));
    data->numNet = 0;
    data->curNumNet = 0;
    data->hashTable = (netHash_ptr)calloc(sizeof(struct NETHASH), default_hash_size);
    for (int i = 0; i < default_hash_size; i++)
    {
        data->hashTable[i].start = NULL;
    }
    data->netArray = NULL;
    return data;
}


void  destroy_netDB(netDB_ptr rmdb)
{
    for (int i = 0; i < default_hash_size; i++)
    {
        net_ptr sweep = rmdb->hashTable[i].start;
        while(sweep)
        {
            net_ptr sweep_next = sweep->next;
            destroy_net(sweep);
            sweep = sweep_next;
        }
    }
    free(rmdb->hashTable);
    if (rmdb->netArray) free(rmdb->netArray);
    free(rmdb);
}


// Utility function
net_ptr _get_net(netDB_ptr data, char* netName)
{
    int hashIdx = hash_function(netName);
    net_ptr sweep = data->hashTable[hashIdx].start;
    while(sweep)
    {
        if (!strcmp(sweep->netName, netName)) return sweep;
        sweep = sweep->next;
    }
    return NULL;
}


void print_allNets(netDB_ptr data)
{
    for (int i = 0; i < data->numNet; i++)
    {
        net_ptr net = data->netArray[i];
        printf("Net %s\n", net->netName);
    }
}
