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

void convolutionFFT(float* x, size_t lx, float *h, size_t lh, float y[])
{
    size_t i,j;

    if(lx != lh){
    	printf("ERROR: Size of signal must be equal to size of filter.");
    	exit(0);
    }

    float _Complex * H_FFT =(float _Complex *)malloc(lh*sizeof(float _Complex));
    fftGS(h, lh,H_FFT);

    float _Complex * A_FFT =(float _Complex *)malloc(lx*sizeof(float _Complex));
	fftGS(x, lh, A_FFT);

    for (i = 0; i < lx; ++i) {
        A_FFT[i] = A_FFT[i]*H_FFT[i];
    }

    ifftGS(A_FFT,lx,y);

    free(H_FFT);
    free(A_FFT);
}

void convolutionOverlapSave(float* x, size_t lx, float localBuffer[], size_t lb, float* h, float y[]) {

    size_t i,j = 0;

    if(lb < lx+lx){
        printf("bs size (%d) must be next pow of 2. Size (%d)\n",(int)lb,(int)lx);
        exit(0);
    }

	for (j = lx; j < lb; j++) {
        localBuffer[j] = x[j-lx];
    }    

    convolutionFFT(localBuffer,lb, h, lb, localBuffer);

    for (j = 0; j < lx; j++) {
    	localBuffer[j]=x[j];
        y[j]= localBuffer[j+lx];
        localBuffer[j+lx] = 0;
    }
}


void convolutionFFTW(float* x, size_t lx, float *h, size_t lh, float y[])
{
    size_t i,j;

    if(lx != lh){
    	printf("ERROR: Size of signal must be equal to size of filter.");
    	exit(0);
    }

    float _Complex * H_FFT  =(float _Complex *)malloc(lh*sizeof(float _Complex));
    fftGS(h, lh,H_FFT);

    float _Complex * A_FFT =(float _Complex *)malloc(lx*sizeof(float _Complex));
	fftGS(x, lh, A_FFT);

    for (i = 0; i < lx; i=i+8) {
        A_FFT[i+0] = A_FFT[i+0]*H_FFT[i+0];
        A_FFT[i+1] = A_FFT[i+1]*H_FFT[i+1];
        A_FFT[i+2] = A_FFT[i+2]*H_FFT[i+2];
        A_FFT[i+3] = A_FFT[i+3]*H_FFT[i+3];
        A_FFT[i+4] = A_FFT[i+4]*H_FFT[i+4];
        A_FFT[i+5] = A_FFT[i+5]*H_FFT[i+5];
        A_FFT[i+6] = A_FFT[i+6]*H_FFT[i+6];
        A_FFT[i+7] = A_FFT[i+7]*H_FFT[i+7];
    }

    ifftGS(A_FFT,lx,y);

    free(H_FFT);
    free(A_FFT);
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
    // lx+1 dado que lx al ser potencia de 2 no realiza nada.
    int size_lx_lh = nextpw2(lx+1);


    int l_tfilter = size_lx_lh;
    float * tfilter = (float *) calloc(l_tfilter, sizeof(float));
 	for (i = 0; i < lh; ++i) {
        tfilter[i] = H1[i];
    }
    
    int l_tsound = size_lx_lh;
    float * tsound = (float *) calloc(l_tsound, sizeof(float));

 	for (i = 0; i < lx; ++i) {
        tsound[i] = A[i];
    }
   
	int l_tresult = size_lx_lh;
    float * tresult = (float *) calloc(l_tresult, sizeof(float));

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        convolutionFFT(tsound,l_tsound,tfilter,l_tfilter,tresult);

    }

    for (i = 0; i < tamResultado; ++i) {
        D[i] = tresult[i];
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