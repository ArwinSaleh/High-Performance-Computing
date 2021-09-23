#include <stdio.h>
#include <stdlib.h>

void writeToFile(char *fileName){
    printf("TEST1");
    const size_t size = 1048576;
    printf("TEST2");
    size_t * big_array = (size_t*) malloc(sizeof(size_t) * size*size);
    printf("TEST3");
    for ( size_t ix = 0; ix < size; ++ix ){
        big_array[ix] = ix;
    }

    FILE *data_file = fopen(fileName, "w");
    fwrite (big_array , sizeof(size_t), sizeof(big_array), data_file);
    fclose(data_file);
    printf("\nMatrix data has been stored in %s\n", fileName);
    free(big_array);
}

void readAndCheckFile(char *fileName)
{
    const int size = 10;
    int isCorrect = 1;

    FILE *data_file = fopen(fileName, "r");
    for ( size_t ix = 0; ix < size; ++ix )
    {
        for ( size_t jx = 0; jx < size; ++jx )
        {
            size_t currentElement;
            fscanf(data_file, "%ld", &currentElement);
            if (currentElement != ix * jx)
            {
                isCorrect = 0;
                break;
            }
            if (!isCorrect)
            {
                break;
            }
        }
    }
    fclose(data_file);

    if (isCorrect){
        printf("\nEach element in the %s matrix is equal to ix*jx\n", fileName);
    }
    else if (!isCorrect)
    {
        printf("\nERROR! Each element in the %s matrix is NOT equal to ix*jx\n", fileName);
    }
}

int main(){
    printf("TEST");
    writeToFile("file.dat");
    //readAndCheckFile("file.dat");

}
