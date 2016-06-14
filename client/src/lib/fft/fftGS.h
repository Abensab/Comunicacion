#ifndef FFTGS_H
#define FFTGS_H

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>

enum SIGN{
	FORWARD = 1,
	REVERSE = -1
};

void fftGS_General(float _Complex * x, int ncol, enum SIGN sig);
void fftGS(float * vector, int ncol, float _Complex * x );
void ifftGS(float _Complex * x, int ncol, float * X);

#endif

