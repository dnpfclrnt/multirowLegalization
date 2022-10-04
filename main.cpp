#include "src/db/database.h"


int main(int argc, char** argv)
{
    char* filename = argv[0]
    database_ptr data = database_init(filename);
    destroy_database(data);
    return 0;
}