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
  const size_t benchIter = 5000;
  const size_t size = 1000;

  double * asentries = (double*) malloc(sizeof(double) * size*size);
  double ** as = (double**) malloc(sizeof(double*) * size);
  for ( size_t ix = 0, jx = 0; ix < size; ++ix, jx+=size )
  {
    as[ix] = asentries + jx;
  }

  double entry = 0;
  for ( size_t ix = 0; ix < size; ++ix )
  {
    for ( size_t jx = 0; jx < size; ++jx )
    {
      as[ix][jx] = entry;
      entry += 0.1;
    } 
  }

  for (size_t runs = 0; runs < benchIter; runs++)
  {
    double * row_sums = (double*) malloc(sizeof(double) * size);
    double * col_sums = (double*) malloc(sizeof(double) * size);

    for ( size_t ix = 0; ix < size; ++ix )
    {
      double sum = 0.;    
      for ( size_t jx = 0; jx < size; ++jx )
      {
        sum += as[ix][jx];
      }
      row_sums[ix] = sum;
    }

    for ( size_t jx = 0; jx < size; ++jx ) 
    {
      double sum = 0.;
      for ( size_t ix = 0; ix < size; ++ix )
      {
        sum += as[ix][jx];
      }
      col_sums[jx] = sum;
    }

    
    free(as);
    free(asentries);

    if (runs == benchIter-1)
    {
      printf("row sum0: %f \ncol sum0: %f\n", row_sums[0], col_sums[0]);;
    }
  }
}