#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void convolve(double* signal, size_t signalLen, double* filter, size_t filterLen, double* result){
    size_t n;

    size_t kmin, kmax, k;

    for (n = 0; n < signalLen + filterLen - 1; n++)
    {
        result[n] = 0;

        //Optimización del recorrer.
        kmin = (n >= filterLen - 1) ? n - (filterLen - 1) : 0;
        kmax = (n < signalLen - 1) ? n : signalLen - 1;

        //printf("%d kmin, %d kmax\n",(int)kmin,(int)kmax);

        for (k = kmin; k <= kmax; k++)
        {
            result[n] += signal[k] * filter[n - k];
        }
    }
}
void input_side_conv(double *x, size_t lx, double *h, size_t lh, double *y)
{
    size_t i,j;

    for(i=0;i<lx; i++)

        for(j=0;j<lh;j++)

            y[i+j]=y[i+j]+x[i]*h[j];
}



#define COUNT(X) (sizeof(X) / sizeof((X)[0]))

int main(void)
{
    clock_t tic,toc;

    double signal[] = { 3,4,5,6,7,8,9,10 };
    double filter[] = { 2,1 };
    double result[COUNT(signal) + COUNT(filter) - 1];

    tic = clock();
    convolve(signal, COUNT(signal), filter, COUNT(filter), result);
    toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    input_side_conv(signal, COUNT(signal), filter, COUNT(filter), result);
    toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);


    size_t signalLength = 512;
    size_t filterLength = 61;
    size_t resultLength = signalLength + filterLength - 1;

    double *A = malloc( signalLength*sizeof(double) );
    double *B = malloc( filterLength*sizeof(double) );
    double *C = malloc( resultLength*sizeof(double) );

    /* datos aleatorios */
    int j;
    for( j=0; j<signalLength; j++ ) {
       A[j] = ((double) rand()/ RAND_MAX);
    }

    for( j=0; j<filterLength; j++ ) {
        B[j] = ((double) rand()/ RAND_MAX);
    }

    tic = clock();
    convolve(A, signalLength, B, filterLength, C);
    toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);

    tic = clock();
    input_side_conv(A, signalLength, B, filterLength, C);
    toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);



    return 0;
}