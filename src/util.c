#include "util.h"
#include<string.h>

char *basename (char *filename)
{
    char *token = strtok(filename, "/");
    char *previous;
    while (token != NULL)
    {
        previous = token;
        token = strtok(NULL, "/");
    }
    return previous;
}
