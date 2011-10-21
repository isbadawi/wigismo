/* Hash table, mapping strings to void pointers. */
#include"chash.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

static int hash(char *str)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_SIZE;
}

static void default_free(void* p)
{
    return;
}

static chash_entry* new_chash_entry(char* key, void *data)
{
    chash_entry* new_chash_entry = (chash_entry*)malloc(sizeof(chash_entry));
    new_chash_entry->key = (char*)malloc(strlen(key) + 1);
    strcpy(new_chash_entry->key, key);
    new_chash_entry->data = data;
    new_chash_entry->next = NULL; 
    return new_chash_entry;
}

static void free_chash_entry(chash_entry* e, chash_callback_t* on_free)
{
    while (e->next != NULL)
    {
        chash_entry* temp = e;
        e = e->next;
        free(temp->key);
        on_free(temp->data);
        free(temp);
    }
}
  
chash* chash_new(void)
{
    chash* table = (chash*)malloc(sizeof(chash));
    int i;
    for (i = 0; i < HASH_SIZE; ++i)
    {
        table->table[i] = (chash_entry*)malloc(sizeof(chash_entry));
        table->table[i]->key = NULL;
        table->table[i]->data = NULL;
        table->table[i]->next = NULL;
    }
    table->size = 0;
    table->free = default_free;
    return table;
}

void chash_free(chash* table)
{
    int i;
    for (i = 0; i < HASH_SIZE; ++i)
    {
        if (table->table[i]->next != NULL)
            free_chash_entry(table->table[i]->next, table->free);
        free(table->table[i]);
    }
    free(table);
}

void chash_put(chash* table, char* key, void* data)
{
    int hashed_key = hash(key);
    chash_entry* head = table->table[hashed_key];
    chash_entry* i;
    for (i = head->next; i != NULL; i = i->next)
        if (!(strcmp(i->key, key)))
        {
            void *temp = i->data;
            i->data = data;
            table->free(temp);
            return;
        }
    chash_entry* e = new_chash_entry(key, data);
    e->next = head->next;
    head->next = e;
    table->size += 1;
}

void* chash_get(chash* table, char* key)
{
    chash_entry* head = table->table[hash(key)]->next;
    for (; head != NULL; head = head->next)
        if (!strcmp(head->key, key))
            return head->data;
    return NULL;
}

void chash_del(chash* table, char* key)
{
    int hashed_key = hash(key);
    chash_entry* head = table->table[hashed_key];
    chash_entry* prev = head;
    for (head = head->next; head != NULL; head = head->next)
    {
        if (!strcmp(head->key, key))
        {
            free(head->key);
            prev->next = head->next;
            table->size -= 1;
            table->free(head->data);
            free(head);
            return;
        }
        prev = head;
    }
}

char** chash_keys(chash* table)
{
    char** keys = (char**)malloc(table->size * sizeof(char*));
    int key = 0;
    int i;
    for (i = 0; i < HASH_SIZE; ++i)
    {
        chash_entry* head;
        for (head = table->table[i]->next; head != NULL; head = head->next)
        {
            keys[key] = (char*)malloc(strlen(head->key + 1));
            strcpy(keys[key], head->key);
            key++;
        }
    }
    return keys;
}

void** chash_values(chash* table)
{
    void** values = (void**)malloc(table->size * sizeof(void*));
    int value = 0;
    int i;
    for (i = 0; i < HASH_SIZE; ++i)
    {
        chash_entry* head;
        for (head = table->table[i]->next; head != NULL; head = head->next)
        {
            values[value] = head->data;
            value++;
        }
    }
    return values;
}      

chash_item** chash_items(chash* table)
{
    chash_item** items = (chash_item**)malloc(table->size * sizeof(chash_item*));
    int item = 0;
    int i;
    for (i = 0; i < HASH_SIZE; ++i)
    {
        chash_entry* head;
        for (head = table->table[i]->next; head != NULL; head = head->next)
        {
            items[item] = (chash_item*)malloc(sizeof(chash_item));
            items[item]->key = (char*)malloc(strlen(head->key + 1));
            strcpy(items[item]->key, head->key);
            items[item]->data = head->data;
            item++;
        }
    }
    return items;
}

void chash_pretty_print(chash* table, chash_callback_t* print_item)
{
    printf("{\n");
    chash_item** items = chash_items(table);
    int i;
    for (i = 0; i < table->size; ++i)
    {
        printf("    \"%s\": ", items[i]->key);
        print_item(items[i]->data);
        if (i < table->size - 1)
            printf(",");
        printf("\n");
    }
    printf("}");
}
