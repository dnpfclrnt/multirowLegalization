#include "src/db/database.h"


int main(int argc, char** argv)
{
    struct timeval start;
    char* filename = argv[1];
    gettimeofday(&start, NULL);
    database_ptr data = database_init(filename);
    struct timeval dataConstruction;
    gettimeofday(&dataConstruction, NULL);
    double data_construction_time = stopwatch(&start, &dataConstruction);
    printf("=======================================\n");
    printf("Data construction time : %3lf\n", data_construction_time);
    printf("=======================================\n");

    // print_allInst(data->inst_data);
    // print_allNets(data->net_data);
    // print_allPorts(data->port_data);

    destroy_database(data);
    return 0;
}