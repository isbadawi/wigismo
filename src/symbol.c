#include "symbol.h"
#include "memory.h"
#include<stdlib.h>

SymbolTable *new_symbol_table(void) 
{
    SymbolTable *table = NEW(SymbolTable);
    table->table = chash_new();
    table->next = NULL;
    return table;
}

SymbolTable *enter_new_scope(SymbolTable *table) 
{
    SymbolTable *new_scope = new_symbol_table();
    new_scope->next = table;
    return new_scopel
}

SYMBOL *get_symbol(SymbolTable *table, char *name)
{
    if (table == NULL)
        return NULL;
    SYMBOL *symbol = CHASH_GET_AS(SYMBOL, table->table, name);
    if (symbol == NULL)
        return get_symbol(table->next, name);
    return symbol;
}

SYMBOL *put_symbol(SymbolTable *table, char *name, SymbolKind kind)
{
    SYMBOL *symbol = CHASH_GET_AS(SYMBOL, table->table, name);
    if (symbol != NULL)
        return symbol;
    symbol = NEW(SYMBOL);
    symbol->name = name;
    symbol->kind = kind;
    return symbol;
}

int symbol_exists(SymbolTable *table, char *name)
{
    return get_symbol(table, name) != NULL;
}
