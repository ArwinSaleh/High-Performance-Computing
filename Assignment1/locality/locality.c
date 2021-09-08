#include <stddef.h>
#include <stdio.h>
#include "helper_functions.h"

void row_sums(
  double * sums,
  const double ** matrix,
  size_t nrs,
  size_t ncs
)
{
  for ( size_t ix = 0; ix < nrs; ++ix ) {
    double sum = 0.;
    for ( size_t jx = 0; jx < ncs; ++jx )
      sum += matrix[ix][jx];
    sums[ix] = sum;
  }
}

void col_sums(
  double * sums,
  const double ** matrix,
  size_t nrs,
  size_t ncs
  )
{
  for ( size_t jx = 0; jx < ncs; ++jx ) {
    double sum = 0.;
    for ( size_t ix = 0; ix < nrs; ++ix )
      sum += matrix[ix][jx];
    sums[jx] = sum;
  }
}

int main()
{
    int size = 10;

    double * asentries = (double*) malloc(sizeof(double) * size*size);
    double ** as = (double**) malloc(sizeof(double*) * size);
    for ( size_t ix = 0, jx = 0; ix < size; ++ix, jx+=size )
    as[ix] = asentries + jx;

    for ( size_t ix = 0; ix < size; ++ix )
    for ( size_t jx = 0; jx < size; ++jx )
        as[ix][jx] = rand_double();

    printf("%f", as[4][10]);

    

    free(as);
    free(asentries);
}