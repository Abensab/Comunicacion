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

int nextpw2(int valor);

// size of lb must be pow of 2

void convolution(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[]);
void convolutionOverlapAdd(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, float y[]);
void convolution8(float* x, size_t signalStart, size_t signalEnd, float *h, size_t lh, float y[]);
void convolutionOverlapAdd8(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, float y[]);


void convolutionOverlapAddINT(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, int y[]);
void convolutionOverlapAdd8INT(float* partX, size_t lx, float localBuffer[], size_t lb, float* h, size_t lh, float y[]);


#endif //COMUCINACION_CONVOLUTION_H
