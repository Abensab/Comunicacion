//
// Created by myhay on 27/05/16.
//

#include "convolution.h"

//NEON
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

void convolutionOverlapAdd(float* partX, size_t lx, float localBuffer[], size_t lb/*potencia de 2*/, float* h, size_t lh, float y[]) {

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


/*                  INT                     */

void convolutionOverlapAddINT(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, int y[]) {

    size_t i,j = 0;

    if(lx < lh){
        printf("H size (%d) must be smaller than X size (%d)\n",(int)lh,(int)lx);
        exit(0);
    }

    convolution(partX, 0, lx, h, lh, localBuffer);

    for (j = 0; j < lx; j++) {
        y[j]= (int)localBuffer[j];
        localBuffer[j] = 0;
    }

    for (i = 0; i < lx; i++) {
        localBuffer[i] = localBuffer[j];
        localBuffer[j] = 0;
        j++;
    }
}

void convolutionOverlapAdd8INT(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, int y[]) {

    size_t i,j = 0;

    if(lx < lh){
        printf("H size (%d) must be smaller than X size (%d)\n",(int)lh,(int)lx);
        exit(0);
    }

    convolution8(partX, 0, lx, h, lh, localBuffer);

    for (j = 0; j < lx; j+=8) {
        y[j+0]= (int) localBuffer[j+0];
        y[j+1]= (int) localBuffer[j+1];
        y[j+2]= (int) localBuffer[j+2];
        y[j+3]= (int) localBuffer[j+3];
        y[j+4]= (int) localBuffer[j+4];
        y[j+5]= (int) localBuffer[j+5];
        y[j+6]= (int) localBuffer[j+6];
        y[j+7]= (int) localBuffer[j+7];

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

