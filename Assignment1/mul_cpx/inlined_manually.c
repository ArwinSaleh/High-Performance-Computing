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
        as_re_val[size],
        as_im_val[size],
        bs_re_val[size],
        bs_im_val[size],
        cs_re_val[size],
        cs_im_val[size];

        for (int i = 0; i < size; i++)
        {
            as_re_val[i] = 0;
            as_im_val[i] = 0;
            bs_re_val[i] = drand(0, 10);
            bs_im_val[i] = drand(0, 10);
            cs_re_val[i] = drand(0, 10);
            cs_im_val[i] = drand(0, 10);

            double *a_re = as_re_val;
            double *a_im = as_im_val;
            double *b_re = bs_re_val;
            double *b_im = bs_im_val;
            double *c_re = cs_re_val;
            double *c_im = cs_im_val;

            /*  Multiplication of two complex numbers b and c
            
                    (b_re + b_im * i) * (c_re + c_im * i) = b_re * (c_re + c_im * i) + b_im * i * (c_re + c_im * i) = 
                    
                        = b_re * c_re + b_re * c_im * i + b_im * i * c_re - b_im * c_im = 
                            
                            = (b_re * c_re - b_im * c_im) + i(b_re * c_im + b_im * c_re)
                */

            *a_re = (*b_re * *c_re) - (*b_im * *c_im);
            *a_im = (*b_re * *c_im) + (*b_im * *c_re);
        }
        if (runs == benchIter-1)
            {
                printf("\na_re: %f\na_im: %f\n\n", as_re_val[size/4], as_im_val[size/4]);
            }
   }
}