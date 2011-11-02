#include "pretty.h"
#include "symbol.h"
#include "chash/chash.h"
#include <stdio.h>
#include <stdlib.h>

extern int print_symbols;
static int _indent = 0;
static int _spaces = 1;

void prettyHTML(HTML *);
void prettySCHEMA(SCHEMA *);
void prettyVARIABLE(VARIABLE *);
void prettyFUNCTION(FUNCTION *);
void prettySESSION(SESSION *);

void prettyHTMLBODY(HTMLBODY *);
void prettyATTRIBUTE(ATTRIBUTE *a);
void prettyTYPE(TYPE *t);
void prettyARGUMENT(ARGUMENT *a);
void prettySTATEMENT(STATEMENT *s);
void prettyDOCUMENT(DOCUMENT *d);
void prettyPLUG(PLUG *p);
void prettyEXP(EXP *e);
void prettyID(ID *id);
void prettyFIELDVALUE(FIELDVALUE *f);
void prettySYMBOL(void *i);
void prettySCOPE(SymbolTable *t);
void prettySymbolTable(SymbolTable *table);
void prettyRECEIVE(RECEIVE *r);
void prettyINPUT(INPUT *i);
void printTYPE(EXP *e);

static void print_indent(void)
{
    int i;
    for(i = 0; i < _indent; i++)
        putc(' ', stdout);
}

static void indent(void)
{
    _indent += 4;
}

static void dedent(void)
{
    _indent -= 4;
}

void prettySERVICE(SERVICE *service)
{
    printf("service\n{\n");
    indent();
    if (print_symbols)
        prettySCOPE(mst);
    prettyHTML(service->htmls);
    prettySCHEMA(service->schemas);
    prettyVARIABLE(service->variables);
    prettyFUNCTION(service->functions);
    prettySESSION(service->sessions);
    dedent();
    printf("}\n");
}

void prettyHTML(HTML *htmls)
{
    if(htmls == NULL)
        return;

    prettyHTML(htmls->next);
    print_indent();
    printf("const html %s = <html>", htmls->name);
    prettyHTMLBODY(htmls->htmlbodies);
    printf("</html>;\n\n");
}

void prettyHTMLBODY(HTMLBODY *h)
{
    if(h == NULL)
        return;

    prettyHTMLBODY(h->next);

    switch(h->kind)
    {
        case opentagK:
            printf("<%s%s", h->val.tagH.name, (h->val.tagH.attributes == NULL) ? "" : " ");
            prettyATTRIBUTE(h->val.tagH.attributes);
            printf(">");
            break;
        case closetagK:
            printf("</%s>", h->val.tagH.name);
            break;
        case selfclosingK:
            printf("<%s ", h->val.tagH.name);
            prettyATTRIBUTE(h->val.tagH.attributes);
            printf("/>");
        case gapK:
            printf("<[%s]>", h->val.gapH);
            break;
        case whateverK:
            printf("%s", h->val.whateverH);
            break;
        case metaK:
            printf("<!--%s-->", h->val.metaH);
            break;
        case inputK:
            printf("<input ");
            prettyATTRIBUTE(h->val.inputH.attributes);
            printf(">");
            break;
        case selectK:
            printf("<select ");
            prettyATTRIBUTE(h->val.selectH.attributes);
            printf(">");
            prettyHTMLBODY(h->val.selectH.htmlbodies);
            printf("</select>");
            break;
    }
}

void prettyATTRIBUTE(ATTRIBUTE *a)
{
    if(a == NULL)
        return;

    prettyATTRIBUTE(a->next);

    if (a->value == NULL)
        printf("%s ", a->name);
    else
        printf("%s=\"%s\" ", a->name, a->value);
}

void prettySCHEMA(SCHEMA *s)
{
    if(s == NULL)
        return;

    prettySCHEMA(s->next);

    print_indent();
    printf("schema %s {\n", s->name);
    indent();
    prettyVARIABLE(s->variables);
    dedent();
    print_indent();
    printf("}\n\n");
}

void prettyVARIABLE(VARIABLE *v)
{
    if(v == NULL)
        return;

    prettyVARIABLE(v->next);

    print_indent();
    prettyTYPE(v->type);
    printf(" %s;\n", v->name);
}

void prettyTYPE(TYPE *t)
{
    switch(t->kind)
    {
        case intK:
            printf("int");
            break;
        case boolK:
            printf("bool");
            break;
        case stringK:
            printf("string");
            break;
        case voidK:
            printf("void");
            break;
        case tupleK:
            printf("tuple %s", t->name);
            break;
    }
}           

void prettyFUNCTION(FUNCTION *f)
{
    if(f == NULL)
        return;

    prettyFUNCTION(f->next);

    print_indent();
    prettyTYPE(f->returntype);
    printf(" %s(", f->name);
    prettyARGUMENT(f->arguments);
    printf(")\n");
    prettySTATEMENT(f->statements);
    printf("\n");
}

void prettySESSION(SESSION *s)
{
    if (s == NULL)
        return;

    prettySESSION(s->next);
    print_indent();
    printf("session %s()\n", s->name);
    prettySTATEMENT(s->statements);
}

void prettyARGUMENT(ARGUMENT *a)
{
    if(a == NULL)
        return;

    prettyARGUMENT(a->next);

    if(a->next != NULL)
    {
        printf(", ");
    }

    
    prettyTYPE(a->type);
    printf(" %s", a->name);
}

void prettySTATEMENT(STATEMENT *s)
{
    if(s == NULL)
        return;

    if (s->kind != sequenceK)
        print_indent();

    switch(s->kind)
    {
        case skipK:
            printf(";\n");
            break;
        case sequenceK:
            prettySTATEMENT(s->val.sequenceS.first);
            prettySTATEMENT(s->val.sequenceS.second);
            break;
        case showK:
            printf("show ");
            prettyDOCUMENT(s->val.showS.document);
            prettyRECEIVE(s->val.showS.receives);
            printf(";\n");
            break;
        case exitK:
            printf("exit ");
            prettyDOCUMENT(s->val.exitS.document);
            printf(";\n");
            break;
        case returnK:
            printf("return");
            if (s->val.returnS != NULL)
                printf(" ");

            prettyEXP(s->val.returnS);
            printf(";\n");
            break;
        case blockK:
            _spaces = 1;
            printf("{\n");
            indent();
            if (print_symbols)
                prettySCOPE(s->val.blockS.table);
            prettyVARIABLE(s->val.blockS.variables);
            prettySTATEMENT(s->val.blockS.body);
            dedent();
            print_indent();
            printf("}\n");
            break;
        case ifK:
            printf("if (");
            prettyEXP(s->val.ifS.condition);
            printf(")\n");
            if (s->val.ifS.body->kind != blockK)
            {
                indent();
                prettySTATEMENT(s->val.ifS.body);
                dedent();
            } 
            else 
                prettySTATEMENT(s->val.ifS.body);
            
            break;
        case ifelseK:
            printf("if (");
            prettyEXP(s->val.ifelseS.condition);
            printf(")\n");
            
            if (s->val.ifelseS.thenpart->kind != blockK)
            {
                indent();
                prettySTATEMENT(s->val.ifelseS.thenpart);
                dedent();
            } 
            else 
            {
                prettySTATEMENT(s->val.ifelseS.thenpart);
                printf("\n");
            }

            print_indent();
            printf("else\n");
            
            if (s->val.ifelseS.elsepart->kind != blockK)
            {
                indent();
                prettySTATEMENT(s->val.ifelseS.elsepart);
                dedent();
            } 
            else 
                prettySTATEMENT(s->val.ifelseS.elsepart);
            break;
        case whileK:
            printf("while (");
            prettyEXP(s->val.whileS.condition);
            printf(")\n");
            if (s->val.whileS.body->kind != blockK)
            {
                indent();
                prettySTATEMENT(s->val.whileS.body);
                dedent();
            } 
            else 
                prettySTATEMENT(s->val.whileS.body);
            break;
        case expK:
            prettyEXP(s->val.expS);
            printf(";\n");
            break;
    }
}

void prettyDOCUMENT(DOCUMENT *d)
{
    if(d->plugs == NULL)
        printf("%s", d->name);
    else
    {
        printf("plug %s [", d->name);
        prettyPLUG(d->plugs);
        printf("]");
    }
}

void prettyPLUG(PLUG *p)
{
    if (p == NULL)
        return;

    prettyPLUG(p->next);
    if(p->next != NULL)
    {
        printf(", ");
    }

    printf("%s=", p->name);
    prettyEXP(p->exp);
}

void prettyEXP(EXP *e)
{
    char* operators[25];

    operators[orK] = "||";
    operators[andK] = "&&";
    operators[eqK] = "==";
    operators[ltK] = "<";
    operators[gtK] = ">";
    operators[leqK] = "<=";
    operators[geqK] = ">=";
    operators[neqK] = "!=";
    operators[plusK] = "+";
    operators[minusK] = "-";
    operators[timesK] = "*";
    operators[divK] = "/";
    operators[modK] = "%";
    operators[combineK] = "<<";

    if(e == NULL)
        return;

    if(e->next != NULL)
        printf(", ");

    prettyEXP(e->next);

    extern int print_types;
    if (print_types)
        printf("(");

    switch(e->kind)
    {
        case idK:
            printf("%s", e->val.idE.name);
            break;
        case idtupleK:
            printf("%s.%s", e->val.idtupleE.name, e->val.idtupleE.field);
            break;
        case assignK:
            printf("%s = ", e->val.assignE.left);
            prettyEXP(e->val.assignE.right);
            break;
        case assigntupleK:
            printf("%s.%s = ", e->val.assigntupleE.name, 
                e->val.assigntupleE.field);
            prettyEXP(e->val.assigntupleE.right);
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
            prettyEXP(e->val.binaryE.left);
            printf(" %s ", operators[e->kind]);
            prettyEXP(e->val.binaryE.right);
            break;
        case notK:
            printf("!");
            prettyEXP(e->val.unaryE);
            break;
        case uminusK:
            printf("-");
            prettyEXP(e->val.unaryE);
            break;
        case keepK:
            prettyEXP(e->val.keepE.left);
            printf(" \\+ (");
            prettyID(e->val.keepE.ids);
            printf(")");
            break;
        case discardK:
            prettyEXP(e->val.discardE.left);
            printf(" \\- (");
            prettyID(e->val.discardE.ids);
            printf(")");
            break;
        case callK:
            printf("%s(", e->val.callE.name);
            prettyEXP(e->val.callE.args);
            printf(")");
            break;
        case intconstK:
            printf("%d", e->val.intconstE);
            break;
        case boolconstK:
            printf((e->val.boolconstE) ? "true" : "false");
            break;
        case stringconstK:
            printf("\"%s\"", e->val.stringconstE);
            break;
        case tupleconstK:
            printf("tuple {");
            prettyFIELDVALUE(e->val.tupleE.fieldvalues);
            printf("}");
            break;
    }


    if (print_types)
    {
        printf(" : ");
        prettyTYPE(e->type);
        printf(")");
    }
}

void prettyID(ID *id)
{
    if (id == NULL)
        return;

    prettyID(id->next);
    if (id->next != NULL)
        printf(", ");

    printf("%s", id->name);
}

void prettyFIELDVALUE(FIELDVALUE *f)
{
    if (f == NULL)
        return;

    prettyFIELDVALUE(f->next);
    if (f->next != NULL)
        printf(", ");

    printf("%s=", f->name);
    prettyEXP(f->exp);
}

void prettyRECEIVE(RECEIVE *r)
{
    if (r == NULL)
        return;

    printf(" receive [");
    prettyINPUT(r->inputs);
    printf("]");
}

void prettyINPUT(INPUT *i)
{
    if (i == NULL)
        return;

    prettyINPUT(i->next);
    if (i->next != NULL)
        printf(", ");
    
    printf("%s=%s", i->lhs, i->rhs);
}

void prettySYMBOL(void *i)
{
    SYMBOL *s;
    s = (SYMBOL*)i;

    switch(s->kind)
    {
        case htmlSym:
            printf("html");
            break;
        case variableSym:
            prettyTYPE(s->val.variableS->type);
            printf(" variable");
            break;
        case sessionSym:
            printf("session");
            break;
        case functionSym:
            printf("function returning ");
            prettyTYPE(s->val.functionS->returntype);
            break;
        case argumentSym:
            prettyTYPE(s->val.argumentS->type);
            printf(" argument");
            break;            
        case schemaSym:
            printf("schema");
            break;
    }
}

void prettySymbolTable(SymbolTable *table)
{
    if(table == NULL)
        return;
    prettySymbolTable(table->next);
    chash_pretty_print(table->table, prettySYMBOL, _indent + _spaces);
    _spaces += 4;
}

void prettySCOPE(SymbolTable *table)
{
    if(table == NULL)
        return;
    print_indent();
    printf("=================================================\n");
    print_indent();
    printf("=============");
    printf("|Symbols in this Scope|");
    printf("=============\n");
    print_indent();
    printf("=================================================\n");
    prettySymbolTable(table);
    print_indent();
    printf("=================================================\n\n");
}
