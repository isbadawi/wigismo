#include<stdio.h>
#include<string.h>
#include "minunit.h"
#include "chash.h"

int tests_run = 0;

chash* table;
int items_deleted;

void increment_delete_count(void *data)
{
    items_deleted++;
}

void set_up(void)
{
    items_deleted = 0;
    table = chash_new();
    table->free = increment_delete_count;
}

void tear_down(void)
{
    chash_free(table);
}

char* test_new_chash(void)
{
    mu_assert("chash_new returns NULL", table != NULL);
    mu_assert("chash_new returns table with non-zero size", table->size == 0);
    return 0;
}

char* test_get_missing_key(void)
{
    void *data = chash_get(table, "nosuchkey");
    mu_assert("chash_get returns non-NULL for missing key", data == NULL);
    return 0;
}

char* test_get_existing_key(void)
{
    int x = 4;
    chash_put(table, "x", &x);
    int data = *CHASH_GET_AS(int, table, "x");
    mu_assert("chash_get returns wrong data for key", data == 4);
    return 0;
}

char* test_put_same_key_twice(void)
{
    int x1 = 4;
    int x2 = 5;
    chash_put(table, "x", &x1);
    chash_put(table, "x", &x2);
    mu_assert("chash_put does not delete old data", items_deleted == 1);
    int data = *CHASH_GET_AS(int, table, "x");
    mu_assert("chash_get returns wrong data for duplicate key", data == 5);
    return 0;
}

char* test_delete_key(void)
{
    int x = 4;
    chash_put(table, "x", &x);
    chash_del(table, "x");
    mu_assert("chash_del doesn't delete data", items_deleted == 1);
    void* data = chash_get(table, "x");
    mu_assert("chash_get return non-NULL for deleted key", data == NULL);
    return 0;
}

int array_contains(char* str, char** array, int size)
{
    while (size--)
        if (!strcmp(array[size], str))
            return 1;
    return 0;
}

char* test_get_all_keys(void)
{
    int x = 4;
    char y = 'a';
    chash_put(table, "x", &x);
    chash_put(table, "y", &y);
    chash_put(table, "test", "test");
    char** keys = chash_keys(table);
    mu_assert("chash_keys is missing a key", array_contains("x", keys, 3));
    mu_assert("chash_keys is missing a key", array_contains("y", keys, 3));
    mu_assert("chash_keys is missing a key", array_contains("test", keys, 3));
    return 0;
}

char* test_get_all_keys_and_values(void)
{
    int x = 4;
    int y = 5;
    int test = 6;
    chash_put(table, "x", &x);
    chash_put(table, "y", &y);
    chash_put(table, "test", &test);
    char** keys = chash_keys(table);    
    void** values = chash_values(table);
    int i;
    for (i = 0; i < table->size; ++i)
    {
        int expected_val = *((int*)values[i]);
        int actual_val = *CHASH_GET_AS(int, table, keys[i]);
        mu_assert("chash_keys and chash_values don't return items in the same"
                  "order", expected_val == actual_val);
    }
    return 0;
}

char* test_get_items(void)
{
    int x = 4;
    int y = 5;
    int test = 6;
    chash_put(table, "x", &x);
    chash_put(table, "y", &y);
    chash_put(table, "test", &test);
    char** keys = chash_keys(table);    
    void** values = chash_values(table);
    chash_item** items = chash_items(table);
    int i; 
    for (i = 0; i < table->size; ++i)
    {
        mu_assert("chash_items returns keys in wrong order", 
                  !strcmp(items[i]->key, keys[i]));
        mu_assert("chash_items returns values in wrong order",
                  values[i] == items[i]->data);
    }
    return 0;
}

char* test_put_put_del_put(void)
{
    chash_put(table, "x", "test");
    chash_put(table, "x", "test2");
    chash_del(table, "x");
    chash_put(table, "x", "newval");
    mu_assert("put_put_del_put should entail 2 deletes", items_deleted == 2);
    return 0;
}
