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


struct FPOS{
    prec x;
    prec y;
};


struct POS{
    int x;
    int y;
};