#ifndef BASIC_FUNCTIONS
#define BASIC_FUNCTIONS

#include <pthread.h>

typedef struct Arg_thread_TAG{

    long long timeToStart;
    int flag;
    int finishPlaying;
    int IDPlaying;

} Arg_thread;

#define TRUE   1
#define FALSE  0

#endif