#include "src/db/database.h"
#include "src/lg/legalization.h"


int main(int argc, char** argv)
{
    using namespace std;
    struct timeval start;
    char* filename = argv[1];
    gettimeofday(&start, NULL);
    database_ptr data = database_init(filename);
    struct timeval dataConstruction;
    gettimeofday(&dataConstruction, NULL);
    double data_construction_time = stopwatch(&start, &dataConstruction);
    printf("=======================================\n");
    printf("Data construction time : %3lf sec\n", data_construction_time);
    printf("=======================================\n");
    initHPWL(data);
    int HPWL_first = getHPWL(data);
    placmenet_map_init(data);
    std::string lg_mode = "SIZE";
    // std::string lg_mode = "DEFAULT";
    #ifdef PROTOTYPE
    intArray_ptr arr = legalize(data, (char*)lg_mode.c_str());
    check_legality_global(data, arr);
    #else
    legalize(data, (char*)lg_mode.c_str());
    check_legality_global(data);
    #endif

    printf("=======================================\n");
    printf("############### SUMMARY ###############\n");
    struct timeval lg_finish;
    gettimeofday(&lg_finish, NULL);
    double runtime = stopwatch(&start, &lg_finish);
    printf("Total runtime : %3lf sec\n", runtime);
    int* disp = (int*)malloc(sizeof(int) * data->inst_data->numInst);
    int maxDisplacement = -1;
    int totalDisplacement = 0;
    for (int i = 0; i < data->inst_data->numInst; i++)
    {
        disp[i] = abs(data->inst_data->initialPOS[i].x - data->inst_data->instArray[i]->pmin.x);
        disp[i] += abs(data->inst_data->initialPOS[i].y - data->inst_data->instArray[i]->pmin.y);
        if (maxDisplacement < disp[i])
        {
            maxDisplacement = disp[i];
        }
        totalDisplacement += disp[i];
    }
    initHPWL(data);
    int HPWL = getHPWL(data);
    printf("Total Displacement\t=\t%d\n", totalDisplacement);
    printf("Max Displacement\t=\t%d\n", maxDisplacement);
    printf("HPWL Difference \t=\t%d\n", HPWL - HPWL_first);
    printf("HPWL %u --> %u\n", HPWL_first, HPWL);
    printf("=======================================\n");

    if (argc == 3)
    {
        string a = "PLOT";
        char* plot_keyword = (char*)a.c_str();
        if (!strcmp(plot_keyword, argv[2]))
        {
            for (int i = 0; i < data->inst_data->numInst; i++)
            {
                printf("PLOT %d\n", disp[i]);
            }
        }
    }


    destroy_database(data);
    return 0;
}