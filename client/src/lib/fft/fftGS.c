#include "fftGS.h"

/* General FFT Using complex numbers.
   Parameters: Pointer to vector of complex numbers, 
   size of vector, FORWARD (FFT) or REVERSE (ifft).
   Return: double complex pointer to vector.
*/
void fftGS_General(float _Complex * x, int ncol, enum SIGN sig){

	int t = ceil(log(ncol)/log(2));
	int q;
	for (q = t; 1 <= q; --q)
	{
		int L = pow(2, q);
		int r = ncol/L;
		int L_ = L/2;
		int j;
		for (j = 0; j < L_; ++j)
		{
			float _Complex w = cos(2*M_PI*j/L) + ( sig * sin(2*M_PI*j/L)*I);
			int k;
			for (k = 0; k < r; ++k)
			{
				int index = (k*L)+L_+j;
				float _Complex tau = x[k*L+L_+j];

				x[k*L+j+L_]=w*(x[k*L+j]-tau);
				x[k*L+j]=x[k*L+j]+tau;
			}
		}
	}

	// Bit Reversal
	// Algoritmo 1.5.1
	int k;
	for (k = 0; k < ncol; ++k)
	{
		// Algoritmo 1.5.2
		int j = 0;
		int m = k;
		int q;
		for (q = 0; q < t; ++q)
		{
			int s = floor(m/2);
			j=2*j+(m-2*s);
			m=s;
		}
		// End Algoritmo 1.5.2

		float _Complex temportal_swap;
		if (j>k){
			temportal_swap = x[k];
			x[k]=x[j];
			x[j]=temportal_swap;
		}
	}

}

void fftGS(float * vector, int ncol, float _Complex * x){
	int i;
	for (i = 0; i < ncol; ++i)
	{
		x[i] = vector[i];
	}

	fftGS_General(x,ncol,FORWARD);

}

void ifftGS(float _Complex * x, int ncol, float * X){

	fftGS_General(x,ncol,REVERSE);
	int i;
	for (i = 0; i < ncol; ++i)
	{
		X[i]=creal(x[i])/ncol;
	}

}