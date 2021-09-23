#include <stdio.h>
#include <stdlib.h>

void writeToFile(char *fileName){
    const size_t size = 1048576;
    size_t * big_array = (size_t*) malloc(sizeof(size_t) * size);
    for ( size_t ix = 0; ix < size; ++ix ){
        big_array[ix] = ix;
    }

    FILE *data_file = fopen(fileName, "w");
    fwrite(big_array , sizeof(size_t), sizeof(big_array), data_file);
    fclose(data_file);
    printf("\nMatrix data has been stored in %s\n", fileName);
    free(big_array);
}

void readAndCheckFile(char *fileName)
{
    const size_t size = 1048576;
    size_t * big_array = (size_t*) malloc(sizeof(size_t) * size);
    if (big_array == NULL) {fputs ("Memory error",stderr); exit (2);}
    size_t result;

    FILE *data_file = fopen(fileName, "rb");

    fread(big_array , sizeof(size_t), sizeof(big_array), data_file);

    // copy the file into the buffer:
    result = fread(big_array, 1, size, data_file);
    if (result != size) {fputs ("Reading error",stderr); exit (3);}
    printf("\nMatrix data has been stored in %s\n", fileName);

    fclose(data_file);
    free(big_array);
}

int main(){
    
    writeToFile("file.dat");
    readAndCheckFile("file.dat");

}
