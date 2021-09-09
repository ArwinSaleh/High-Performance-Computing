#include <stddef.h>
#include <stdio.h>
#include "helper_functions.h"

void compute_row_sums
(
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

void compute_col_sums
(
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
    int size = 1000;

    double * asentries = (double*) malloc(sizeof(double) * size*size);
    double ** as = (double**) malloc(sizeof(double*) * size);
    for ( size_t ix = 0, jx = 0; ix < size; ++ix, jx+=size )
    as[ix] = asentries + jx;

    for ( size_t ix = 0; ix < size; ++ix )
    for ( size_t jx = 0; jx < size; ++jx )
        as[ix][jx] = drand();
        
    double * row_sums;
    double * col_sums;

    compute_row_sums(&row_sums, as, size, size);
    compute_col_sums(&col_sums, as, size, size);
    
    free(as);
    free(asentries);

    printf("row sum0: %f \ncol sum0: %f", row_sums[0], col_sums[0]);
}