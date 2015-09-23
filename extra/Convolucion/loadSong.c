#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <time.h>

#include <sys/time.h>                // for gettimeofday()

#include <limits.h>

#include "../../client/include/spatiallib.h"

#define c       343
#define pi      3.141592
#define FS      44100
#define land    -1 // porque está fuera del array de altavoces

//double comp      = 0;
//int ang_max     = 90*(pi/180);// rad

double H_1[61] ={-0.00661109583724264,-0.00650043529566125,-0.00632930310706349,-0.00617482490003971,-0.00602953993132536,-0.00598452022424796,-0.00604361199320415,-0.00630563700873370,-0.00677794258748761,-0.00755797763617839,-0.00864652310346542,-0.0101290944464563,-0.0119893471647713,-0.0142908353440042,-0.0169911140198601,-0.0201243563895989,-0.0236165930309936,-0.0274695564585432,-0.0315773165621993,-0.0359115018748141,-0.0403393430824099,-0.0448101568406542,-0.0491745430350945,-0.0533717619323007,-0.0572496004201327,-0.0607520798261281,-0.0637393605766290,-0.0661751281673016,-0.0679458772409232,-0.0690473584402903,2.29712671743954,-0.0690473584402903,-0.0679458772409232,-0.0661751281673016,-0.0637393605766290,-0.0607520798261281,-0.0572496004201327,-0.0533717619323007,-0.0491745430350945,-0.0448101568406542,-0.0403393430824099,-0.0359115018748141,-0.0315773165621993,-0.0274695564585432,-0.0236165930309936,-0.0201243563895989,-0.0169911140198601,-0.0142908353440042,-0.0119893471647713,-0.0101290944464563,-0.00864652310346542,-0.00755797763617839,-0.00677794258748761,-0.00630563700873370,-0.00604361199320415,-0.00598452022424796,-0.00602953993132536,-0.00617482490003971,-0.00632930310706349,-0.00650043529566125,-0.00661109583724264};


typedef struct SuperWAVTag{
    int* leido;
    unsigned char **filewav;

} SuperWAV;

typedef struct ClientSoundTag{

    const char *sound_folder;
    int word_length, sounds_number;
    char *sounds_list;

} ClientSound;

typedef struct ClientSpeakersTag{

    int speakers_number, chanels_number;
    float** list_positions_speakers;
    float* speakers_tecta;

} ClientSpeakers;


char * handleWAVFiles(ClientSound soundConfig){
    // -------------------------------------- HANDLE OF WAV FILES -------------------------------------//
    char * archivos_senal;

    /*Modificado la ruta de ./FicherosPrueba/001_piano.wav
    * para que lea desde una carpeta inferior
    * */
    archivos_senal= (char *)calloc(soundConfig.sounds_number* soundConfig.word_length, sizeof(char));

    int i;
    for (i = 0; i < soundConfig.sounds_number; ++i) {
        strcpy(archivos_senal + i*soundConfig.word_length, soundConfig.sounds_list+i*soundConfig.word_length);
    }

// -------------------------------------- HANDLE OF ALSA DEVICE -------------------------------------//

    return archivos_senal;
}

SuperWAV loadFile(ClientSound soundConfig){
    SuperWAV filewav;

    filewav.filewav = (unsigned char **) malloc (soundConfig.sounds_number*sizeof(unsigned char *));
    filewav.leido = (int*) malloc (soundConfig.sounds_number*sizeof(int));

    char * archivos_senal = handleWAVFiles(soundConfig);

    int i;
    for (i = 0; i < soundConfig.sounds_number; ++i) {
        filewav.leido[i] = OpenWavConvert32(&filewav.filewav[i],archivos_senal + i * soundConfig.word_length);
    }
    return filewav;
}

void convolve(unsigned char * signal, size_t signalLen, double* filter, size_t filterLen, double* result){
    size_t n;

    size_t kmin, kmax, k;
    double valor;
    for (n = 0; n < signalLen + filterLen - 1; n++)
    {
        result[n] = 0;

        //Optimización del recorrer.
        kmin = (n >= filterLen - 1) ? n - (filterLen - 1) : 0;
        kmax = (n < signalLen - 1) ? n : signalLen - 1;

        //printf("%d kmin, %d kmax\n",(int)kmin,(int)kmax);

        for (k = kmin; k <= kmax; k++)
        {
            valor = (double*) signal+k;
            printf("%f\n",valor);

            result[n] += valor * filter[n - k];
            exit(0);
        }
    }
}

// channels starts in 1.
void generateSongWFS(int** bufferToModify, int index, SuperWAV fileWAV, int songNumber, int buffSize, WFS values, int chanals) {

    int i;
    int j;

    for (j = 0; j < chanals; ++j) {
        if( NULL == bufferToModify[j] ) {
            bufferToModify[j] = (int *) calloc (buffSize , sizeof(int));
        }
    }

    int val;
    int itn = 0;
    int startPosBuffer = index*buffSize;
    int maxPos = 0;

    for (j = 0; j < chanals; ++j) {

        if(values.parray[j] == 1){
            itn = (int)ceil(values.tn[j]);
            maxPos = itn + (fileWAV.leido[songNumber]);
        }

        int actualPosBuff = 0;
        for (i = 0; i < buffSize; ++i) {
            val = 0;
            if(values.parray[j] == 1){

                actualPosBuff = i+startPosBuffer;

                if(itn <= actualPosBuff && actualPosBuff < maxPos){
                    val = (*((int *) fileWAV.filewav[songNumber] + actualPosBuff - itn ));
                }
            }

            bufferToModify[j][i] = (int)(values.an[j]*val);
        }
    }
}


int main()
{
    // ==================== inicial config ====================
    float tecta[4] = {0.0, 0.0, -90.0, -90.0};
    float listPos[4][4] = {{2.0,5.0},{2.0,7.0},{4.0,9.0},{6.0,9.0}};

    ClientSpeakers speakers;
    speakers.speakers_tecta = tecta;
    speakers.list_positions_speakers = (float **) malloc(sizeof(float *) *4);

    speakers.list_positions_speakers[0] = listPos[0];
    speakers.list_positions_speakers[1] = listPos[1];
    speakers.list_positions_speakers[2] = listPos[2];
    speakers.list_positions_speakers[3] = listPos[3];

    speakers.speakers_number = 4;
    speakers.chanels_number = 4;


    ClientSound sound;
    sound.sounds_number = 4;
    sound.word_length = 50;
    sound.sound_folder = "../../bin/sound/";
    sound.sounds_list = (char *) calloc(sound.sounds_number * sound.word_length, sizeof(char));;

    strcpy(sound.sounds_list + 0 * sound.word_length, "../../bin/sound/001_piano.wav");
    strcpy(sound.sounds_list + 1 * sound.word_length, "../../bin/sound/voz4408.wav");
    strcpy(sound.sounds_list + 2 * sound.word_length, "../../bin/sound/001_bajo.wav");
    strcpy(sound.sounds_list + 3 * sound.word_length, "../../bin/sound/001_bateriabuena.wav");
    //strcpy(sound.sounds_list + 3 * sound.word_length, "../../bin/sound/001_piano.wav");


    SuperWAV fileWAV = loadFile(sound);

    size_t resultLength = fileWAV.leido[0] + 61 - 1;
    double *C = malloc( resultLength*sizeof(double) );

    clock_t tic,toc;

    tic = clock();
    convolve(fileWAV.filewav[0], fileWAV.leido[0], H_1, 61, C);
    toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    exit(0);
}