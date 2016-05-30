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

    //if(sum != 0){checkPrinter(A,B,lx,lh);}

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
        if ((bs = atoi(argv[3])) < 16) bs = 16;
    }

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
    float *C = (float *) calloc(tamResultado*2, sizeof(float));
    //float *D = (float *) calloc(tamResultado + bs, sizeof(float));
    float *E = (float *) calloc(tamResultado*2, sizeof(float));
    float *F = (float *) calloc(tamResultado*2, sizeof(float));
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
    printf("\nTesting Convolution Overlap Add:\n");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int l_localBuffer = nextpw2(bs+lh-1);
    float * localBuffer;
    float *processBuffer;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    localBuffer = (float *) calloc(l_localBuffer, sizeof(float));
    processBuffer = (float *) malloc(bs * sizeof(float));


    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {

        float iterations = (float) lx / bs;
        int posA = 0;

        printf("Iterations=%f\n", iterations);
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

            convolutionOverlapAdd(processBuffer, bs, localBuffer, l_localBuffer, H1, lh, &E[k * bs]);
        }
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution Overlap ADD \n", elapsedTime/nTimes );

    errorValue = errorCheck(B,E,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    memset(localBuffer , 0, l_localBuffer*sizeof(float));

    gettimeofday (&t0, NULL);
    for (i = 0; i < nTimes; ++i) {

        float iterations = (float) lx / bs;
        int pos = 0;

        int k;
        for (k = 0; k <= (int) iterations + 1; ++k) { // +1 en caso de que se tenga que el resto no de exactamente 0
            //Esto en el código principal de reproducir está previsto

            //set memory to 0
            memset(processBuffer, 0, bs * sizeof(float));
            int z = 0;
            // copiar A a vector de procesamiento
            for (pos = k * bs; pos < lx; ++pos) {
                processBuffer[z] = A[pos];
                ++z;
            }
            convolutionOverlapAdd8(processBuffer, bs, localBuffer, l_localBuffer, H1, lh, &F[k * bs]);
        }
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
    printf("Elapsed: %.20f miliseconds => convolution Overlap ADD 8\n", elapsedTime/nTimes );

    errorValue = errorCheck(B,F,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    return 0;
}
