#include <stdio.h>
#include <time.h>

int main()
{
    const unsigned long size = 1000000000;
    unsigned long sum = 0;

    clock_t begin = clock();
    for (unsigned long i = 1; i <= size; ++i)
    {
        sum += i;
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("The sum is %lu for size %lu \nand the time it took is %f seconds\n", sum, size, time_spent);
}