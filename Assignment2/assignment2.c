#include <stdio.h>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

void read_chunk(FILE *data_file, float ** cells_chunk, const int chunk_size);

int main()
{

    //const size_t n_cells = 429496729;
    //const size_t n_distances = n_cells * (n_cells - 1) / 2;

    //const size_t cell_chunk = 60000;

    const char *file_name = "cells.txt";


    FILE *data_file = fopen(file_name, "r");

    long lSize;

    // obtain file size
    fseek(data_file, 0, SEEK_END); // seek to the end of the file
    lSize = ftell(data_file);  // get the current file pointer 
    rewind(data_file);  // rewind to the beginning of the file

    // (+) or (-) sign, 2 digits and 3 decimal points,
    // 3 coordinates, 2 even spaces, 1 '\n'.
    // In total: 24 characters, no '\n' on last row.
    const size_t n_cells = lSize/(sizeof(char)*24) + 1;
    const size_t chunk_size = 1;

    /*
    3D-coordinates between -10 and 10. Highest possible
    euclidian distance between two points is therefore
    sqrt(20^2 + 20^2 + 20^2) = {ROUNDED TO TWO DECIMALS}
    = 34.64, this means that there are in total 
    100 * 34.64 = 3464 unique distances in this space.
    */
    const int n_unique_distances = 3464;
    
    // Each index is distance*100
    int distances[n_unique_distances];
    for(int ix = 0; ix < n_unique_distances; ix++)
    {
        distances[ix] = 0;
    }

    int distance_index;

    const size_t n_chunks = n_cells / chunk_size;
    
    float * chunk1_entries = (float*) malloc(sizeof(float) * chunk_size * 3);
    float ** chunk1;
    chunk1 = malloc(sizeof(*chunk1)*chunk_size + sizeof(**chunk1) * chunk_size * 3);
    float * chunk2_entries = (float*) malloc(sizeof(float) * chunk_size * 3);
    float ** chunk2;
    chunk2 = malloc(sizeof(*chunk2)*chunk_size + sizeof(**chunk2) * chunk_size * 3);

    for ( size_t ix = 0, jx = 0; ix < chunk_size; ++ix, jx+=3 )
    {
        chunk1[ix] = chunk1_entries + jx;
        chunk2[ix] = chunk2_entries + jx;
    }

    for (size_t chunk1_index = 0; chunk1_index < n_chunks; chunk1_index++)
    {
        read_chunk(data_file, chunk1, chunk_size);
        for (size_t ix = 0; ix < chunk_size - 1; ix++)
        {
            for (size_t jx = ix + 1; jx < chunk_size; jx++)
            {
                distance_index = sqrt(  (chunk1[ix][0] - chunk1[jx][0]) * (chunk1[ix][0] - chunk1[jx][0]) +
                                        (chunk1[ix][1] - chunk1[jx][1]) * (chunk1[ix][1] - chunk1[jx][1]) +
                                        (chunk1[ix][2] - chunk1[jx][2]) * (chunk1[ix][2] - chunk1[jx][2])) * 100;
                distances[(int)distance_index] += 1;
            }
        }
        for (size_t chunk2_index = 0; chunk2_index < n_chunks; chunk2_index++)
        {
            read_chunk(data_file, chunk2, chunk_size);
            for (size_t iy = 0; iy < chunk_size; iy++)
            {
                for (size_t jy = 0; jy < chunk_size; jy++)
                {
                    distance_index = sqrt(  (chunk1[iy][0] - chunk2[jy][0]) * (chunk1[iy][0] - chunk2[jy][0]) +
                                            (chunk1[iy][1] - chunk2[jy][1]) * (chunk1[iy][1] - chunk2[jy][1]) +
                                            (chunk1[iy][2] - chunk2[jy][2]) * (chunk1[iy][2] - chunk2[jy][2])) * 100;
                    distances[(int)distance_index] += 1;
                }
            }
        }
        fseek(data_file, (chunk1_index + 1) * n_cells * 24, SEEK_SET);
    }
    fclose(data_file);

    free(chunk1_entries);
    free(chunk1);
    free(chunk2_entries);
    free(chunk2);

    FILE * pFile = fopen ("OUTPUT.txt","w");
    for (int i = 0; i <= n_unique_distances; i++)
    {
        fprintf(pFile, "%f: %d\n", i*0.01, distances[i]);
    }
    fclose(pFile);
}

void read_chunk(    FILE *data_file, 
                    float ** cells_chunk, 
                    const int chunk_size    )
{
    for (int i = 0; i < chunk_size; i++)
    {
        fscanf(data_file, "%f %f %f", &cells_chunk[i][0], &cells_chunk[i][1], &cells_chunk[i][2]);
    }
}