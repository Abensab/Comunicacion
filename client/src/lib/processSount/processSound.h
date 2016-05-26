#ifndef PROCESSLIB_H
#define PROCESSLIB_H

#include "../spacial/spatial.h"
#include "../configuration/configuration.h"

typedef struct SuperWAVTag{
    int* leido;
    unsigned char **filewav;

} SuperWAV;

typedef struct WFSTag {

    int * parray;
    int * pos;
    float* tn;
    float* an;

} WFS;

typedef struct PlayerTAG{

    long long timeToStart;
    float * client_pos;
    float ** songPos;

    WFS * wfsVector;

    int l1;
    int finishPlaying;
    int IDPlaying;

    pthread_mutex_t lock;

    int **bufferToPlay;

    ClientCard card;
    ClientSound sound;
    ClientSpeakers speakers;
    int timeToStrartSeconds;

    SuperWAV fileWAV;


} Player;

WFS waveFieldSynthesis(ClientSpeakers speakers, float soundPosX, float soundPosY );

char * handleWAVFiles(ClientSound soundConfig);
SuperWAV loadFile(ClientSound soundConfig);

void generateSongWFS(int** bufferToModify, int index,SuperWAV fileWAV, int songNumber, int buffSize, WFS values, int chanals);

void setBuffMem(int** bufferToStore,  int bufferSize, int chanelsNumber);
void addToBuffer(int** bufferToStore, int** bufferToAdd,  int bufferSize, int chanelsNumber);


void** castBufferToVoid(int** buffer, int chanals);



#endif


