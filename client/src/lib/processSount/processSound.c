
//to round supperior
#include <tgmath.h>

#include "processSound.h"

#define WORD_LENGTH 50

/*Filtro de altavoces*/

#define c       343
#define pi      3.141592
#define FS      44100
#define land    -1 // porque está fuera del array de altavoces

double comp      = 0;
int ang_max     = 90*(pi/180);// rad
int nalt        = 96; //numero de altavoces

WFS waveFieldSynthesis(ClientSpeakers speakers, float x, float y ){

       WFS result;
       result.parray = (int *)calloc(speakers.speakers_number, sizeof(int));;
       result.pos = (int *)calloc(speakers.speakers_number, sizeof(int));;

       result.tn = (float *)malloc(speakers.speakers_number*sizeof(float));
       result.an = (float *)malloc(speakers.speakers_number*sizeof(float));

       int i;

       //Guardar diferencia de pozicion
       float * difX = (float *) calloc (speakers.speakers_number, sizeof(float));
       float * difY = (float *) calloc (speakers.speakers_number, sizeof(float));
       //Guardar angulos
       float * alfa=(float *) calloc (speakers.speakers_number, sizeof(float));

       //Calcular diferencia de poziciones x,y
       for (i = 0; i < speakers.speakers_number; ++i) {
           difX[i] = speakers.list_positions_speakers[i][0]-x;
           difY[i] = speakers.list_positions_speakers[i][1]-y;
       }

       //Calcular angulo resultante
       for (i = 0; i < speakers.speakers_number; ++i) {
           alfa[i] = atan2(difY[i],difX[i])*180/pi;
       }

       int sizeOfPos = 0;
       //memset(pos, -1, sizeof(int)*speakers.speakers_number);
       for (i = 0; i < speakers.speakers_number; ++i) {
           //printf("%f=> %f <= %f : ",speakers.speakers_tecta[i]+90,alfa[i],speakers.speakers_tecta[i]-90  );
           if ( ( ( alfa[i] <= speakers.speakers_tecta[i]+90 ) && ( alfa[i] >= speakers.speakers_tecta[i]-90 ) ) ){
               //printf("DENTRO\n");
               result.pos[i]=i;
               result.parray[i] = 1;
               sizeOfPos++;
           }else{
               //printf("FUERA\n");
               result.pos[i]=-1;
               result.parray[i] = 0;
           }
       }

   /*    int parray_act[sizeOfPos];
       int j = 0;
       for (i = 0; i < speakers.speakers_number; i++) {
           if(result.pos[i] > -1){
               parray_act[j] = result.pos[i];
               j++;
           }
       }
   */

       float r;

       for (i = 0; i < speakers.speakers_number; ++i) {
           r = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );
           result.an[i] = 1/(sqrt(r));
           result.tn[i] =-land*(FS*(r/c));
       }
/*
       printf("\n\n Resultados obtenidos: \n\n");
       for (i = 0; i < speakers.speakers_number; ++i) {
            printf("    an\t[%5.f]\t", result.an[i]);
            printf("    tn\t[%5.f]\t", result.tn[i]);
            printf("    pos\t[%d]\t", result.pos[i]);
            printf("    parray\t[%d]\n", result.parray[i]);
       }
*/
       return result;
   }

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
        filewav.leido[i] = OpenWavConvert32(&filewav.filewav[i],archivos_senal + i*soundConfig.word_length);
    }
    return filewav;
}

// channels starts in 1.
void generateSongWFS(int** bufferToModify, int index, SuperWAV fileWAV, int songNumber, int buffSize, WFS values, int chanals) {

    int i;
    int j;

    for (j = 0; j < chanals; ++j) {
        if( NULL == bufferToModify[j] ) {
            //bufferToModify[j] = (int *) malloc (buffSize * sizeof(int));
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
                    //val = (*((int *) fileWAV.filewav[j] + actualPosBuff - itn ));
                    val = (*((int *) fileWAV.filewav[songNumber] + actualPosBuff - itn ));
                }
            }

            //printf("valueInt: %d\t\t value*an: %d\t\t an: %f\t\t valueIni: %d \n",val, (int)(values.an[j]*val),
            //        (float)values.an[j] ,bufferToModify[j][i]);


            bufferToModify[j][i] = (int)(values.an[j]*val);
            //bufferToModify[j][i] += (int)(values.an[j]*val);

            //if(j == 0){
            //    printf("val: %d\t parray: %d\t bufferValue: %d\n", val, values.parray[j], bufferToModify[j][i]);
            //}
        }
    }
}

void setBuffMem(int** bufferToStore, int bufferSize, int chanelsNumber){
    int j;

    for (j = 0; j < chanelsNumber; ++j) {
        if( NULL == bufferToStore[j] ) {
            bufferToStore[j] = (int *) calloc (bufferSize , sizeof(int));
        }else{
            memset(bufferToStore[j] , 0, sizeof(int) * bufferSize);
        }
    }
}

void addToBuffer(int** bufferToStore, int** bufferToAdd,  int bufferSize, int chanelsNumber){
    int i, j;

    for(j = 0; j < chanelsNumber; j++){
        for (i = 0; i < bufferSize; ++i) {
            //printf("channel: %d\t bufferToAdd: %d\t bufferToStore: %d\t", j, bufferToAdd[j][i], bufferToStore[j][i]);
            bufferToStore[j][i] = bufferToStore[j][i] + bufferToAdd[j][i];
            //printf("new bufferToStore: %d\n", bufferToStore[j][i]);
        }
    }
}

void** castBufferToVoid(int** buffer, int chanals){

    int i;
    for (i = 0; i < chanals; ++i) {
        buffer[i] = (void *) buffer[i];
    }
    return (void**)buffer;
}