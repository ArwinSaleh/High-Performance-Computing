#include <stdio.h>
#include <stdlib.h>

double drand ( double low, double high )
{
    return ( (double)random() * ( high - low ) ) / (double)RAND_MAX + low;
}

int irand ( int low, int high )
{
    return ( (int)random() * ( high - low ) ) / (int)RAND_MAX + low;
}