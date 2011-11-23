#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> /* for basename */
#include"tree.h"
#include"pretty.h"
#include"weed.h"
#include"symbol.h"
#include"type.h"
#include"code.h"
#include"util.h"

void yyparse();

SERVICE *theservice;
char *infile;
extern int lineno;
extern int errors;
int print_types = 1;
int print_symbols = 1;

void print_usage(void)
{
    printf("Usage: wigismo [options] file\n"
           "Options:\n"
           "  -h, --help                Display this information.\n"
           "  -p, --pretty-print        Pretty print parsed file to stdout.\n"
           "  -x, --suppress-symbols    Don't print symbol information\n"
           "  -n, --suppress-types      Don't print type information\n"
           "  -s, --no-symbols          Disable symbol table phase (implies -t, -x)\n"
           "  -t, --no-types            Disable type checking phase (implies -n, -c)\n"
           "  -c, --no-code             Disable code generation.\n"
           "  -o, --output FILE         Redirect output to file (defaults to stdout).\n");
}

int main(int argc, char *argv[])
{
    char *outfilename;
    int o_switch = 0;
    int pretty_print = 0;
    int symbol_phase = 1;
    int type_phase = 1;
    int code_phase = 1;
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
        {
            symbol_phase = 0;
            type_phase = 0;
            print_symbols = 0;
            print_types = 0;
            code_phase = 0;
        }
        if (!strcmp(argv[i], "--no-types") || !strcmp(argv[i], "-t"))
        {
            type_phase = 0;
            print_types = 0;
            code_phase = 0;
        }
        if (!strcmp(argv[i], "--suppress-symbols") || !strcmp(argv[i], "-x"))
            print_symbols = 0;
        if (!strcmp(argv[i], "--suppress-types") || !strcmp(argv[i], "-n"))
            print_types = 0;
        if (!strcmp(argv[i], "--no-code") || !strcmp(argv[i], "-c"))
            code_phase = 0;
        if (!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o"))
        {
            i++;
            outfilename = argv[i];
            o_switch = 1;
        }
    }
    if (freopen(argv[argc - 1], "r", stdin) != NULL)
    {
        if (!o_switch)
            outfilename = replace_extension(argv[argc - 1], ".py");
        infile = basename(strdup(argv[argc - 1]));
        lineno = 1;
        yyparse();
    }
    else
        return 1;

    /* if there's a syntax error, theservice won't be constructed, and 
       attempting to walk the AST will result in a segfault. */
    if (theservice == NULL)
        return 1;
    theservice->name = strip_extension(argv[argc - 1]);
    
    if (!weedSERVICE(theservice))
        return 1; 
    
    if (symbol_phase)
        symSERVICE(theservice);

    if (errors)
        return 1;

    if (type_phase)
        typeSERVICE(theservice);

    if (errors)
        return 1;

    if (pretty_print)
        prettySERVICE(theservice);

    if (code_phase)
    {
        FILE *outfile = fopen(outfilename, "w");
        codeSERVICE(theservice, outfile);
    }

    return 0;
}
