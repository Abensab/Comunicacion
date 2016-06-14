//
// Created by myhay on 14/06/16.
//

#include "comparation.h"

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

void convolutionOverlapAdd8(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, float y[]) {

    size_t i,j = 0;

    if(lx < lh){
        printf("H size (%d) must be smaller than X size (%d)\n",(int)lh,(int)lx);
        exit(0);
    }

    convolution8(partX, 0, lx, h, lh, localBuffer);

    for (j = 0; j < lx; j+=8) {
        y[j+0]= localBuffer[j+0];
        y[j+1]= localBuffer[j+1];
        y[j+2]= localBuffer[j+2];
        y[j+3]= localBuffer[j+3];
        y[j+4]= localBuffer[j+4];
        y[j+5]= localBuffer[j+5];
        y[j+6]= localBuffer[j+6];
        y[j+7]= localBuffer[j+7];

        localBuffer[j+0] = 0;
        localBuffer[j+1] = 0;
        localBuffer[j+2] = 0;
        localBuffer[j+3] = 0;
        localBuffer[j+4] = 0;
        localBuffer[j+5] = 0;
        localBuffer[j+6] = 0;
        localBuffer[j+7] = 0;
    }

    for (i = 0; i < lx; i+=8) {
        localBuffer[i+0] = localBuffer[j+0];
        localBuffer[i+1] = localBuffer[j+1];
        localBuffer[i+2] = localBuffer[j+2];
        localBuffer[i+3] = localBuffer[j+3];
        localBuffer[i+4] = localBuffer[j+4];
        localBuffer[i+5] = localBuffer[j+5];
        localBuffer[i+6] = localBuffer[j+6];
        localBuffer[i+7] = localBuffer[j+7];
        localBuffer[i+7] = localBuffer[j+7];


        localBuffer[j+0] = 0;
        localBuffer[j+1] = 0;
        localBuffer[j+2] = 0;
        localBuffer[j+3] = 0;
        localBuffer[j+4] = 0;
        localBuffer[j+5] = 0;
        localBuffer[j+6] = 0;
        localBuffer[j+7] = 0;

        j+=8;
    }
}
/*
void convolutionNEON(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[])
{
    size_t i,j;
    size_t lx = signalEnd-signalStart;

    float32_t xi;
    float32x4_t t;
    float32x4_t hi_1,hi_2;
    float32x4_t acum;

    for(i=signalStart;i<signalEnd; i++){
        xi = x[i];
        for(j=0;j<lh;j+=8){

            hi_1 = vld1q_f32(h+j);
            hi_2 = vld1q_f32(h+j+4);

            t = vmulq_n_f32(hi_1,xi);

            acum = vld1q_f32(&y[i+j]);
            acum = vaddq_f32(acum,t);
            vst1q_f32(&y[i+j],acum);

            t = vmulq_n_f32(hi_2,xi);

            acum = vld1q_f32(&y[i+j+4]);
            acum = vaddq_f32(acum,t);
            vst1q_f32(&y[i+j+4],acum);
        }
    }
}

void convolutionOverlapAddNEON(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, float y[]) {

    size_t i,j = 0;

    if(lx < lh){
        printf("H size (%d) must be smaller than X size (%d)\n",(int)lh,(int)lx);
        exit(0);
    }

    convolutionNEON(partX, 0, lx, h, lh, localBuffer);

    for (j = 0; j < lx; j+=8) {
        y[j+0]= localBuffer[j+0];
        y[j+1]= localBuffer[j+1];
        y[j+2]= localBuffer[j+2];
        y[j+3]= localBuffer[j+3];
        y[j+4]= localBuffer[j+4];
        y[j+5]= localBuffer[j+5];
        y[j+6]= localBuffer[j+6];
        y[j+7]= localBuffer[j+7];

        localBuffer[j+0] = 0;
        localBuffer[j+1] = 0;
        localBuffer[j+2] = 0;
        localBuffer[j+3] = 0;
        localBuffer[j+4] = 0;
        localBuffer[j+5] = 0;
        localBuffer[j+6] = 0;
        localBuffer[j+7] = 0;
    }

    for (i = 0; i < lx; i+=8) {
        localBuffer[i+0] = localBuffer[j+0];
        localBuffer[i+1] = localBuffer[j+1];
        localBuffer[i+2] = localBuffer[j+2];
        localBuffer[i+3] = localBuffer[j+3];
        localBuffer[i+4] = localBuffer[j+4];
        localBuffer[i+5] = localBuffer[j+5];
        localBuffer[i+6] = localBuffer[j+6];
        localBuffer[i+7] = localBuffer[j+7];
        localBuffer[i+7] = localBuffer[j+7];


        localBuffer[j+0] = 0;
        localBuffer[j+1] = 0;
        localBuffer[j+2] = 0;
        localBuffer[j+3] = 0;
        localBuffer[j+4] = 0;
        localBuffer[j+5] = 0;
        localBuffer[j+6] = 0;
        localBuffer[j+7] = 0;

        j+=8;
    }
}

*/

void convolutionFFT(float* x, size_t signalEnd, float *h, size_t lh, float y[])
{
    size_t i,j;

    int iter = signalEnd/lh;
    float _Complex * H_FFT  =(float _Complex *)malloc(lh*sizeof(float _Complex));
    fftGS(h, lh,H_FFT );

    float _Complex * A_FFT =(float _Complex *)malloc(signalEnd*sizeof(float _Complex));

    for (i = 0; i < iter; ++i) {
        fftGS(&x[i*iter], lh, &A_FFT[i*iter]);
        for (j = 0; j < lh; ++j)
        {
            A_FFT[i*iter+j] = A_FFT[i*iter+j]*H_FFT[j];
        }
    }

    ifftGS(A_FFT,signalEnd,y);
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

    int nTimes = 1;

    printf("Size A: %d, Size H: %d , Size block: %d \n",lx,lh,bs);

    printf("Convolution;ConvolutionLoopUnroll8;ConvolutionOverlapADD;ConvolutionOverlapADD8;ConvolutionARM;ConvolutionARMOverlapADD;ConvolutionFFT;ConvolutionFFTW;\n");

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

    errorValue = errorCheck(B,D,lx,lh);
    if(errorValue != 0)
        printf("error: %.20f \n",errorValue);

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

    errorValue = errorCheck(B,D,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    memset(D , 0, tamResultado*2*sizeof(float));
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
            convolutionOverlapAdd8(processBuffer, bs, localBuffer, l_localBuffer, H1, lh, &D[k * bs]);
        }
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
//    printf("Elapsed: %.20f miliseconds => convolution Overlap ADD 8\n", elapsedTime/nTimes );
    printf("%.20f;", elapsedTime/nTimes );
    errorValue = errorCheck(B,D,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    memset(D , 0, tamResultado*2*sizeof(float));
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
            convolutionFFT(processBuffer,bs,H1,lh,&D[k * bs]);

        }
    }

    gettimeofday (&t1, NULL);
    elapsedTime = (t1.tv_sec - t0.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t1.tv_usec - t0.tv_usec) / 1000.0;   // us to ms
//    printf("Elapsed: %.20f miliseconds => convolution FFT\n", elapsedTime/nTimes );
    printf("%.20f;", elapsedTime/nTimes );
    //errorValue = errorCheck(A,D,lx,lh);
    if(errorValue != 0)
        printf("Error: %.20f \n",errorValue);
}