#include "error.h"
#include<stdio.h>

extern char *yytext;
extern char *infile;
int lineno;
int errors = 0;

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
    fprintf(stderr, "*** syntax error before %s at line %i of file %s\n", 
            yytext, lineno, infile);
    fprintf(stderr, "*** compilation terminated ***\n");
}

void reportStrError(const char *msg, char *name, int lineno)
{ 
    fprintf(stderr, "*** ");
    fprintf(stderr, msg, name);
    fprintf(stderr, " at line %i of file %s\n", lineno, infile);
    errors++;
}

void reportError(char *s, int lineno)
{
    fprintf(stderr, "*** ");
    fprintf(stderr, "%s at line %i\n", s, lineno);
    errors++;
}
