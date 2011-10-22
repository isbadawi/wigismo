#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> /* for basename */
#include"tree.h"
#include"pretty.h"
#include"weed.h"
#include"symbol.h"

void yyparse();

SERVICE *theservice;
char *infile;
extern int lineno;
extern int errors;

void print_usage(void)
{
    printf("Usage: wigismo [options] file\n"
           "Options:\n"
           "  -h, --help                Display this information.\n"
           "  -p, --pretty-print        Pretty print parsed file to stdout.\n"
           "  -s, --no-symbols          Disable symbol table phase)\n");
}

char* options[] = {"-h", "--help", 
                   "-p", "--pretty-print", 
                   "-s", "--no-symbols"};

int valid_option(char *opt)
{
    int i;
    for (i = 0; i < 6; i++)
        if (!strcmp(opt, options[i]))
            return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    int pretty_print = 0;
    int symbol_phase = 1;
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
        if (!strcmp(argv[i], "--no-symbols") || !strcmp(argv[i], "-s"))
            symbol_phase = 0;
    }
    if (!valid_option(argv[argc - 1]) && freopen(argv[argc - 1], "r", stdin) != NULL)
    {
        infile = basename(strdup(argv[argc - 1]));
        lineno = 1;
        yyparse();
    }
    /* read from stdin, useful for piping */
    else 
    {
        lineno = 1;
        yyparse();
    }
    /* if there's a syntax error, theservice won't be constructed, and 
       attempting to walk the AST will result in a segfault. */
    if (theservice == NULL)
        return 1;
    
    if (!weedSERVICE(theservice))
        return 1; 
    
    if (symbol_phase)
        symSERVICE(theservice);

    if (errors)
        return 1;

    if (pretty_print)
        prettySERVICE(theservice);


    return 0;
}
