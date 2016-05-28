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
    if(sum != 0){checkPrinter(A,B,lx,lh);}

    return sum;

}

int main(int argc, char **argv) {

    int lx = 8;
    int lh = 8;
    int bs = 8;

    /* Extract Arguments */
    if (argc >= 2) {
        if ((lx = atoi(argv[1])) < 8) lx = 8;
    }
    if (argc >= 3) {
        if ((lh = atoi(argv[2])) < 4) lh = 4;
    }
    //if (argc >= 4) {
    //    if ((bs = atoi(argv[3])) < 16) bs = 16;
    //}

    bs = lh;


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

    int tamResultado = lx+lh-1;

    //Result vector normal convolution
    float *B = malloc( tamResultado*sizeof(float) );


/*
    Test vectors with the result size resultSize+bs
*/
    float *C = (float *) calloc(tamResultado + bs, sizeof(float));
    //float *D = (float *) calloc(tamResultado + bs, sizeof(float));
    float *E = (float *) calloc(tamResultado + bs, sizeof(float));
    float *F = (float *) calloc(tamResultado + bs, sizeof(float));
    //float *G = (float *) calloc(tamResultado + bs, sizeof(float));


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
    if(errorValue != 0)
        printf("error: %.20f \n",errorValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        convolutionNEON8(A,0,lx,H1,lh,D);
    }
    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution NEON 8\n", elapsedTime/nTimes );

    errorValue = errorCheck(B,D,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);


    printf("\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    printf("Testing Convolution Overlap Add:\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int l_localBuffer = nextpw2(bs+lh-1);
    float * localBuffer = (float *) calloc(l_localBuffer, sizeof(float));

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        int j;
        for(j=0; j<(lx/bs); j++ ) {
            convolutionOverlapAdd(&A[j*bs], bs, localBuffer, l_localBuffer, H1, lh, &E[j*bs]);
        }

        // Para tener un vector de 0 y añadir los ultimos ejementos.
        float * lastPass = (float *) calloc(bs, sizeof(float));
        int last_index = j*bs;
        int sizeleft = lx - last_index;
        int z;

        if( sizeleft != 0 ){
            for (z = 0; z < sizeleft; ++z) {
                lastPass[z] = A[z+last_index];
            }
        }
        convolutionOverlapAdd(lastPass, bs, localBuffer, l_localBuffer, H1, lh, &E[j*bs]);
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution Overlap ADD \n", elapsedTime/nTimes );

    errorValue = errorCheck(B,E,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        int j;
        for(j=0; j<(lx/bs); j++ ) {
            convolutionOverlapAdd8(&A[j*bs], bs, localBuffer, l_localBuffer, H1, lh, &F[j*bs]);
        }

        // Para tener un vector de 0 y añadir los ultimos ejementos.
        float * lastPass = (float *) calloc(bs, sizeof(float));
        int last_index = j*bs;
        int sizeleft = lx - last_index;
        int z;

        if( sizeleft != 0 ){
            for (z = 0; z < sizeleft; ++z) {
                lastPass[z] = A[z+last_index];
            }
        }
        convolutionOverlapAdd8(lastPass, bs, localBuffer, l_localBuffer, H1, lh, &F[j*bs]);
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution Overlap ADD 8\n", elapsedTime/nTimes );

    errorValue = errorCheck(B,F,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {
        int j;
        for(j=0; j<(lx/bs); j++ ) {
            convolutionOverlapAdd8(&A[j*bs], bs, localBuffer, l_localBuffer, H1, lh, &G[j*bs]);
        }

        // Para tener un vector de 0 y añadir los ultimos ejementos.
        float * lastPass = (float *) calloc(bs, sizeof(float));
        int last_index = j*bs;
        int sizeleft = lx - last_index;
        int z;

        if( sizeleft != 0 ){
            for (z = 0; z < sizeleft; ++z) {
                lastPass[z] = A[z+last_index];
            }
        }
        convolutionOverlapAdd8(lastPass, bs, localBuffer, l_localBuffer, H1, lh, &G[j*bs]);
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution Overlap ADD NEON 8\n", elapsedTime/nTimes );

    errorValue = errorCheck(B,G,lx,lh);
    if(errorValue != 0)
        printf("G error: %.20f \n",errorValue);

    return 0;
}
