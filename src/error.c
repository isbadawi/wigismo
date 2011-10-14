#include "error.h"
#include<stdio.h>

extern char *yytext;
extern char *infile;
int lineno;

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
    fprintf(stderr, "*** syntax error before %s at line %i of file %s\n", 
            yytext, lineno, infile);
    fprintf(stderr, "*** compilation terminated ***\n");
}
