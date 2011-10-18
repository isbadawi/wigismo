#include"weed.h"
#include"tree.h"
#include<stdlib.h>
#include<stdio.h>

char *infile = "TESTING";
int lineno = 1;

int test_weed_void_var_in_schema(void)
{
    SCHEMA *void_schema = makeSCHEMA("empty", makeVARIABLE(makeTYPEvoid(), "var"));
    return !weedSCHEMA(void_schema);
}

int test_weed_returning_void_function(void)
{
    FUNCTION *f = makeFUNCTION(makeTYPEvoid(), "f", NULL, makeSTATEMENTblock(
        makeSTATEMENTreturn(makeEXPintconst(1)),NULL));
    return !weedFUNCTION(f);
}

int main(int argc, const char *argv[])
{
    int failures = 0;
    if (!test_weed_void_var_in_schema())
    {
        printf("test_weed_void_var_in_schema failed.\n");
        failures++;
    }
    if (!test_weed_returning_void_function())
    {
        printf("test_weed_returning_void_function failed.\n");
        failures++;
    }
    
    if (failures == 0)
        printf("All unit tests pass.\n");

    return 0;
}
