#ifndef BASIC_FUNCTIONS
#define BASIC_FUNCTIONS

#include <pthread.h>

typedef struct Player_TAG{

    long long timeToStart;
    int flag;
    int finishPlaying;
    int IDPlaying;
    pthread_mutex_t lock;
    int l; //pointer
    void **bufs;

} Player;

#define TRUE   1
#define FALSE  0

#endif