#include<stdio.h>
#include<string.h>

void print_usage(void)
{
    printf("Usage: wigismo [options] file\n"
           "Options:\n"
           "  --help                Display this information.\n"
           "  --pretty-print        Pretty print parsed file to stdout.\n");
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        print_usage();
        return 0;
    }
    int pretty_print = 0;
    int i;
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
    char *infile = argv[argc - 1];
}
