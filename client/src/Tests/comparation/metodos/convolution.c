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

void convolution8(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[])
{
    size_t i,j;
    //size_t lx = signalEnd-signalStart;

    float temp[8];
    for(i=signalStart;i<signalEnd; i++){
        for(j=0;j<lh;j+=8){

            temp[0] = x[i]*h[j+0];
            temp[1] = x[i]*h[j+1];
            temp[2] = x[i]*h[j+2];
            temp[3] = x[i]*h[j+3];

            temp[4] = x[i]*h[j+4];
            temp[5] = x[i]*h[j+5];
            temp[6] = x[i]*h[j+6];
            temp[7] = x[i]*h[j+7];

            y[i+j+0] += temp[0];
            y[i+j+1] += temp[1];
            y[i+j+2] += temp[2];
            y[i+j+3] += temp[3];

            y[i+j+4] += temp[4];
            y[i+j+5] += temp[5];
            y[i+j+6] += temp[6];
            y[i+j+7] += temp[7];
        }
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

    //printf("SignalSize;FilterSize;blockSize;Convolution;ConvolutionLoopUnroll8;ConvolutionOverlapADD;ConvolutionOverlapADD8;ConvolutionARM;ConvolutionARMOverlapADD;ConvolutionFFT;ConvolutionFFTW;\n");
    printf("%d;%d;%d;",lx,lh,bs);
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
    printf("%.20f;", elapsedTime/nTimes );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        convolution8(A,0,lx,H1,lh,D);
    }
    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    //printf("Elapsed: %.20f miliseconds => convolution loopUnroling i,j 8\n", elapsedTime/nTimes );
    printf("%.20f;", elapsedTime/nTimes );

    //errorValue = errorCheck(B,D,lx,lh);
    if(errorValue != 0)
        printf("error: %.20f \n",errorValue);

}