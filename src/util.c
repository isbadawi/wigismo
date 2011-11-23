#include "util.h"
#include <string.h>
#include <stdlib.h>

char* replace_extension(char *filename, char *ext)
{
    char *base = strip_extension(filename);
    char *newname = malloc(strlen(base) + strlen(ext) + 1);
    strcpy(newname, base);
    strcat(newname, ext);
    return newname;
}

char *strip_extension(char *filename)
{
    if (strchr(filename, '.') == NULL)
        return filename;
    int len = strlen(filename);
    int i;
    for (i = len - 1; i >= 0; i--)
        if (filename[i] == '.')
            break;

    char *newname = (char*)malloc(i+1);
    strncpy(newname, filename, i); 
    return newname;
}
 
