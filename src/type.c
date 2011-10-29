#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "error.h"
#include "symbol.h"
#include "type.h"

void initTypes();
int equalTYPE(EXP *s, EXP *t);
int verifyTupleEquivalence(EXP *s, EXP *t);
int hasFieldValue(FIELDVALUE *s, EXP *t)

TYPE *intTYPE, *boolTYPE, *stringTYPE;

void initTypes()
{
    intTYPE = makeTYPEint();
    boolTYPE = makeTYPEbool();
    stringTYPE = makeTYPEstring();
}

int equalTYPE(TYPE *s, TYPE *t)
{
    if (s->kind != t->kind) return 0;
    if (s->kind == tupleK)
    {
        if (strcmp(s->name, t->name) == 0)
            return 1;
        return compareSchemaStructuralEquivalence(s, t);
    }
    return 1;
}

int compareSchemaStructuralEquivalence(TYPE *s, TYPE *t)
{
    SCHEMA *schemaS = get_symbol(mst, s->name)->val.schemaS;
    SCHEMA *schemaT = get_symbol(mst, t->name)->val.schemaS;

    VARIABLE *currVariableS = schemaS->variables;
    while(currVariableS != NULL)
    {
        if(!hasVariableInSchema(currVariableS, schemaT))
            return 0;
        currVariableS = currVariableS->next;
    }
    return 1;
}

int hasVariableInSchema(VARIABLE *v, SCHEMA *s)
{
    VARIABLE *currV = s->variables;
    while (currV != NULL)
    {
        if (strcmp(v->name, currV->name) == 0)
        {
            if(v->type->kind == currV->type->kind)
                return 1;
            else
                return 0;
        }
        currV = currV->next;
    }
    return 0;
}

void checkBOOL(TYPE *t, int lineno)
{
    if(t->kind != boolK)
    {
        reportError("bool type expected", lineno);
    }
}

void checkINT(TYPE *t, int lineno)
{
    if(t->kind != intK)
    {
        reportError("int type expected", lineno);
    }
}

void typeSERVICE(SERVICE *s)
{
   typeFUNCTION(s->functions);
   typeSESSION(s->sessions);
}

//TODO check that returns return proper type
// need to pass symbol table to check identifiers (x+1 is int if x is int)
void typeFUNCTION(FUNCTION *f)
{
    if(f == NULL)
        return;
    typeFUNCTION(f->next);
    typeSTATEMENT(f->statements, f->returntype, NULL);
}

void typeSTATEMENT(STATEMENT *s, TYPE *returntype)
{
    if(s == NULL)
        return;
    
    switch(s->kind)
    {
        case skipK:
            break;
        case sequenceK:
            typeSTATEMENT(s->val.sequenceS.first, returntype);
            typeSTATEMENT(s->val.sequenceS.second, returntype);
        case showK:  //TODO always accept strings?
        case exitK:
            break;
        case returnK:
            if(returntype->kind != voidK)
            {
                if(!equalTYPE(s->val.returnS->type, returntype))
                {
                    reportError("wrong return type", s->lineno);
                }
            }
            break;
        case blockK:
            typeSTATEMENT(s->val.blockS.body, returntype);
            break;
        case ifK:
            checkBOOL(s->val.whileS.condition->type, s->lineno);
            typeSTATEMENT(s->val.ifS.body, returntype);
            break;
        case ifelseK:
            checkBOOL(s->val.whileS.condition->type, s->lineno);
            typeSTATEMENT(s->val.ifelseS.thenpart, returntype);
            typeSTATEMENT(s->val.ifelseS.elsepart, returntype);
            break;
        case whileK:
            checkBOOL(s->val.whileS.condition->type, s->lineno);
            typeSTATEMENT(s->val.whileS.body, returntype);
            break;
        case expK:
            typeEXP(s->val.expS);
            break;

    }
}

