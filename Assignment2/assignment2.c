#include <stdio.h>


size_t distance_counter = 0;

size_t rows = 429496729;

int main()
{

    for (size_t ix = 0; ix < rows - 1; ix++)
    {

        for (size_t jx = ix + 1; jx < rows; jx++)
        {
            distance_counter += 1;
        }
    }

    printf("DISTANCES: %ld", distance_counter);
}