#include "symbol.h"
#include "memory.h"
#include "error.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void symHTML(HTML *html);
void symSESSION(SESSION *s);
void symSCHEMA(SCHEMA *s);
void symVARIABLE(VARIABLE *v, SymbolTable *table);
void symFUNCTION(FUNCTION *f);
void symFUNCTIONBODIES(FUNCTION *f);
void symARGUMENT(ARGUMENT *a, SymbolTable *table);
void symSTATEMENT(STATEMENT *s, SymbolTable *table);
void symDOCUMENT(DOCUMENT *d, SymbolTable *table);
void symPLUG(PLUG *p, HTML *html, SymbolTable *table);
void symRECEIVE(RECEIVE *r, DOCUMENT *d, SymbolTable *table);
void symTUPLE(EXP *e, SymbolTable *table);
void symINPUT(INPUT *i, HTML *h, SymbolTable *table);
void symEXP(EXP *e, SymbolTable *table);
void symID(ID *id, SymbolTable *table);
void symFIELDVALUE(FIELDVALUE *fv, SymbolTable *table);

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
    SYMBOL *symbol;
    if (table == NULL)
        return NULL;
    symbol = CHASH_GET_AS(SYMBOL, table->table, name);
    if (symbol == NULL)
        return get_symbol(table->next, name);
    return symbol;
}

SYMBOL *get_symbol_as(SymbolTable *table, char *name, SymbolKind kind)
{
    SYMBOL *symbol;
    if (table == NULL)
        return NULL;
    symbol = CHASH_GET_AS(SYMBOL, table->table, name);
    if (symbol == NULL || symbol->kind != kind)
        return get_symbol_as(table->next, name, kind);
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
            SYMBOL *schema = get_symbol_as(mst, v->type->name, schemaSym);
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
    SymbolTable *local;
    if (f == NULL)
        return;
    symFUNCTIONBODIES(f->next);
    /* remark: we enter a new scope here, and not in the block statement,
       so that function arguments and locals will be in the same scope */
    local = enter_new_scope(mst);
    f->statements->val.blockS.table = local;
    symARGUMENT(f->arguments, local);
    symVARIABLE(f->statements->val.blockS.variables, local);
    symSTATEMENT(f->statements->val.blockS.body, local);  
}

void symARGUMENT(ARGUMENT *a, SymbolTable *table)
{
    if (a == NULL)
        return;
    symARGUMENT(a->next, table);
    if (local_symbol_exists(table, a->name))
        reportStrError("argument %s already defined", a->name, a->lineno);
    else
    {
        SYMBOL *sym = put_symbol(table, a->name, argumentSym);
        sym->val.argumentS = a;
    }
    if (a->type->kind == tupleK)
    {
        SYMBOL *schema = get_symbol_as(mst, a->type->name, schemaSym);
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
            s->val.blockS.table = local_table;
            symVARIABLE(s->val.blockS.variables, local_table);
            symSTATEMENT(s->val.blockS.body, local_table);
            break;
        case ifK:
            symEXP(s->val.ifS.condition, table);
            symSTATEMENT(s->val.ifS.body, table);
            break;
        case ifelseK:
            symEXP(s->val.ifelseS.condition, table);
            symSTATEMENT(s->val.ifelseS.thenpart, table);
            symSTATEMENT(s->val.ifelseS.elsepart, table);
            break;
        case whileK:
            symEXP(s->val.whileS.condition, table);
            symSTATEMENT(s->val.whileS.body, table);
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
    SYMBOL *html;
    if (r == NULL)
        return;
    symRECEIVE(r->next, d, table);
    html = get_symbol_as(mst, d->name, htmlSym);
    if (html != NULL)
    {
        HTML *h = html->val.htmlS;
        symINPUT(r->inputs, h, table);
    }
}

char* get_name(ATTRIBUTE *attributes)
{
    if (!strcmp(attributes->name, "name"))
        return attributes->value;
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
    return html_has_input(hb->next, name);
}

void symINPUT(INPUT *i, HTML *h, SymbolTable *table)
{
    if (i == NULL)
        return;
    symINPUT(i->next, h, table);
    if (strchr(i->lhs, '.') != NULL)
        symTUPLE(makeEXPlvalue(i->lhs), table);
    else
    {
        SYMBOL *var = get_symbol(table, i->lhs);
        if (var == NULL || (var->kind != variableSym && var->kind != argumentSym))
            reportStrError("identifier %s not declared", i->lhs, i->lineno);
        if (!html_has_input(h->htmlbodies, i->rhs))
            reportStrError("input %s not found in HTML", i->rhs, i->lineno);
        i->leftsym = var;
    }
}

int schema_has_var(SCHEMA *s, char *field)
{
    VARIABLE *v;
    for (v = s->variables; v != NULL; v = v->next)
        if (!strcmp(v->name, field))
            return 1;
    return 0;
}

void symTUPLE(EXP *e, SymbolTable *table)
{
    SYMBOL *s;
    s = get_symbol(table, e->val.idtupleE.name);
    if (s == NULL || (s->kind != variableSym && s->kind != argumentSym))
        reportStrError("identifier %s not declared", e->val.idtupleE.name, e->lineno);
    if (s->kind == variableSym)
    {
        SCHEMA *schema;
        VARIABLE *v = s->val.variableS;
        if (v->type->kind != tupleK)
            reportStrError("identifier %s not a tuple", e->val.idtupleE.name, e->lineno);
        schema = get_symbol_as(table, v->type->name, schemaSym)->val.schemaS;
        if (!schema_has_var(schema, e->val.idtupleE.field))
            reportStrError("schema has no field %s", e->val.idtupleE.field, e->lineno);
        e->val.idtupleE.schema = schema;
    }
    else if (s->kind == argumentSym)
    {
        SCHEMA *schema;
        ARGUMENT *a = s->val.argumentS;
        if (a->type->kind != tupleK)
            reportStrError("identifier %s not a tuple", e->val.idtupleE.name, e->lineno);
        schema = get_symbol_as(table, a->type->name, schemaSym)->val.schemaS;
        if (!schema_has_var(schema, e->val.idtupleE.field))
            reportStrError("schema has no field %s", e->val.idtupleE.field, e->lineno);
        e->val.idtupleE.schema = schema;
    }         
}

void symEXP(EXP *e, SymbolTable *table)
{
    SYMBOL *s;
    if (e == NULL)
        return;
    symEXP(e->next, table);
    
    switch (e->kind)
    {
        case idK:
            s = get_symbol(table, e->val.idE.name);
            if(s == NULL)
                reportStrError("identifier %s not declared", e->val.idE.name, e->lineno);
            e->val.idE.idsym = s;    
            break;
        case idtupleK:
            symTUPLE(e, table);
            break;
        case assignK:
            s = get_symbol(table, e->val.assignE.left);
            if(s == NULL || (s->kind != variableSym && s->kind != argumentSym))
                reportStrError("identifier %s not declared", e->val.assignE.left, e->lineno);
            e->val.assignE.leftsym = s;
            symEXP(e->val.assignE.right, table);
            break;
        case assigntupleK:
            s = get_symbol(table, e->val.assigntupleE.name);
            if (s == NULL || (s->kind != variableSym && s->kind != argumentSym))
                reportStrError("identifier %s not declared", e->val.idtupleE.name, e->lineno);
            if (s->kind == variableSym)
            {
                SCHEMA *schema;
                VARIABLE *v = s->val.variableS;
                if (v->type->kind != tupleK)
                    reportStrError("identifier %s not a tuple", e->val.assigntupleE.name, e->lineno);
                schema = get_symbol_as(table, v->type->name, schemaSym)->val.schemaS;
                if (!schema_has_var(schema, e->val.assigntupleE.field))
                    reportStrError("schema has no field %s", e->val.assigntupleE.field, e->lineno);
                e->val.assigntupleE.schema = schema;
                
            }
            else if (s->kind == argumentSym)
            {
                SCHEMA *schema;
                ARGUMENT *a = s->val.argumentS;
                if (a->type->kind != tupleK)
                    reportStrError("identifier %s not a tuple", e->val.assigntupleE.name, e->lineno);
                schema = get_symbol_as(table, a->type->name, schemaSym)->val.schemaS;
                if (!schema_has_var(schema, e->val.assigntupleE.field))
                    reportStrError("schema has no field %s", e->val.assigntupleE.field, e->lineno);
                e->val.assigntupleE.schema = schema;
            }                     
            symEXP(e->val.assigntupleE.right, table);
            break;
        case orK:
        case andK:
        case eqK:
        case ltK:
        case gtK:
        case leqK:
        case geqK:
        case neqK:
        case plusK:
        case minusK:
        case timesK:
        case divK:
        case modK:
        case combineK:
            symEXP(e->val.binaryE.left, table);
            symEXP(e->val.binaryE.right, table);
            break;
        case notK:
        case uminusK:
            symEXP(e->val.unaryE, table);
            break;
        case keepK:
            symEXP(e->val.keepE.left, table);
            symID(e->val.keepE.ids, table);
            break;
        case discardK:
            symEXP(e->val.discardE.left, table);
            symID(e->val.discardE.ids, table);
            break;
        case callK:
            s = get_symbol_as(table, e->val.callE.name, functionSym);
            if(s == NULL)
                reportStrError("Function %s not defined", e->val.callE.name, e->lineno);
            symEXP(e->val.callE.args, table);
            break;
        case intconstK:
        case boolconstK:
        case stringconstK:
            break;
        case tupleconstK:
            symFIELDVALUE(e->val.tupleE.fieldvalues, table);
            break;
    }
}

void symFIELDVALUE(FIELDVALUE *fv, SymbolTable *table)
{
    if (fv == NULL)
        return;
    symFIELDVALUE(fv->next, table);
    symEXP(fv->exp, table);
}

void symID(ID *id, SymbolTable *table)
{
}

void symSESSION(SESSION *s)
{
    SYMBOL *sym;
    if(s == NULL)
        return;
    symSESSION(s->next);
    sym = get_symbol(mst, s->name);
    if(sym != NULL)
        reportStrError("session %s already declared", s->name, s->lineno);
    else
    {
        sym = put_symbol(mst, s->name, sessionSym);
        sym->val.sessionS = s;
    }

    symSTATEMENT(s->statements, mst);
}
