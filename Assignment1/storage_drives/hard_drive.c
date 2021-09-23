#include <stdio.h>
#include <stdlib.h>

void writeToFile(char *fileName){
    const int size = 1048576;
    int * big_array = (int*) malloc(sizeof(int) * size);
    if (big_array == NULL) {fputs ("Memory error in WRITE",stderr); exit (2);}
    for ( int ix = 0; ix < size; ++ix ){
        big_array[ix] = ix;
    }

    FILE *data_file = fopen(fileName, "w");
    fwrite(big_array, size*sizeof(int), 2, data_file);
    fclose(data_file);
    printf("\nMatrix data has been stored in %s\n", fileName);
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

    int * buffer = (int*) malloc(sizeof(int) * lSize + 1);
    if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

    fread(buffer, 1, lSize, data_file);

    printf("\n%lu\n", buffer[1048575]);

    fclose(data_file);
    free(buffer);
}

int main(){

    writeToFile("file.dat");
    readAndCheckFile("file.dat");

}
