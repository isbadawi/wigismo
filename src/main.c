#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tree.h"

void yyparse();

SERVICE *theservice;
char *infile;
extern int lineno;

void print_usage(void)
{
    printf("Usage: wigismo [options] file\n"
           "Options:\n"
           "  --help                Display this information.\n"
           "  --pretty-print        Pretty print parsed file to stdout.\n");
}

int main(int argc, char *argv[])
{
    int pretty_print = 0;
    int i;
    if (argc == 1)
    {
        print_usage();
        return 0;
    }
    for (i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "--help"))
        {
            print_usage();
            return 0;
        }
        if (!strcmp(argv[i], "--pretty-print"))
            pretty_print = 1;
    }
    infile = argv[argc - 1];
    if (freopen(infile, "r", stdin) != NULL)
    {
        lineno = 1;
        yyparse();
    }
    else 
    {
        fprintf(stderr, "Unable to open file: %s\n", infile);
        return 1;
    }
    printf("Testing: these are the session names in the given service:\n");
    SESSION *s;
    for (s = theservice->sessions; s != NULL; s = s->next)
        printf("%s\n", s->name);
    return 0;
}
