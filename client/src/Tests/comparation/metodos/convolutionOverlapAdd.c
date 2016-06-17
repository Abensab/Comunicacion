//
// Created by myhay on 14/06/16.
//

#include <stddef.h>
#include <stdio.h>

//exit(0), malloc, calloc
#include <stdlib.h>

//time
#include <time.h>

//ceil
#include <math.h>
#include <string.h>

//fft
#include "../../../lib/fft/fftGS.h"

//neon
#include <arm_neon.h>


int nextpw2(int valor){
    return  pow(2, ceil(log(valor)/log(2)));
}

void convolution(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[])
{
    size_t i,j;
    //size_t lx = signalEnd-signalStart;

    for(i=signalStart;i<signalEnd; i++){
        for(j=0;j<lh;j++){
            y[i+j]=y[i+j]+x[i]*h[j];
        }
    }
}


void convolutionOverlapAdd(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, float y[]) {

    size_t i,j = 0;

    if(lx < lh){
        printf("H size (%d) must be smaller than X size (%d)\n",(int)lh,(int)lx);
        exit(0);
    }

    convolution(partX, 0, lx, h, lh, localBuffer);

    for (j = 0; j < lx; j++) {
        y[j]= localBuffer[j];
        localBuffer[j] = 0;
    }

    for (i = 0; i < lx; i++) {
        localBuffer[i] = localBuffer[j];
        localBuffer[j] = 0;
        j++;
    }
}


void checkPrinter(float* A,float* B, size_t lx, size_t lh){
    int j;
    for( j=0; j<(lx+lh-1);j++ ) {
        if(A[j]-B[j] != 0){
            printf("%d\t%.20f\t%.20f\n",(int)j,A[j],B[j]);
        }
    }
}

float errorCheck(float* A,float* B, size_t lx, size_t lh){
    int j;
    float sum = 0;

    for( j=0; j<(lx+lh-1);j++ ) {
        sum+= A[j]-B[j];
    }

    if(sum != 0){checkPrinter(A,B,lx,lh);}

    return sum;

}

int main(int argc, char **argv) {

    int lx = 8;
    int lh = 8;
    /* Extract Arguments */
    if (argc >= 2) {
        if ((lx = atoi(argv[1])) < 8) lx = 8;
    }
    if (argc >= 3) {
        if ((lh = atoi(argv[2])) < 4) lh = 4;
    }
    int bs = lh;
    if (argc >= 4) {
        if ((bs = atoi(argv[3])) < 8) bs = 8;
    }

    struct timeval t0, t1;

    /* completely random! */
    unsigned int seed = 50085;
    srand( seed );

    /* Generate H1 from 1 to lh*/
    int i;
    float* H1 __attribute__((aligned(16))) = malloc( lh*sizeof(float) );
    for (i = 0; i < lh; ++i) {
        H1[i] = (float)rand()/RAND_MAX;
    }


    /* Generate A from 1 to lx*/
    float* A __attribute__((aligned(16))) = malloc( lx*sizeof(float) );
    for (i = 0; i < lx; ++i) {
        A[i] = (float)rand()/RAND_MAX;
    }

    int tamResultado = lx+lh-1;

    //Result vector normal convolution
    float *B = malloc( tamResultado*sizeof(float) );


/*
    Cectors with the result size resultSize+bs
*/
    float *C = (float *) calloc(tamResultado*2, sizeof(float));
    float *D = (float *) calloc(tamResultado*2, sizeof(float));

    /*=============================================================================================================*/
    double elapsedTime;
    float errorValue;

    int nTimes = 1000;

    //printf("Testing Convolution:\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        convolution(A,0,lx,H1,lh,B);
    }
    gettimeofday (&t1, NULL);

    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
//    printf("Elapsed: %.20f miliseconds => convolution\n", elapsedTime/nTimes );
    //printf("%.20f;", elapsedTime/nTimes );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //printf("\nTesting Convolution Overlap Add:\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    memset(D , 0, tamResultado*2*sizeof(float));

    int l_localBuffer = nextpw2(bs+lh-1);
    float * localBuffer;
    float * processBuffer;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Buffer para almacenar el residuo de la convolución
    localBuffer = (float *) calloc(l_localBuffer, sizeof(float));
    // Buffer donde se cópia los valores de A indiferente de si es o no potencia de 2
    // En caso de no tener más valores se queda en 0
    processBuffer = (float *) malloc(bs * sizeof(float));


    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {

        float iterations = (float) lx / bs;
        int posA = 0;

        //printf("Iterations=%f\n", iterations);
        float *processBuffer = (float *) malloc(bs * sizeof(float));
        int k;
        for (k = 0; k <= (int) iterations + 1; ++k) { // +1 en caso de que se tenga que el resto no de exactamente 0
            //Esto en el código principal de reproducir está previsto

            //set memory to 0
            memset(processBuffer, 0, bs * sizeof(float));
            int z = 0;
            // copiar A a vector de procesamiento
            for (posA = k * bs; posA < lx; ++posA) {
                processBuffer[z] = A[posA];
                ++z;
            }

            convolutionOverlapAdd(processBuffer, bs, localBuffer, l_localBuffer, H1, lh, &D[k * bs]);
        }
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
//    printf("Elapsed: %.20f miliseconds => convolution Overlap ADD \n", elapsedTime/nTimes );
    printf("%.20f;", elapsedTime/nTimes );

    //errorValue = errorCheck(B,D,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);

}