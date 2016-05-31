//
// Created by myhay on 31/05/16.
//

#include "angleTest.h"

/*Filtro de altavoces*/

#define c       343
#define pi      3.141592
#define FS      44100
#define land    -1 // porque está fuera del array de altavoces

double comp      = 0;
int ang_max     = 90*(pi/180);// rad
int nalt        = 96; //numero de altavoces

int main(){
    int i;

    //float speakers_tecta[4] = {0.0, 0.0, -90.0, -90.0};
    //float list_positions_speakers[4][2] = {{2.0,5.0},{2.0,7.0},{4.0,9.0},{6.0,9.0}};

    float speakers_tecta[2] = {0.0, 180.0};
    float list_positions_speakers[2][2] = {{2.0,0.0},{4.0,0.0}};

    int speakers_number = 2;

    float * difX = (float *) calloc (speakers_number, sizeof(float));
    float * difY = (float *) calloc (speakers_number, sizeof(float));

    float * alfa=(float *) calloc (speakers_number, sizeof(float));

    printf("X: ");
    float x;
    scanf("%f", &x);
    printf("Y: ");
    float y;
    scanf("%f", &y);


    while(x != 1000){

        printf("\nResultados:\n");
        for (i = 0; i < 4; ++i) {
            difX[i] = list_positions_speakers[i][0]-x;
            difY[i] = list_positions_speakers[i][1]-y;
        }

        // Ángulo necesito esto
        //float * alfa=(float *) calloc (speakers_number, sizeof(float));
        for (i = 0; i < speakers_number; ++i) {
            alfa[i] = atan2(difY[i],difX[i])*180/pi;
        }

        int sizeOfPos = 0;
        for (i = 0; i < speakers_number; ++i) {
            printf("%f=> %f <= %f\n",speakers_tecta[i]+90,alfa[i],speakers_tecta[i]-90  );
            if ( ( ( alfa[i] <= speakers_tecta[i]+90 ) && ( alfa[i] >= speakers_tecta[i]-90 ) ) ){
                printf("DENTRO: pos = %d, parray = 1\n",i);
                sizeOfPos++;
            }else{
                printf("FUERA: pos = -1, parray = 0\n");
            }
        }

        float r;

        for (i = 0; i < speakers_number; ++i) {
            r = sqrt( (difX[i] * difX[i]) + (difY[i] * difY[i]) );
            printf("AN[%d]=%f\n",i,1/(sqrt(r)));
            printf("TN[%d]=%f\n",i,-land*(FS*(r/c)));
        }

        printf("X: ");
        scanf("%f", &x);
        printf("Y: ");
        scanf("%f", &y);

    }
}