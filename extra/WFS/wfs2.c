#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/time.h>                // for gettimeofday()

#include <limits.h>

#include "../../client/include/spatiallib.h"

#define c       343
#define pi      3.141592
#define FS      44100
#define land    -1 // porque está fuera del array de altavoces

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

typedef struct WFSTag {

    int * parray;
    int * pos;
    float* tn;
    float* an;

} WFS;

WFS waveFieldSynthesis(ClientSpeakers speakers, float posX, float posY ){

    WFS result;
    result.parray = (int *)calloc(speakers.speakers_number, sizeof(int));;
    result.pos = (int *)calloc(speakers.speakers_number, sizeof(int));;

    result.tn = (float *)malloc(speakers.speakers_number*sizeof(float));
    result.an = (float *)malloc(speakers.speakers_number*sizeof(float));

    int i;

    float fte[2]={posX,posY};

    float *fuente = fte;
    float x = fuente[0];
    float y = fuente[1];


    float difX[speakers.speakers_number];
    float difY[speakers.speakers_number];

    for (i = 0; i < speakers.speakers_number; ++i) {
        difX[i] = speakers.list_positions_speakers[i][0]-x;
        difY[i] = speakers.list_positions_speakers[i][1]-y;
    }

    float alfa[speakers.speakers_number];// Ángulo //necesito esto
    for (i = 0; i < speakers.speakers_number; ++i) {
        alfa[i] = atan2(difY[i],difX[i])*180/pi;
    }


    int sizeOfPos = 0;
    for (i = 0; i < speakers.speakers_number; ++i) {
        if ( ( ( alfa[i] <= speakers.speakers_tecta[i]+90 ) && ( alfa[i] >= speakers.speakers_tecta[i]-90 ) ) ){
            result.pos[i]=i;
            result.parray[i] = 1;
            sizeOfPos++;
        }else{
            result.pos[i]=-1;
            result.parray[i] = 0;
        }
    }

    float r;

    for (i = 0; i < speakers.speakers_number; ++i) {
        r = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );
        result.an[i] = 1/(sqrt(r));
        result.tn[i] =-land*(FS*(r/c));
    }

 /*   printf("parray_act\t\t[");
    for (i = 0; i < speakers.speakers_number; i++) {
        printf("%d ",result.pos[i]);
    }
    printf("]\n");

    printf("    an\t\t\t[");
    for (i = 0; i < speakers.speakers_number; i++) {
        printf("%f ",result.an[i]);
    }
    printf("]\n");

    printf("    tn\t\t\t[");
    for (i = 0; i < speakers.speakers_number; i++) {
        printf("%f ",result.tn[i]);
    }
    printf("]\n");
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
        filewav.leido[i] = OpenWavConvert32(&filewav.filewav[i],archivos_senal + i * soundConfig.word_length);
    }
    return filewav;
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
        //printf("Break1\n");
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
    

    SuperWAV fileWAV = loadFile(sound);

    int** pruebaBuffGnerado= (int **) malloc(sizeof(int *) * speakers.chanels_number);

    int j;
    for (j = 0; j < speakers.chanels_number; ++j) {
        pruebaBuffGnerado[j] = NULL;
    }

    // ==================== fin inicial config ====================


    struct timeval t1, t2;
    double elapsedTime;

    //start timer
    //gettimeofday(&t1, NULL);


    WFS resultWFS = waveFieldSynthesis(speakers,0.0,11.0);

    // stop timer
    //gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    //elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    //elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    //printf(" --WFS PASADO %f -- \n", elapsedTime);


    //printf("%d, %d, %d, %d\n",fileWAV.leido[0],fileWAV.leido[1],fileWAV.leido[2],fileWAV.leido[3]);

    //printf("%d \n",fileWAV.leido[0]);

    //int maxLenghFile = getMaxInt(fileWAV.leido, sound.sounds_number);
    //int maxDellay = ceil(getMaxFloat(resultWFS.tn, speakers.speakers_number));
    //printf(" ==> %d\n",maxLenghFile+maxDellay);
    //printf(" ==> %d\n",(maxLenghFile+maxDellay)/512);

    int l1;
    int soundIteration;
    //for(l1 = 0; l1 < (maxLenghFile+maxDellay)/512 ; l1++){

    for(l1 = 0; l1 < 10; l1++){

	//start timer
    	gettimeofday(&t1, NULL);
        for(soundIteration = 0; soundIteration < sound.sounds_number; soundIteration++){
            generateSongWFS(pruebaBuffGnerado,l1,fileWAV, soundIteration, 512, resultWFS, 4);
            //maxDellay = ceil(getMaxFloat(resultWFS.tn, speakers.speakers_number));    
        }
        
	
	// stop timer
    	gettimeofday(&t2, NULL);

    	// compute and print the elapsed time in millisec
    	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    	printf("%d, %f \n",l1, elapsedTime);
    }

    // ========================================

    printf(" ============== \n");

    int speakers_increase;
    int z;
    
    int** BuffGnerado= (int **) malloc(sizeof(int *) * speakers.chanels_number);



    for(speakers_increase = 2; speakers_increase <= 100; speakers_increase += 2){

        printf("speakers number: %d \n",speakers_increase);
        speakers.speakers_tecta = (float *) malloc(sizeof(float) * speakers_increase);
        speakers.list_positions_speakers = (float **) malloc(sizeof(float *) * speakers_increase);
        
        for(z = 0; z < speakers_increase; z++){

            speakers.list_positions_speakers[z] = listPos[z%4];
            speakers.speakers_tecta[z] = tecta[z%4];

            speakers.speakers_number = speakers_increase;
            speakers.chanels_number = speakers_increase;

            BuffGnerado = (int **) malloc(sizeof(int *) * speakers.chanels_number);

                int j;
                for (j = 0; j < speakers.chanels_number; ++j) {
                    BuffGnerado[j] = NULL;
                }

        }

        resultWFS = waveFieldSynthesis(speakers,0.0,11.0);

        for(l1 = 0; l1 < 10; l1++){
        //start timer
            gettimeofday(&t1, NULL);
            for(soundIteration = 0; soundIteration < sound.sounds_number; soundIteration++){
                //generateSongWFS(BuffGnerado,l1,fileWAV, soundIteration, 512, resultWFS, speakers.chanels_number);
                //generateSongWFS(BuffGnerado,l1,fileWAV, soundIteration, 1024, resultWFS, speakers.chanels_number);
                generateSongWFS(BuffGnerado,l1,fileWAV, soundIteration, 2048, resultWFS, speakers.chanels_number);
            }
            

        // stop timer
            gettimeofday(&t2, NULL);

            // compute and print the elapsed time in millisec
            elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
            elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
            printf("%d; %f \n",l1, elapsedTime);
        }

    }

    exit(0);
}
