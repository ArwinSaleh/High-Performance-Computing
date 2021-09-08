#include <stdio.h>
#include <stdlib.h>

double drand ( double low, double high )
{
    return ( (double)random() * ( high - low ) ) / (double)RAND_MAX + low;
}