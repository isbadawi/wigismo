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
    return new_scope;
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

SYMBOL *get_symbol_as(SymbolTable *table, char *name, SymbolKind kind)
{
    if (table == NULL)
        return NULL;
    SYMBOL *symbol = CHASH_GET_AS(SYMBOL, table->table, name);
    if (symbol == NULL || symbol->kind != kind)
        return get_symbol_as(table->next, name, kind)l
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
    chash_put(table->table, name, symbol);
    return symbol;
}

int symbol_exists(SymbolTable *table, char *name)
{
    return get_symbol(table, name) != NULL;
}

int local_symbol_exists(SymbolTable *table, char *name)
{
    return chash_get(table->table, name) != NULL;
}

void symSERVICE(SERVICE *s)
{
    mst = new_symbol_table();
    symHTML(s->htmls);
    symSCHEMA(s->schemas);
    symVARIABLE(s->variables, mst);
    symFUNCTION(s->functions);
    symFUNCTIONBODIES(s->functions);
    symSESSION(s->sessions);
}

void symHTML(HTML *html)
{
    if(html == NULL)
        return;
    symHTML(html->next);
    if(symbol_exists(mst, html->name))
        reportStrError("html %s already defined", html->name, html->lineno);
    else
    {
        SYMBOL *s = put_symbol(mst, html->name, htmlSym);
        s->val.htmlS = html;
    }
}

void symSCHEMA(SCHEMA *s)
{
    if(s == NULL)
        return;
    symSCHEMA(s->next);
    if(symbol_exists(mst, s->name))
        reportStrError("schema %s already defined", s->name, s->lineno);
    else
    {
        SYMBOL *sym = put_symbol(mst, s->name, schemaSym);
        sym->val.schemaS = s;
    }
}

void symVARIABLE(VARIABLE *v, SymbolTable *table)
{
    if(v == NULL)
        return;
    symVARIABLE(v->next, table);
    if(local_symbol_exists(table, v->name))
        reportStrError("variable %s already defined", v->name, v->lineno);
    else
    {
        SYMBOL *s = put_symbol(table, v->name, variableSym);
        s->val.variableS = v;
        if(v->type->kind == tupleK)
        {
            SYMBOL *schema = get_symbol_as(mst, v->type->name, schemaSym)
            if (schema == NULL)
                reportStrError("tuple %s type not defined", v->name, v->lineno);
        }
    }
}

void symFUNCTION(FUNCTION *f)
{
    if (f == NULL)
        return;
    symFUNCTION(f->next);
    if (symbol_exists(mst, f->name))
        reportStrError("function %s already defined", f->name, f->lineno);
    else
    {
        SYMBOL *s = put_symbol(mst, f->name, functionSym);
        s->val.functionS = f;
    }
}

void symFUNCTIONBODIES(FUNCTION *f)
{
    if (f == NULL)
        return;
    symFUNCTIONBODIES(f->next);
    SymbolTable *local = new_symbol_table(mst);
    symARGUMENT(f->arguments, local);
    symSTATEMENT(f->statements, local);
}

void symARGUMENT(ARGUMENT *a, SymbolTable *table)
{
    if (a == NULL)
        return;
    symARGUMENT(a->next, table);
    if (local_symbol_exists(a->name, table))
        reportStrError("argument %s already defined", a->name, a->lineno);
    if (a->type->kind == tupleK)
    {
        SYMBOL *schema = get_symbol_as(mst, a->type->name, schemaSym)
        if (schema == NULL)
            reportStrError("tuple %s type not defined", a->name, a->lineno);
    }                     
}

void symSTATEMENT(STATEMENT *s, SymbolTable *table)
{
    SymbolTable *local_table;
    if (s == NULL)
        return;
    switch (s->kind)
    {
        case skipK:
            break;
        case sequenceK:
            symSTATEMENT(s->val.sequenceS.first, table);
            symSTATEMENT(s->val.sequenceS.second, table);
            break;
        case showK:
            symDOCUMENT(s->val.showS.document, table);
            symRECEIVE(s->val.showS.receives, s->val.showS.document, table);
            break;
        case exitK:
            symDOCUMENT(s->val.exitS.document, table);
            break;
        case returnK:
            symEXP(s->val.returnS, table);
            break;
        case blockK:
            local_table = enter_new_scope(table);
            symVARIABLE(s->val.blockS.variables, local_table);
            symSTATEMENT(s->val.blockS.body, local_table);
            break;
        case ifK:
            symEXP(s->val.ifS.condition, table);
            symSTATEMENT(s->val.ifS.body);
            break;
        case ifelseK:
            symEXP(s->val.ifelseS.condition, table);
            symSTATEMENT(s->val.ifelseS.thenpart);
            symSTATEMENT(s->val.ifelseS.elsepart);
            break;
        case whileK:
            symEXP(s->val.whileS.condition, table);
            symSTATEMENT(s->val.whileS.body);
            break;
        case expK:
            symEXP(s->val.expS, table);
            break;
    }
}

void symDOCUMENT(DOCUMENT *d, SymbolTable *table)
{
    SYMBOL *html = get_symbol_as(mst, d->name, htmlSym);
    if (html == NULL)
        reportStrError("HTML %s not defined", d->name, d->lineno);
    else
        symPLUG(d->plugs, html->val.htmlS, table);
}

int html_has_gap(HTMLBODY *hb, char *name)
{
    if (hb == NULL)
        return 0;
    if (hb->kind == gapK && !strcmp(hb->val.gapH, name))
        return 1;
    return html_has_gap(hb->next, name);
}

void symPLUG(PLUG *p, HTML *html, SymbolTable *table)
{
    if (p == NULL)
        return;
    symPLUG(p->next, html, table);
    if (!html_has_gap(html->htmlbodies, p->name))
        reportStrError("gap %s not found in HTML", p->name, p->lineno);
    symEXP(p->exp, table);
}

void symRECEIVE(RECEIVE *r, DOCUMENT *d, SymbolTable *table)
{
    if (r == NULL)
        return;
    symRECEIVE(r->next, d, table);
    SYMBOL *html = get_symbol_as(mst, d->name, htmlSym);
    if (html != NULL)
    {
        HTML *h = html->val.htmlS;
        symINPUT(r->inputs, h, table);
    }
}

char* get_name(ATTRIBUTE *attributes)
{
    if (!strcmp(attribute->name, "name"))
        return attribute->value;
    return get_name(attributes->next);
}

int html_has_input(HTMLBODY *hb, char *name)
{
    if (hb == NULL)
        return 0;
    if (hb->kind == inputK 
          && !strcmp(get_name(hb->val.inputH.attributes), name))
        return 1;
    if (hb->kind == selectK 
          && !strcmp(get_name(hb->val.selectH.attributes), name))
        return 1;
    return html_has_input(hb->next);
}

void symINPUT(INPUT *i, HTML *h, SymbolTable *table)
{
    if (i == NULL)
        return;
    symINPUT(i->next);
    SYMBOL *var = get_symbol(table, i->lhs);
    if (var == NULL || (var->kind != variableSym && var->kind != argumentSym))
        reportStrError("identifier %s not declared", i->lhs, i->lineno);
    if (!html_has_input(h->htmlbodies, i->rhs))
        reportStrError("input %s not found in HTML", i->rhs, i->lineno);
}

void symEXP(EXP *e, SymbolTable *table)
{
    if (e == NULL)
        return;
    symEXP(e->next, table);
    
    switch (e->kind)
    {
        case idK:
            SYMBOL *s = get_symbol(table, e->name);
            if(s == NULL)
                reportStrError("Identifier %s not declared", e->name, e->lineno);
                
            break;
        case assignK:
        case binaryK:
        case keepK:
        case discardK:
        case unaryK:
        case callK:
        case intconstK:
        case boolconstK:
        case stringconstK:
        case tupleK:
    }
}
