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

	for (j = lx; j < lb; j=j+8) {
        localBuffer[j+0] = x[j+0-lx];
        localBuffer[j+1] = x[j+1-lx];
        localBuffer[j+2] = x[j+2-lx];
        localBuffer[j+3] = x[j+3-lx];
        localBuffer[j+4] = x[j+4-lx];
        localBuffer[j+5] = x[j+5-lx];
        localBuffer[j+6] = x[j+6-lx];
        localBuffer[j+7] = x[j+7-lx];
    }    

    convolutionFFT(localBuffer,lb, h, lb, localBuffer);

    for (j = 0; j < lx; j=j+8) {
    	localBuffer[j+0]=x[j+0];
    	localBuffer[j+1]=x[j+1];
    	localBuffer[j+2]=x[j+2];
    	localBuffer[j+3]=x[j+3];
    	localBuffer[j+4]=x[j+4];
    	localBuffer[j+5]=x[j+5];
    	localBuffer[j+6]=x[j+6];
    	localBuffer[j+7]=x[j+7];

 		y[j+0]= localBuffer[j+lx+0];
        y[j+1]= localBuffer[j+lx+1];
        y[j+2]= localBuffer[j+lx+2];
        y[j+3]= localBuffer[j+lx+3];
        y[j+4]= localBuffer[j+lx+4];
        y[j+5]= localBuffer[j+lx+5];
        y[j+6]= localBuffer[j+lx+6];
        y[j+7]= localBuffer[j+lx+7];

        localBuffer[j+0+lx] = 0;
        localBuffer[j+1+lx] = 0;
        localBuffer[j+2+lx] = 0;
        localBuffer[j+3+lx] = 0;
        localBuffer[j+4+lx] = 0;
        localBuffer[j+5+lx] = 0;
        localBuffer[j+6+lx] = 0;
        localBuffer[j+7+lx] = 0;
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

    int nTimes = 10;

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

    int size_lx_lh = nextpw2(bs+1);

    float * tfilter_2 = (float *) calloc(size_lx_lh, sizeof(float));
 	for (i = 0; i < lh; ++i) {
        tfilter_2[i] = H1[i];
    }
    
    float * processBuffer_2 = (float *) calloc(size_lx_lh, sizeof(float));
    float * tresult_2 = (float *) calloc(size_lx_lh, sizeof(float));



    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        
        float iterations = (float) lx / bs;
        int pos = 0;

        int k;
        for (k = 0; k <= (int) iterations + 1; ++k) { // +1 en caso de que se tenga que el resto no de exactamente 0
            //Esto en el código principal de reproducir está previsto

            //set memory to 0
            memset(processBuffer_2, 0, size_lx_lh * sizeof(float));
            int z = 0;
            // copiar A a vector de procesamiento
            for (pos = k * bs; pos < lx; ++pos) {
                processBuffer_2[z] = A[pos];
                ++z;
            }
            convolutionOverlapSave(processBuffer_2, bs, localBuffer, l_localBuffer, tfilter_2, tresult_2);
            
            for (i = 0; i < bs; ++i) {
     		    D[k * bs + i] = tresult_2[i];
    		}

        }
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
//    printf("Elapsed: %.20f miliseconds => convolution Overlap ADD 8\n", elapsedTime/nTimes );
    printf("%.20f;", elapsedTime/nTimes );
    //errorValue = errorCheck(B,D,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}