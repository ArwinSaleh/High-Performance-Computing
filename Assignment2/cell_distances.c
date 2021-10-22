#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_CHUNK_SIZE  100000
#define CHARS_PER_LINE  24
#define FILE_NAME       "cells"

static inline float fast_sqrtf(float x) {
/*
Babylonian method used for calculating
square root of float quickly.
Brought from: https://github.com/unia-sik/emsbench/blob/master/embedded/tg/sqrtf.c
*/
  union {
    int i;
    float x;
  } u;
  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1 << 22);

  u.x = u.x + x/u.x;
  u.x = 0.25f*u.x + x/u.x;

  return u.x;
}

static inline void parse_line(char * buffer, float ** cells_chunk, size_t loaded_cells)
{
    float coordinate = 0.0;
    int offset = 0;

    coordinate += ((buffer[1 + offset] - '0') * 10.00);
    coordinate += ((buffer[2 + offset] - '0') * 1.000);
    coordinate += ((buffer[4 + offset] - '0') * 0.100);
    coordinate += ((buffer[5 + offset] - '0') * 0.010);
    coordinate += ((buffer[6 + offset] - '0') * 0.001);

    if (buffer[0 + offset] == '-')
    {
        coordinate = -coordinate;
    }
    cells_chunk[loaded_cells][0] = coordinate;

    coordinate = 0.0;
    offset = 8;

    coordinate += ((buffer[1 + offset] - '0') * 10.00);
    coordinate += ((buffer[2 + offset] - '0') * 1.000);
    coordinate += ((buffer[4 + offset] - '0') * 0.100);
    coordinate += ((buffer[5 + offset] - '0') * 0.010);
    coordinate += ((buffer[6 + offset] - '0') * 0.001);

    if (buffer[0 + offset] == '-')
    {
        coordinate = -coordinate;
    }
    cells_chunk[loaded_cells][1] = coordinate;

    coordinate = 0.0;
    offset = 16;

    coordinate += ((buffer[1 + offset] - '0') * 10.00);
    coordinate += ((buffer[2 + offset] - '0') * 1.000);
    coordinate += ((buffer[4 + offset] - '0') * 0.100);
    coordinate += ((buffer[5 + offset] - '0') * 0.010);
    coordinate += ((buffer[6 + offset] - '0') * 0.001);

    if (buffer[0 + offset] == '-')
    {
        coordinate = -coordinate;
    }
    cells_chunk[loaded_cells][2] = coordinate;

}

static inline size_t read_chunk(FILE *data_file, float ** cells_chunk)
{
/*
Function read_chunk is used to consistently read MAX_CHUNK_SIZE number
of cells per chunk, this keeps memory usage below the 5M byte limit as long
as the number of cells per chunks is low enough.
1 float is 4 bytes, meaning 100,000 cells takes 3*4*100,000 = 1,200,000 bytes
of space in memory.
*/
    size_t loaded_cells = 0;
    int8_t loaded_bytes = 0;

    char buffer[CHARS_PER_LINE];

    while (loaded_cells < MAX_CHUNK_SIZE)
    {
        loaded_bytes = fread(buffer, sizeof(char), CHARS_PER_LINE, data_file);
        if (loaded_bytes == CHARS_PER_LINE)
        {
            parse_line(buffer, cells_chunk, loaded_cells);
            loaded_cells++;
        }
        else
        {
            break;
        }
    }
    return loaded_cells;
}

int main(int argc, char* argv[])
{
    if (argc >= 2 && argv[1][0] == '-' && argv[1][1] == 't' && argv[1][2] > 0)
    {
        int n_threads = argv[1][2];
        omp_set_num_threads(n_threads);
    }
    else
    {
        printf("\nWRONG INPUT\n");
        exit(1);
    }

    FILE *data_file = fopen(FILE_NAME, "r");

    /*
    3D-coordinates between -10 and 10. Highest possible
    euclidian distance between two points is therefore
    sqrt(20^2 + 20^2 + 20^2) = {ROUNDED TO TWO DECIMALS}
    = 34.64, this means that there are in total
    100 * 34.64 = 3464 unique distances in this space.
    */
    const int n_unique_distances = 3464;

    // Each index is distance*100
    int distances[n_unique_distances+1];
    for(int ix = 0; ix < n_unique_distances+1; ix++)
    {
        distances[ix] = 0;
    }

/*
1 float = 4 bytes

Running 
	valgrind ./cell_distances -t5
shows us that this program uses 4,029,520 bytes
for MAX_CHUNK_SIZE = 100000.
*/
    float * chunk1_entries = (float*) malloc(sizeof(float) * MAX_CHUNK_SIZE*3);
    float ** chunk1 = (float**) malloc(sizeof(float*) * MAX_CHUNK_SIZE);
    float * chunk2_entries = (float*) malloc(sizeof(float) * MAX_CHUNK_SIZE*3);
    float ** chunk2 = (float**) malloc(sizeof(float*) * MAX_CHUNK_SIZE);

    for ( size_t ix = 0, jx = 0; ix < MAX_CHUNK_SIZE; ++ix, jx+=3 )
    {
        chunk1[ix] = chunk1_entries + jx;
        chunk2[ix] = chunk2_entries + jx;
    }

    size_t chunk_size1, chunk_size2;
    int read_index = 0;
    while ((chunk_size1 = read_chunk(data_file, chunk1)) > 0)
    {
        #pragma omp parallel for reduction(+:distances[:(n_unique_distances+1)])
        for (size_t ix = 0; ix < chunk_size1 - 1; ix++)
        {
            for (size_t jx = ix + 1; jx < chunk_size1; jx++)
            {
                float delta_x = chunk1[ix][0] - chunk1[jx][0];
                float delta_y = chunk1[ix][1] - chunk1[jx][1];
                float delta_z = chunk1[ix][2] - chunk1[jx][2];
                float squared = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
                float distance = fast_sqrtf(squared);
                int distance_index = 100 * (distance + 0.005);
		distances[distance_index]++;
            }
        }
        while ((chunk_size2 = read_chunk(data_file, chunk2)) > 0)
        {
            #pragma omp parallel for collapse(2) reduction(+:distances[:(n_unique_distances+1)])
            for (size_t iy = 0; iy < chunk_size1; iy++)
            {
                for (size_t jy = 0; jy < chunk_size2; jy++)
                {
                    float delta_x = chunk1[iy][0] - chunk2[jy][0];
                    float delta_y = chunk1[iy][1] - chunk2[jy][1];
                    float delta_z = chunk1[iy][2] - chunk2[jy][2];
                    float squared = delta_x * delta_x + delta_y * delta_y + delta_z * delta_z;
                    float distance = fast_sqrtf(squared);
                    int distance_index = 100 * (distance + 0.005);
                    distances[distance_index]++;
                }
            }
        }
        fseek(data_file, (read_index + 1) * MAX_CHUNK_SIZE * CHARS_PER_LINE, SEEK_SET);
	read_index++;
    }
    fclose(data_file);

    free(chunk1_entries);
    free(chunk1);
    free(chunk2_entries);
    free(chunk2);

    for (int i = 0; i <= n_unique_distances; i++)
    {
	int current_dist_count = distances[i];
        if (current_dist_count > 0)
        {
            printf("%05.2f %d\n", i*0.01, current_dist_count);
        }
    }
}
