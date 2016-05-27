//
// Created by myhay on 27/05/16.
//

#include "testConvolution.h"

void checkPrinter(float* A,float* B, size_t lx, size_t lh){
    int j;
    for( j=0; j<(lx+lh-1);j++ ) {
        if(A[j]-B[j] != 0){
            printf("%d\t%f\t%f\n",(int)j,A[j],B[j]);
        }
    }

}


float errorCheck(float* A,float* B, size_t lx, size_t lh){
    int j;
    float sum = 0;

    for( j=0; j<(lx+lh-1);j++ ) {
        sum+= A[j]-B[j];
    }

//    if(sum != 0)
//        checkPrinter(A,B,lx,lh);

    return sum;

}

int main(int argc, char **argv) {

    int lx = 16;
    int lh = 16;
    int bs = 16;

    /* Extract Arguments */
    if (argc >= 2) {
        if ((lx = atoi(argv[1])) < 16) lx = 16;
    }
    if (argc >= 3) {
        if ((lh = atoi(argv[2])) < 16) lh = 16;
    }
    //if (argc >= 4) {
    //    if ((bs = atoi(argv[3])) < 16) bs = 16;
    //}

    bs = lx;


    struct timeval t0, t1;

    /* Generate H1 from 1 to lh*/
    int i;
    float* H1 __attribute__((aligned(16))) = malloc( lh*sizeof(float) );
    for (i = 0; i < lh; ++i) {
        H1[i] = (float)(i+1);
    }


    /* Generate A from 1 to lx*/
    float* A __attribute__((aligned(16))) = malloc( lx*sizeof(float) );
    for (i = 0; i < lx; ++i) {
        A[i] = (float)(i+1);
    }

    int tamResultado = bs+lh;

    //Result vector normal convolution
    float *B = malloc( tamResultado*sizeof(float) );


/*
    Test vectors with the result size resultSize+bs
*/
    float *C = (float *) calloc(tamResultado + bs, sizeof(float));
    float *D = (float *) calloc(tamResultado + bs, sizeof(float));
    float *E = (float *) calloc(tamResultado + bs, sizeof(float));
    float *F = (float *) calloc(tamResultado + bs, sizeof(float));
    float *G = (float *) calloc(tamResultado + bs, sizeof(float));


/*=============================================================================================================*/
    double elapsedTime;
    float errorValue;
    int nTimes = 1;

    printf("Size A %d , Size H %d \n",lx,lh);


    printf("Testing Convolution:\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        convolution(A,0,lx,H1,lh,B);
    }
    gettimeofday (&t1, NULL);

    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution\n", elapsedTime/nTimes );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        convolution8(A,0,lx,H1,lh,C);
    }
    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution loopUnroling i,j 8\n", elapsedTime/nTimes );

    errorValue = errorCheck(B,C,lx,lh);
    printf("error: %.20f \n",errorValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        //convolutionNEON8(A,0,lx,H1,lh,D);
    }
    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution NEON 8\n", elapsedTime/nTimes );

    errorValue = errorCheck(B,D,lx,lh);
    printf("Error: %.20f \n",errorValue);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    printf("Testing Convolution Overlap Add:\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    printf("\n");

/*
    errorValue = errorCheck(B,E,lx,lh);
    if(errorValue != 0){
        printf("E error: %.20f \n",errorValue);
    }

    errorValue = errorCheck(B,F,lx,lh);
    if(errorValue != 0){
        printf("F error: %.20f \n",errorValue);
    }

    errorValue = errorCheck(B,G,lx,lh);
    if(errorValue != 0){
        printf("G error: %.20f \n",errorValue);
    }
*/
    return 0;
}
