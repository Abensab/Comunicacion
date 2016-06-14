//
// Created by myhay on 27/05/16.
//

#include "convolution.h"

//FFTW
#include <fftw3.h>


int nextpw2(int valor){
    return  pow(2, ceil(log(valor)/log(2)));
}

void convolution(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[]){

}

void convolution8(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[]){

}

void convolutionOverlapAdd(float* partX, size_t lx, float localBuffer[], size_t lb/*potencia de 2*/, float* h, size_t lh, float y[]) {

}

void convolutionOverlapAdd8(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, float y[]) {

}


/*                  INT                     */

void convolutionOverlapAddINT(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, int y[]) {

}

void convolutionOverlapAdd8INT(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, int y[]) {

}

