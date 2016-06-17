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
#include <complex.h>
#include "../../../lib/fft/fftGS.h"

//neon
#include <arm_neon.h>

//fftw

//#include <fftw3.h>
#include <fftw3.h>

//#include "../../../../../bin/fftw/fftw-3.3.4/api/fftw3.h"

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

void convolutionFFTW(float* x, size_t lx, float *h, size_t lh, float y[])
{
    int i = 0;

    // lx+1 dado que lx al ser potencia de 2 no realiza nada.
    int size_lx_lh = nextpw2(lx+1);

    fftw_complex *X,*H;
    /* define fftw_plan which contains all the data needed
       to compute the fft */
    fftw_plan fftX;
    fftw_plan fftH;
    fftw_plan ifftR;

    /* allocate memory */
    X = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*size_lx_lh);
    H = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*size_lx_lh);

    fftX=fftw_plan_dft_1d(size_lx_lh,X,X,FFTW_FORWARD,FFTW_ESTIMATE);
    fftH=fftw_plan_dft_1d(size_lx_lh,H,H,FFTW_FORWARD,FFTW_ESTIMATE);

    ifftR=fftw_plan_dft_1d(size_lx_lh,X,X,FFTW_BACKWARD,FFTW_ESTIMATE);


    for(i=0;i<lx;i=i+8) {
        X[i+0] = x[i+0]+0.0f*_Complex_I;
        X[i+1] = x[i+1]+0.0f*_Complex_I;
        X[i+2] = x[i+2]+0.0f*_Complex_I;
        X[i+3] = x[i+3]+0.0f*_Complex_I;
        X[i+4] = x[i+4]+0.0f*_Complex_I;
        X[i+5] = x[i+5]+0.0f*_Complex_I;
        X[i+6] = x[i+6]+0.0f*_Complex_I;
        X[i+7] = x[i+7]+0.0f*_Complex_I;
    }

    for(i=0;i<lh;i=i+8) {
        H[i+0] = h[i+0]+0.0f*_Complex_I;
        H[i+1] = h[i+1]+0.0f*_Complex_I;
        H[i+2] = h[i+2]+0.0f*_Complex_I;
        H[i+3] = h[i+3]+0.0f*_Complex_I;
        H[i+4] = h[i+4]+0.0f*_Complex_I;
        H[i+5] = h[i+5]+0.0f*_Complex_I;
        H[i+6] = h[i+6]+0.0f*_Complex_I;
        H[i+7] = h[i+7]+0.0f*_Complex_I;

    }

    fftw_execute(fftX);
    fftw_execute(fftH);

    for (i = 0; i < size_lx_lh; i=i+8) {
        X[i+0] = X[i+0]*H[i+0];
        X[i+1] = X[i+1]*H[i+1];
        X[i+2] = X[i+2]*H[i+2];
        X[i+3] = X[i+3]*H[i+3];
        X[i+4] = X[i+4]*H[i+4];
        X[i+5] = X[i+5]*H[i+5];
        X[i+6] = X[i+6]*H[i+6];
        X[i+7] = X[i+7]*H[i+7];
    }


    fftw_execute(ifftR);

    for (i = 0; i < lx+lh-1; ++i) {
        y[i] = creal(X[i])/(double)size_lx_lh;
    }

    fftw_destroy_plan(fftX);
    fftw_destroy_plan(fftH);
    fftw_destroy_plan(ifftR);

    fftw_free(X);
    fftw_free(H);
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    memset(D , 0, tamResultado*2*sizeof(float));
    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        convolutionFFTW(A,lx, H1, lh, D);
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
//    printf("Elapsed: %.20f miliseconds => convolution FFT\n", elapsedTime/nTimes );
    printf("%.20f;", elapsedTime/nTimes );
    //errorValue = errorCheck(B,D,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);

}