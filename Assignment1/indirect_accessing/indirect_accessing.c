#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

double rand_double ( double low, double high )
{
    return ( (double)random() * ( high - low ) ) / (double)RAND_MAX + low;
}

int main(int argc, char *argv[])
{
    const char *currentArgument = argv[1];
    const double a = 3.;
    const size_t size = 1000000;
    const size_t benchIter = 1000;

    double * y = (double*) malloc(sizeof(double) * size);
    double * p = (double*) malloc(sizeof(double) * size);
    double * x = (double*) malloc(sizeof(double) * size);
    for (size_t runs = 0; runs < benchIter; runs++)
    {
        for (size_t i = 0; i < size; i++)
        {
            x[i] = rand_double(0, 10);
        }

        if (currentArgument[0] == '1')
        {
            for ( size_t ix = 0; ix < size; ++ix  )
            {
                p[ix] = ix;
            }
        }

        else if (currentArgument[0] == '2')
        {
            const size_t size_jump = 1000;
            for ( size_t jx = 0, kx = 0; jx < size_jump; ++jx)
            {
                for ( size_t ix = jx; ix < size; ix += size_jump, ++kx)
                {
                    p[ix] = kx;
                }
            }
        }

        for ( size_t kx = 0; kx < size; ++kx ) {
            size_t jx = p[kx];
            y[jx] += a * x[jx];
        }
        if (runs == benchIter-1)
        {
            printf("\n%f\n", y[0]);
        }
    }
        free(y);
        free(p);
        free(x);
}
