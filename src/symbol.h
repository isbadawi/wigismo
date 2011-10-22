#ifndef __symbol_h
#define __symbol_h

#include "chash/chash.h"
#include "tree.h"

typedef struct SymbolTable {
    chash *table;
    struct SymbolTable *next;
} SymbolTable;

SymbolTable *new_symbol_table(void);
SymbolTable *enter_new_scope(SymbolTable *table);
SYMBOL *put_symbol(SymbolTable *t, char *name, SymbolKind kind);
SYMBOL *get_symbol(SymbolTable *t, char *name);
int symbol_exists(SymbolTable *t, char *name);

SymbolTable *mst;

void symSERVICE(SERVICE *s);


#endif
