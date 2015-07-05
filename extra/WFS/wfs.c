#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define c       343
#define pi      3.141592
#define FS      44100
#define land    -1 // porque está fuera del array de altavoces

//double comp      = 0;
//int ang_max     = 90*(pi/180);// rad

typedef struct ClientSpeakersTag{

    int speakers_number, chanels_number;
    float** list_positions_speakers;
    float* speakers_tecta;

} ClientSpeakers;

typedef struct WFSTag {

    int * parray;
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
        alfa[i] =atan2(difY[i],difX[i]);
    }

    for (i = 0; i < speakers.speakers_number; ++i) {
        alfa[i] = (alfa[i]*180/pi)+90-speakers.speakers_tecta[i];
    }

    //printf("\n\n Resultados obtenidos: \n\n");
    //printf("    alfa\t\t[%f %f %f %f]\n",alfa[0],alfa[1],alfa[2],alfa[3]);
    //printf("\n\n Resultados esperados: \n\n");
    //printf("    alfa\t\t[-71.565051177077990 -63.434948822922010 63.434948822922010 71.565051177077990]\n");


    int parray[4];
    int* pos = (int *)calloc(speakers.speakers_number, sizeof(int));;
    int sizeOfPos = 0;
    //memset(pos, -1, sizeof(int)*speakers.speakers_number);
    for (i = 0; i < speakers.speakers_number; ++i) {
        if ( ( ( alfa[i] < 90 ) && ( alfa[i] > -90 ) ) || ( ( alfa[i] < 450 ) && ( alfa[i] > 270 ) ) ){
            pos[i]=i;
            parray[i] = 1;
            sizeOfPos++;
        } else{
            pos[i]=-1;
            parray[i] = 0;
        }
    }

    int parray_act[sizeOfPos];
    int j = 0;
    for (i = 0; i < speakers.speakers_number; i++) {
        if(pos[i] > -1){
            parray_act[j] = pos[i];
            j++;
        }
    }

    //printf("\n\n Resultados obtenidos: \n\n");
    //printf("    parray\t\t[%d %d %d %d]\n",parray[0],parray[1],parray[2],parray[3]);
    //printf("    parray_act\t\t[%d %d %d %d]\n",parray_act[0],parray_act[1],parray_act[2],parray_act[3]);
    //printf("    pos\t\t\t[%d %d %d %d]\n",pos[0],pos[1],pos[2],pos[3]);
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
    float an[speakers.speakers_number];
    float tn[speakers.speakers_number];

    for (i = 0; i < speakers.speakers_number; ++i) {
        r = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );
        rr = (1.44/2+1.44*cos(45*pi/180));
        if(r<0){
            s = r*(-1);
        }else{
            s=r;
        }
        A = sqrt(rr/(rr+s));
        an[i] = A*cos(alfa[i]*(pi/180))/(sqrt(r));
        tn[i] =-land*(FS*(r/c));
    }



    WFS result;
    result.an = an;
    result.parray = parray_act;
    result.tn = tn;


    //printf("\n\n Resultados obtenidos: \n\n");
    //printf("    A\t\t\t[%f %f %f %f]\n",A[0],A[1],A[2],A[3]);
    //printf("    an\t\t\t[%f %f %f %f]\n",an[0],an[1],an[2],an[3]);
    //printf("    r\t\t\t[%f %f %f %f]\n",r[0],r[1],r[2],r[3]);
    //printf("    r0\t\t\t[%f %f %f %f]\n",rr[0],rr[1],rr[2],rr[3]);
    //printf("    s0\t\t\t[%f %f %f %f]\n",s[0],s[1],s[2],s[3]);
    //printf("    tn\t\t\t[%f %f %f %f]\n",tn[0],tn[1],tn[2],tn[3]);

    //printf("\n\n Resultados esperados: \n\n");
    //printf("    A\t\t\t[0.464313639232586 0.529048348552147 0.529048348552147 0.464313639232586]\n");
    //printf("    an\t\t\t[0.058384349182880 0.111880104145946 0.111880104145946 0.058384349182880]\n");
    //printf("    r\t\t\t[6.324555320336759 4.472135954999580 4.472135954999580 6.324555320336759]\n");
    //printf("    r0\t\t\t[1.738233764908628 1.738233764908628 1.738233764908628 1.738233764908628]\n");
    //printf("    s0\t\t\t[6.324555320336759 4.472135954999580 4.472135954999580 6.324555320336759]\n");
    //printf("    tn\t\t\t[8.131571126147262e+02 5.749889084999460e+02 5.749889084999460e+02 8.131571126147262e+02]\n");



    return result;
}

int main()
{

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

    waveFieldSynthesis(speakers,0.0,11.0);

    return 0;
}