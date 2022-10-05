#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <climits>
#include <cmath>
#include <ctime>
#include <limits>
#include <cfloat>
#include <sys/time.h>


// typedef float prec;
typedef double prec;
// typedef long double prec;

#define numThreads 16
#define default_hash_size 1024
#define hash_key 66337
#define max_line_length 4096


struct FPOS{
    prec x;
    prec y;
};


struct POS{
    int x;
    int y;
};


struct BBOX{
    struct POS pmin;
    struct POS pmax;
};


// Mode selection
#define DRV_VISUAL /*true*/ false
#define mode_x 0
#define mode_y 1
