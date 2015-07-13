#ifndef PROCESSLIB_H
#define PROCESSLIB_H

#include "spatiallib.h"
#include "configurationlib.h"

typedef struct SuperWAVTag{
    int* leido;
    unsigned char **filewav;

} SuperWAV;

typedef struct PlayerTAG{

    long long timeToStart;
    float * client_pos;
    float ** songPos;


    int flag;
    int finishPlaying;
    int IDPlaying;

    pthread_mutex_t lock;

    void **bufferToPlay;

    ClientCard card;
    ClientSound sound;
    ClientSpeakers speakers;
    int timeToStrartSeconds;

} Player;

typedef struct WFSTag {

    int * parray;
    int * pos;
    float* tn;
    float* an;

} WFS;


WFS waveFieldSynthesis(ClientSpeakers speakers, char* song, float soundPosX, float soundPosY );

char * handleWAVFiles(ClientSound soundConfig);
SuperWAV loadFile(ClientSound soundConfig);

void bufferGenerator(int** bufferToModify, int index,SuperWAV fileWAV,int buffSize, double **WFS, int chanals);
void** castBufferToVoid(int** buffer, int chanals);



#endif


