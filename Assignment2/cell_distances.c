#include <stdio.h>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

double randfrom(double min, double max) 
{
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

double round(double var)
{
    // Rounds to 2 digits.
    double value = (int)(var * 100 + .5);
    return (double)value / 100;
}

void init_threads(const int num_threads)
{
    omp_set_dynamic(0);     // Explicitly disable dynamic teams
    omp_set_num_threads(num_threads); // Use num_threads threads for all consecutive parallel regions
}

void printThreadCount()
{
    #pragma omp parallel
    {
        printf("Hello from process: %d\n", omp_get_thread_num());
    }
}

void readFile(double * cell_entries, char *file_name, const int size)
{
    FILE *data_file = fopen(file_name, "r");
    /*
    for (size_t i = 0; i < size; i++)
    {
        double x, y, z;
        sscanf(file_name, "%lf %lf %lf", &x, &y, &z);
        printf("\n(x, y, z): (%lf, %lf, %lf)\n", x, y, z);
    }
    */

    for ( size_t ix = 0; ix < size*3; ix=ix+3 )
    {
        double currentElement;
        fscanf(data_file, "%lf %lf %lf", &cell_entries[ix], &cell_entries[ix+1], &cell_entries[ix+2]);
        printf("\nCURRENT: %lf %lf %lf", cell_entries[ix], cell_entries[ix+1], cell_entries[ix+2]);
    }
    fclose(data_file);

}

void printCells(double ** cells, const int rows, const int cols)
{
    for(size_t ix = 0; ix < rows; ix++)
    {
        for (size_t jx = 0; jx < cols; jx++)
        {
            printf("\ncell (%li, %li): %f\n", ix, jx, cells[ix][jx]);
        }
    }
}

int main(int argc, char *argv[]){

    const size_t rows = 429496729; // = 2^32
    const size_t cols = 3;

    if (argc > 1)
    {
        if (strlen(argv[1]) > 2 && argv[1][0] == '-' && argv[1][1] == 't')
        {
            int num_threads;
            sscanf(argv[1], "-t%d", &num_threads);
            init_threads(num_threads);
        }
    }

    // Test to confirm the number threads being used
    //printThreadCount();

    double * cell_entries = (double*) malloc(sizeof(double) * rows*cols);
    double ** cells;
    cells = malloc(sizeof(*cells)*rows + sizeof(**cells) * rows * cols);
    for ( size_t ix = 0, jx = 0; ix < rows; ++ix, jx+=cols )
    cells[ix] = cell_entries + jx;

    for ( size_t ix = 0; ix < rows; ++ix )
    for ( size_t jx = 0; jx < cols; ++jx )
        cells[ix][jx] = randfrom(-10.0, 10.0);


    //readFile(cell_entries, "cells.txt", size);

    //printCells(cells, rows, cols);

    // Get total number of cell combinations.
    size_t number_of_distances = rows * (rows - 1);
    for (size_t i = 1; i < rows; i++)
    {
        number_of_distances -= i;
    }

    printf("\nTHIS: %ld\n", number_of_distances);

    double * distances = (double*) malloc(sizeof(double) * number_of_distances);

    size_t distance_counter = 0;
    for (size_t ix = 0; ix < rows - 1; ix++)
    {
        int x_ix = cells[ix][0];
        int y_ix = cells[ix][1];
        int z_ix = cells[ix][2];

        for (size_t jx = ix + 1; jx < rows; jx++)
        {
            int x_jx = cells[jx][0];
            int y_jx = cells[jx][1];
            int z_jx = cells[jx][2];

            //distances[distance_counter] = round(    sqrt(   (x_ix - x_jx) * (x_ix - x_jx) + 
            //                                                (y_ix - y_jx) * (y_ix - y_jx) + 
            //                                                (z_ix - z_jx) * (z_ix - z_jx)   )   );

            //printf("\nDISTANCE %d = %f\n", distance_counter+1, distances[distance_counter]);

            distance_counter += 1;
        }
    }

    free(distances);
    free(cell_entries);
    free(cells);
    
    return 0;
}