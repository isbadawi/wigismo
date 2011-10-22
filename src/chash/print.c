#include"chash.h"
#include<stdio.h>

void print_int(void* i)
{
    printf("%d", *((int*)i));
}

int main(void)
{
    chash *table = chash_new();
    int x = 4;
    int y = 5;
    chash_put(table, "x", &x);
    chash_put(table, "y", &y);
    chash_pretty_print(table, print_int);
}
