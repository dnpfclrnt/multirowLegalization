#include "instance.h"


instance_ptr create_instance(char* instName, int sizeX, int sizeY, int numPins, int rowHeight, ...)
{
    // Allocate memory for current structure
    instance_ptr inst = (instance_ptr)malloc(sizeof(struct INSTANCE));

    inst->instName = strdup(instName);

    // Reset positions (defined in utils.h)
    resetFPOS(inst->fpmin);
    resetFPOS(inst->fcent);
    resetFPOS(inst->fpmax);

    resetPOS(inst->pmin);
    resetPOS(inst->cent);
    resetPOS(inst->pmax);

    inst->size.x = sizeX;
    inst->size.y = sizeY;

    inst->fsize.x = (prec)sizeX;
    inst->fsize.y = (prec)sizeY;

    inst->numPins = numPins;
    inst->instPinArray = (pin_ptr*)calloc(sizeof(pin_ptr), numPins);

    // Rowheight is saved for further calculation of rowIndex. 
    inst->rowHeight = rowHeight;
    inst->numRows = sizeY / rowHeight;
    inst->rowIdxArray = (int*)calloc(sizeof(int), inst->numRows);

    return inst;
}


void destroy_instance(instance_ptr inst)
{
    free(inst->instName);
    free(inst->instPinArray);
    free(inst->rowIdxArray);
    free(inst);
}


instDB_ptr create_instDB(int numInst)
{
    instDB_ptr data = (instDB_ptr)malloc(sizeof(struct INSTDB));
    data->numInst = numInst;
    data->instArray = (instance_ptr*)calloc(sizeof(instance_ptr), numInst);
    return data;
}


void destroy_instDB(instDB_ptr rmdb)
{
    for (int i = 0; i < rmdb->numInst; i++)
    {
        if (rmdb->instArray[i])
        {
            destroy_instance(rmdb->instArray[i]);
        }
    }
    free(rmdb);
}


void place_instance(instance_ptr inst, struct POS lowerLeft)
{
    struct FPOS fLowerLeft = posToFPOS(lowerLeft);
    inst->fpmin = fLowerLeft;
    setFPOS(inst->fcent, fLowerLeft, fpdiv(inst->fsize, 2.0));
    setFPOS(inst->fpmax, fLowerLeft, inst->fsize);

    inst->pmin = lowerLeft;
    setPOS(inst->cent, lowerLeft, pdiv(inst->size, 2));
    setPOS(inst->pmax, lowerLeft, inst->size);


    for (int i = 0; i < inst->numPins; i++)
    {
        pin_ptr pin = inst->instPinArray[i];
        setFPOS(pin->absPos, fLowerLeft, pin->pinPos);
    }


    int rowIdx = lowerLeft.x / inst->rowHeight;
    for (int i = 0; i < inst->numRows; i++)
    {
        inst->rowIdxArray[i] = rowIdx + i * inst->rowHeight;
    }
}