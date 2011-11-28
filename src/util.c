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
    int len;
    int i;
    char *newname;
    if (strchr(filename, '.') == NULL)
        return filename;
    len = strlen(filename);
    for (i = len - 1; i >= 0; i--)
        if (filename[i] == '.')
            break;
    newname = (char*)malloc(i+1);
    strncpy(newname, filename, i); 
    return newname;
}

char *escape_string(char *string)
{
    /* Worst case, every char is escaped. */
    char *result = (char*)malloc(2*strlen(string) + 1);
    char *p = result;
    char s;
    while ((s = *string++))
    {
        if (s == '\n')
        {
            *p++ = '\\';
            *p++ = 'n';
        }
        else if (s == '\'')
        {
            *p++ = '\\';
            *p++ = '\'';
        }
        else if (s == '\\')
        {
            *p++ = '\\';
            *p++ = '\\';
        }
        else
            *p++ = s;
    }
    *p = '\0';
    return result;
}
