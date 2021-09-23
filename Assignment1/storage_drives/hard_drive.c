#include <stdio.h>
#include <stdlib.h>

void writeToFile(char *fileName){
    const size_t size = 1048576;
    size_t * big_array = (size_t*) malloc(sizeof(size_t) * size);
    if (big_array == NULL) {fputs ("Memory error in WRITE",stderr); exit (2);}
    for ( size_t ix = 0; ix < size; ++ix ){
        big_array[ix] = ix;
    }

    FILE *data_file = fopen(fileName, "w");
    fwrite(big_array, size*sizeof(size_t), 2, data_file);
    fclose(data_file);
    //printf("\nMatrix data has been stored in %s\n", fileName);
    free(big_array);
}

void readAndCheckFile(char *fileName)
{
    FILE *data_file = fopen(fileName, "rb");
    long lSize;

    // obtain file size
    fseek(data_file, 0, SEEK_END); // seek to the end of the file
    lSize = ftell(data_file);  // get the current file pointer 
    rewind(data_file);  // rewind to the beginning of the file

    size_t * buffer = (size_t*) malloc(sizeof(size_t) * lSize + 1);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    fread(buffer, 1, lSize, data_file);

    //printf("\n%lu\n", buffer[1048575]);

    fclose(data_file);
    free(buffer);
}

int main(){

    for (int bench_iter = 0; bench_iter < 1000; bench_iter++)
    {

    writeToFile("file.dat");
    readAndCheckFile("file.dat");

    }
}
