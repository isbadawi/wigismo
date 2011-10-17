#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tree.h"
#include"pretty.h"

void yyparse();

SERVICE *theservice;
char *infile;
extern int lineno;

void print_usage(void)
{
    printf("Usage: wigismo [options] file\n"
           "Options:\n"
           "  -h, --help                Display this information.\n"
           "  -p, --pretty-print        Pretty print parsed file to stdout.\n");
}

char* options[] = {"--help", "--pretty-print"};

int valid_option(char *opt)
{
    int i;
    for (i = 0; i < 2; i++)
        if (!strcmp(opt, options[i]))
            return 1;
    return 0;
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
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
        {
            print_usage();
            return 0;
        }
        if (!strcmp(argv[i], "--pretty-print") || !strcmp(argv[i], "-p"))
            pretty_print = 1;
    }
    if (!valid_option(argv[argc - 1]) && freopen(argv[argc - 1], "r", stdin) != NULL)
    {
        infile = argv[argc - 1];
        lineno = 1;
        yyparse();
    }
    /* read from stdin, useful for piping */
    else 
    {
        lineno = 1;
        yyparse();
    }
    if (pretty_print)
        prettySERVICE(theservice);
}
