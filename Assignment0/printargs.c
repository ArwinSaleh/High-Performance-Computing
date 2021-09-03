#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int argAval, argBval;
    int argA, argB = 0;

    for (int i = 0; i < argc; ++i)
    {
        char *currentArgument = argv[i];

        if (currentArgument[0] == '-' && strlen(currentArgument) > 2)
        {
            size_t argLen = strlen(currentArgument);

            char *firstTwo = malloc(2 * sizeof(char));
            if (!firstTwo) 
            {
            /* handle error */
            }

            char *rest = malloc((argLen - 2) * sizeof(char));
            if (!rest) 
            {
            /* handle error */
            }

            memcpy(firstTwo, currentArgument, 2 * sizeof(char));
            memcpy(rest, currentArgument + 2, (argLen - 2) * sizeof(char));

            if (firstTwo[1] == 'a')
            {
                sscanf(rest, "%d", &argAval);
                argA = 1;
            }
            else if (firstTwo[1] == 'b')
            {
                sscanf(rest, "%d", &argBval);
                argB = 1;
            }
            free(firstTwo);
            free(rest);
        }
    }
    if (argA == 1 && argB == 1)
    {
        printf("\nA is %d and B is %d\n", argAval, argBval);
    }
    else if (argA == 1)
    {
        printf("\nA is %d\n", argAval);
    }
    else if (argB == 1)
    {
        printf("\nB is %d\n", argBval);
    }
    
    

    
}