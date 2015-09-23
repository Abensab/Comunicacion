#include <stddef.h>
#include <stdio.h>

void convolve(const double Signal[/* SignalLen */], size_t SignalLen,
              const double Kernel[/* KernelLen */], size_t KernelLen,
              double Result[/* SignalLen + KernelLen - 1 */])
{
    size_t n;

    for (n = 0; n < SignalLen + KernelLen - 1; n++)
    {
        size_t kmin, kmax, k;

        Result[n] = 0;

        kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
        kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

        printf("%d kmin, %d kmax\n",(int)kmin,(int)kmax);


        for (k = kmin; k <= kmax; k++)
        {
            Result[n] += Signal[k] * Kernel[n - k];
        }
    }
}

void printSignal(const char* Name,
                 double Signal[/* SignalLen */], size_t SignalLen)
{
    size_t i;
    size_t k;

    for (i = 0; i < SignalLen; i++)
    {
        printf("%s[%zu] = %.0f   |", Name, i, Signal[i]);
        for (k = 0; k < Signal[i]; k++)
        {
            printf(" * ");
        }
        printf("\n");
    }
    printf("\n");
}

#define COUNT(X) (sizeof(X) / sizeof((X)[0]))

int main(void)
{

    double signal[] = { 1, 1, 1, 1, 1 };
    double kernel[] = { 1, 1, 1 };
    double result[COUNT(signal) + COUNT(kernel) - 1];

    convolve(signal, COUNT(signal), kernel, COUNT(kernel), result);

    printSignal("signal", signal, COUNT(signal));
    printSignal("kernel", kernel, COUNT(kernel));
    printSignal("result", result, COUNT(result));

    return 0;
}