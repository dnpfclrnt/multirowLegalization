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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
// shchung
#include <algorithm>
#include <limits>


// typedef float prec;
typedef double prec;
// typedef long double prec;


typedef unsigned long long cellArea;

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


struct INTARRAY{
    int* intArray;
    int len;
}; typedef INTARRAY* intArray_ptr;


// Mode selection
#define DRV_VISUAL /*true*/ false
#define mode_x 0
#define mode_y 1

#define mode_left 0
#define mode_right 1
#define mode_up 2
#define mode_down 3


// #define PROTOTYPE