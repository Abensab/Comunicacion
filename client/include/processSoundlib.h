#ifndef PROCESSLIB_H
#define PROCESSLIB_H

#include "spatiallib.h"
#include "configurationlib.h"

typedef struct SuperWAVTag{
    //int leido1;
    //int leido2;
    int* leido;
    unsigned char **filewav;

} SuperWAV;

typedef struct PlayerTAG{

    long long timeToStart;
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

double **WFS(double posX, double posY);

char * handleWAVFiles(ClientSound soundConfig);
SuperWAV loadFile(ClientSound soundConfig, ClientSpeakers speakersConfig);

void bufferGenerator(int** bufferToModify, int index,SuperWAV fileWAV,int buffSize, double **WFS, int chanals);
void** castBufferToVoid(int** buffer, int chanals);



#endif


