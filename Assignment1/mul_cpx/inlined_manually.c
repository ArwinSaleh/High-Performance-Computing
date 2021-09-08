#include "helper_functions.h"

int main()
{
    /*
    const int size = 100000000;

    double * as_re = (double*) malloc(sizeof(double) * size);
    double * as_im = (double*) malloc(sizeof(double) * size);
    double * bs_re = (double*) malloc(sizeof(double) * size);
    double * bs_im = (double*) malloc(sizeof(double) * size);
    double * cs_re = (double*) malloc(sizeof(double) * size);
    double * cs_im = (double*) malloc(sizeof(double) * size);
    */

   const int benchIter = 1000000;

   for (int runs = 0; runs < benchIter; runs++)
   {
        const int size = 30000;

        double
        as_re[size],
        as_im[size],
        bs_re[size],
        bs_im[size],
        cs_re[size],
        cs_im[size];

        for (int i = 0; i < size; i++)
        {
            as_re[i] = 0;
            as_im[i] = 0;
            bs_re[i] = drand(0, 10);
            bs_im[i] = drand(0, 10);
            cs_re[i] = drand(0, 10);
            cs_im[i] = drand(0, 10);

            /*  Multiplication of two complex numbers b and c
            
                    (b_re + b_im * i) * (c_re + c_im * i) = b_re * (c_re + c_im * i) + b_im * i * (c_re + c_im * i) = 
                    
                        = b_re * c_re + b_re * c_im * i + b_im * i * c_re - b_im * c_im = 
                            
                            = (b_re * c_re - b_im * c_im) + i(b_re * c_im + b_im * c_re)
                */

            as_re[i] = (bs_re[i] * cs_re[i]) - (bs_im[i] * cs_im[i]);
            as_im[i] = (bs_re[i] * cs_im[i]) + (bs_im[i] * cs_re[i]);
        }
        if (runs == benchIter)
            {
                printf("\na_re: %f\na_im: %f\n\n", as_re[size/4], as_im[size/4]);
            }
   }
}