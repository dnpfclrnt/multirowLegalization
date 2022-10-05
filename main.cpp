#include "src/db/database.h"


int main(int argc, char** argv)
{
    struct timeval start;
    char* filename = argv[1];
    gettimeofday(&start, NULL);
    database_ptr data = database_init(filename);
    struct timeval dataConstruction;
    gettimeofday(&dataConstruction, NULL);
    double time = stopwatch(&start, &dataConstruction);
    printf("Data construction time : %3lf\n", time);
    destroy_database(data);
    return 0;
}