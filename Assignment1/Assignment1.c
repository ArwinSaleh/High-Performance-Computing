#include <stdio.h>

void mul_cpx
(
    double *a_re,
    double *a_im,
    double *b_re,
    double *b_im,
    double *c_re,
    double *c_im
);

int main()
{
    double a_re;
    double a_im;
    double b_re = 1;
    double b_im = 1;
    double c_re = 1;
    double c_im = 1;
    
    mul_cpx
    (
    &a_re,
    &a_im,
    &b_re,
    &b_im,
    &c_re,
    &c_im
    );

    printf("\na_re: %f\na_im: %f\n\n", a_re, a_im);

}

void mul_cpx
(
    double *a_re,
    double *a_im,
    double *b_re,
    double *b_im,
    double *c_re,
    double *c_im
)
{
    /*  Multiplication of two complex numbers b and c

    (b_re + b_im * i) * (c_re + c_im * i) = b_re * (c_re + c_im * i) + b_im * i * (c_re + c_im * i) = 
    
        = b_re * c_re + b_re * c_im * i + b_im * i * c_re - b_im * c_im = 

            = (b_re * c_re - b_im * c_im) + i(b_re * c_im + b_im * c_re)
    */
    
    *a_re = (*b_re * *c_re) - (*b_im * *c_im);
    *a_im = (*b_re * *c_im) + (*b_im * *c_re);
}