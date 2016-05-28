//
// Created by myhay on 27/05/16.
//

#ifndef COMUCINACION_CONVOLUTION_H
#define COMUCINACION_CONVOLUTION_H

#include <stddef.h>
#include <stdio.h>

//exit(0), malloc, calloc
#include <stdlib.h>

//time
#include <time.h>

//ceil
#include <math.h>
#include <string.h>

//NEON
#include <arm_neon.h>
//#include "arm_neon.h"

int nextpw2(int valor);
void convolution(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[]);
void convolutionOverlapAdd(float* partX, size_t lx, float localBuffer[], size_t lb/*potencia de 2*/, float* h, size_t lh, float y[]);
void convolution8(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[]);
void convolutionOverlapAdd8(float* partX, size_t lx, float localBuffer[], size_t lb/*potencia de 2*/, float* h, size_t lh, float y[]);
void convolutionNEON8(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[]);
void convolutionOverlapAddNEON8(float* partX, size_t lx, float localBuffer[], size_t lb/*potencia de 2*/, float* h, size_t lh, float y[]);


#endif //COMUCINACION_CONVOLUTION_H
