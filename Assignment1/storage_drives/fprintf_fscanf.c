#include <stdio.h>
#include <stdlib.h>


void writeToFile(char *fileName){
    const size_t size = 1048576;

    FILE *data_file = fopen(fileName, "w");
    for ( size_t ix = 0; ix < size; ++ix ){
        fprintf(data_file, "%ld\t", ix);
        fprintf(data_file, "\n\n");
    }
    fclose(data_file);
    //printf("\nMatrix data has been stored in %s\n", fileName);
}

void readAndCheckFile(char *fileName)
{
    const size_t size = 1048576;
    size_t * big_array = (size_t*) malloc(sizeof(size_t) * size);

    FILE *data_file = fopen(fileName, "r");
    for ( size_t ix = 0; ix < size; ++ix )
    {
        fscanf(data_file, "%ld", &big_array[ix]);
    }
    fclose(data_file);
    //printf("\n%lu\n", big_array[1048575]);
    free(big_array);
}

int main(){
    for(int bench_iter = 0; bench_iter = 1; bench_iter++)
    {
        writeToFile("file.dat");
        readAndCheckFile("file.dat");
    }
}
