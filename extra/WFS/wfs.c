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

//double comp      = 0;
//int ang_max     = 90*(pi/180);// rad

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

    //printf("\n\n Resultados obtenidos: \n\n");
    //printf("    alt\t\t\t[%f %f %f %f;%f %f %f %f]\n",speakers.list_positions_speakers[0][0],
    //       speakers.list_positions_speakers[1][0],
    //       speakers.list_positions_speakers[2][0],
    //       speakers.list_positions_speakers[3][0],
    //       speakers.list_positions_speakers[0][1],
    //       speakers.list_positions_speakers[1][1],
    //       speakers.list_positions_speakers[2][1],
    //       speakers.list_positions_speakers[3][1]);
    //printf("\n\n Resultados esperados: \n\n");
    //printf("    alt\t\t\t[2 2 4 6;5 7 9 9]\n");


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


    //printf("\n\n Resultados obtenidos: \n\n");
    //printf("    fte\t\t\t[%f %f]\n",fte[0],fte[1]);
    //printf("    x\t\t\t%f\n",x);
    //printf("    y\t\t\t%f",y);
    //printf("\n\n Resultados esperados: \n\n");
    //printf("    fte\t\t\t[0;11]\n");
    //printf("    x\t\t\t0\n");
    //printf("    y\t\t\t11\n");


    float difX[speakers.speakers_number];
    float difY[speakers.speakers_number];

    for (i = 0; i < speakers.speakers_number; ++i) {
        difX[i] = speakers.list_positions_speakers[i][0]-x;
        difY[i] = speakers.list_positions_speakers[i][1]-y;

        //printf("===> %f - %f = %f\n",speakers.list_positions_speakers[i][0], x, difX[i]);
    }

    //for (i = 0; i < speakers.speakers_number; ++i) {
        //printf("===> %f - %f = %f\n",speakers.list_positions_speakers[i][1], y, difY[i]);
    //}

    //printf("\n\n Resultados obtenidos: \n\n");
    //printf("    difX\t\t[%f %f %f %f]\n",difX[0],difX[1],difX[2],difX[3]);
    //printf("    difY\t\t[%f %f %f %f]\n",difY[0],difY[1],difY[2],difY[3]);
    //printf("\n\n Resultados esperados: \n\n");
    //printf("    difX\t\t[2 2 4 6]\n");
    //printf("    difY\t\t[-6 -4 -2 -2]\n");


    float alfa[speakers.speakers_number];// Ángulo //necesito esto
    for (i = 0; i < speakers.speakers_number; ++i) {
        alfa[i] = atan2(difY[i],difX[i]);
    }

    for (i = 0; i < speakers.speakers_number; ++i) {
        alfa[i] = (alfa[i]*180/pi)+90-speakers.speakers_tecta[i];
    }

    //printf("\n\n Resultados obtenidos: \n\n");
    //printf("    alfa\t\t[%f %f %f %f]\n",alfa[0],alfa[1],alfa[2],alfa[3]);
    //printf("\n\n Resultados esperados: \n\n");
    //printf("    alfa\t\t[-71.565051177077990 -63.434948822922010 63.434948822922010 71.565051177077990]\n");


    int sizeOfPos = 0;
    //memset(pos, -1, sizeof(int)*speakers.speakers_number);
    for (i = 0; i < speakers.speakers_number; ++i) {
        if ( ( ( alfa[i] < 90 ) && ( alfa[i] > -90 ) ) || ( ( alfa[i] < 450 ) && ( alfa[i] > 270 ) ) ){
            result.pos[i]=i;
            result.parray[i] = 1;
            sizeOfPos++;
        } else{
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
    //printf("\n\n Resultados obtenidos: \n\n");
    printf("    parray\t\t[%d %d %d %d]\n",result.parray[0],result.parray[1],result.parray[2],result.parray[3]);
    //printf("    parray_act\t\t[%d %d %d %d]\n",parray_act[0],parray_act[1],parray_act[2],parray_act[3]);
    //printf("    pos\t\t\t[%d %d %d %d]\n",result.pos[0],result.pos[1],result.pos[2],result.pos[3]);
    //printf("\n\n Resultados esperados: \n\n");
    //printf("    parray\t\t[1 1 1 1]\n");
    //printf("    parray_act\t[1 2 3 4]\n");
    //printf("    pos\t\t\t[1 2 3 4]\n");


/*
    float r[speakers.speakers_number];// Ángulo
    float rr[speakers.speakers_number];
    float s[speakers.speakers_number];
    float A[speakers.speakers_number];
    float an[speakers.speakers_number];
    float tn[speakers.speakers_number];

    for (i = 0; i < speakers.speakers_number; ++i) {
        r[i] = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );


        rr[i] = (1.44/2+1.44*cos(45*pi/180));

        if(r[i]<0){
            s[i] = r[i]*(-1);
        }else{
            s[i]=r[i];
        }

        A[i] = sqrt(rr[i]/(rr[i]+s[i]));

        an[i] = A[i]*cos(alfa[i]*(pi/180))/(sqrt(r[i]));

        tn[i] =-land*(FS*(r[i]/c));
    }
*/
    float r;
    float rr;
    float s;
    float A;

    for (i = 0; i < speakers.speakers_number; ++i) {
        r = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );
        rr = (1.44/2+1.44*cos(45*pi/180));
        if(r<0){
            s = r*(-1);
        }else{
            s=r;
        }

        A = sqrt(rr/(rr+s));
        result.an[i] = A*cos(alfa[i]*(pi/180))/(sqrt(r));
        result.tn[i] =-land*(FS*(r/c));
    }


    //printf("\n\n Resultados obtenidos: \n\n");
    //printf("    A\t\t\t[%f %f %f %f]\n",A[0],A[1],A[2],A[3]);
    printf("    an\t\t\t[%f %f %f %f]\n", result.an[0], result.an[1], result.an[2], result.an[3]);
    //printf("    r\t\t\t[%f %f %f %f]\n",r[0],r[1],r[2],r[3]);
    //printf("    r0\t\t\t[%f %f %f %f]\n",rr[0],rr[1],rr[2],rr[3]);
    //printf("    s0\t\t\t[%f %f %f %f]\n",s[0],s[1],s[2],s[3]);
    printf("    tn\t\t\t[%f %f %f %f]\n", result.tn[0], result.tn[1], result.tn[2], result.tn[3]);

    //printf("\n\n Resultados esperados: \n\n");
    //printf("    A\t\t\t[0.464313639232586 0.529048348552147 0.529048348552147 0.464313639232586]\n");
    //printf("    an\t\t\t[0.058384349182880 0.111880104145946 0.111880104145946 0.058384349182880]\n");
    //printf("    r\t\t\t[6.324555320336759 4.472135954999580 4.472135954999580 6.324555320336759]\n");
    //printf("    r0\t\t\t[1.738233764908628 1.738233764908628 1.738233764908628 1.738233764908628]\n");
    //printf("    s0\t\t\t[6.324555320336759 4.472135954999580 4.472135954999580 6.324555320336759]\n");
    //printf("    tn\t\t\t[8.131571126147262e+02 5.749889084999460e+02 5.749889084999460e+02 8.131571126147262e+02]\n");



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

void bufferGenerator(int** bufferToModify, int index,SuperWAV fileWAV,int buffSize, WFS values, int chanals) {

    int i;
    int j;

    for (j = 0; j < chanals; ++j) {
        if( NULL == bufferToModify[j] ) {
            bufferToModify[j] = (int *) malloc (buffSize * sizeof(int));
        }
    }

    for (j = 0; j < chanals; ++j) {
        printf("BrakPoint%d\n",j);
        for (i = 0; i < buffSize; ++i) {
            //printf("===> %d, j = %d\n", i - (int)ceil(values.tn[j]), j );


            int val2 = (*((int *) fileWAV.filewav[j] + (index * buffSize) + i ));
            //int val = (*((int *) fileWAV.filewav[j] + (index * buffSize) + (i - ( (int)ceil(values.tn[j]) )));

            //bufferToModify[j][i] = val; //por an1

            printf("val: %d\t%d\n",val2,val2);


            //printf("BrakPoint %d.%d\n",j,i);
        }
    }
}

// channels starts in 1.
void generateSongWFS(int** bufferToModify, int index,SuperWAV fileWAV, int songNumber, int buffSize, WFS values, int chanals) {

    int i;
    int j;

    for (j = 0; j < chanals; ++j) {
        if( NULL == bufferToModify[j] ) {
            bufferToModify[j] = (int *) malloc (buffSize * sizeof(int));
        }
    }

    int itn = 0;
    int val = 0;
    int startPosBuffer = index*buffSize;
    int maxPos = 0;

    for (j = 0; j < chanals; ++j) {
            //printf("BrakPoint channel %d\n",j);
            //itn = values.tn[j];
            //itn += 1 ;
            if(values.parray[j] == 1){
                itn = ceil(values.tn[j]);
                maxPos = itn + (fileWAV.leido[songNumber]);
            }


            //printf("posBuffer %d, maxPos %d, leido %d, itn %d \n", startPosBuffer, maxPos, fileWAV.leido[songNumber], itn);

         /*   if(itn <= posBuffer){
                printf("itn < posBuffer ==> %d, %d\n",itn, posBuffer);
            }

            if(posBuffer < maxPos){
                printf("\t\tposBuffer < maxPos ==> %d, %d\n",maxPos, posBuffer+512);
            }
*/
            int actualPosBuff = 0;
            for (i = 0; i < buffSize; ++i) {

                if(values.parray[j] == 1){

                    actualPosBuff = i+startPosBuffer;

                    // printf(" values tn %f \n", values.tn[j] );
                    //int ival = ( (int)values.tn[j] ) + 1;
                    //printf("itn %d\n",itn);
                    //printf("tn tn %f \n", values.tn[j]);
                    // printf(" DelBuf %d DelChannel %d  Retardo %d \n", i,j, (int)ceil(values.tn[j]) );

                    //if(itn <= actualPosBuff && actualPosBuff < maxPos ){
                        //val = *(int *) (fileWAV.filewav[songNumber]  + posBuffer + i - itn ); // + (i - itn ) ) ) );
                        //val = (*((int *) fileWAV.filewav[j] + (index * buffSize) + (i - itn) ));
                    //}

                    if(itn <= actualPosBuff && actualPosBuff < maxPos){
                        val = (*((int *) fileWAV.filewav[j] + actualPosBuff - itn ));
                    }

                    //printf("value: %d\t\t an: %f\t\t value*an: %f \n",val, values.an[j] ,values.an[j]*val);
                    //valores no exactos: value: 16252928		 an: 0.111880		 value*an: 1818380.500000
                    //Exacto: 1818377,58464

                    printf("valueInt: %d\t\t valueFloat: %f\t\t an: %f\t\t value*an: %f \n",val,((double)val),(float)values.an[j] ,(double)values.an[j]*((double)val));


                    /*if(actualPosBuff > maxPos){
                            printf("posBuffer > maxPos ==> %d, %d, resta = %d \n",actualPosBuff,maxPos, maxPos - actualPosBuff);
                            break;
                    }*/
                    //printf("%d; %d;",actualPosBuff,val);



                    /*if(actualPosBuff <= 7849800){
                        //printf("posBuffer > maxPos ==> %d, %d, resta = %d\n",actualPosBuff,maxPos, maxPos - actualPosBuff);
                        //printf("Final Canción, index = %d\n",index);
                        int val2 = (*((int *) fileWAV.filewav[j] + (index * buffSize) + i ));
                        printf("%d;\n",val2);
                    }else{
                        printf("-1;\n");
                    }*/

                    /*if(actualPosBuff == 7849800){
                        printf("Final Canción, index = %d, pos = %d\n",index,actualPosBuff);
                    }*/
                }

                bufferToModify[j][i] = val; //por an1

            }

        }

}


int main()
{
    // ==================== inicial config ====================
    float tecta[4] = {90.0,90.0,0.0,0.0};
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

    int* pruebaBuffGnerado[speakers.chanels_number];
    int j;
    for (j = 0; j < speakers.chanels_number; ++j) {
        pruebaBuffGnerado[j] = NULL;
    }

    // ==================== fin inicial config ====================


    //struct timeval t1, t2;
    //double elapsedTime;

    //start timer
    //gettimeofday(&t1, NULL);


    WFS resultWFS = waveFieldSynthesis(speakers,0.0,11.0);

    // stop timer
    //gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    //elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    //elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    //printf(" -- PASADO %f -- \n", elapsedTime);


    //printf("%d, %d, %d, %d\n",fileWAV.leido[0],fileWAV.leido[1],fileWAV.leido[2],fileWAV.leido[3]);

    //printf("%d \n",fileWAV.leido[0]);

    int maxLenghFile = getMaxInt(fileWAV.leido, sound.sounds_number);
    int maxDellay = ceil(getMaxFloat(resultWFS.tn, speakers.speakers_number));
    //printf(" ==> %d\n",maxLenghFile+maxDellay);
    //printf(" ==> %d\n",(maxLenghFile+maxDellay)/512);

    int l1;
    for(l1 = 0; l1 < (maxLenghFile+maxDellay)/512 ; l1++){

        generateSongWFS(pruebaBuffGnerado,l1,fileWAV, 0, 512, resultWFS, 4);
        maxDellay = ceil(getMaxFloat(resultWFS.tn, speakers.speakers_number));

    }


    exit(0);
}