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

int equalTYPE(EXP *s, EXP *t)
{
    if (s->type->kind != t->type->kind) return 0;
    if (s->type->kind == tupleK)
    {
        if (s->type->name != NULL && t->type->name != NULL) //TODO set/tupl/exp/name/null
        {
            if (strcmp(s->type->name, t->type->name) == 0)
                return 1;
            else
            {
                return compareSchemaStructuralEquivalence(s, t);
            }
        }
        else if (s->type->name == NULL && t->type->name != NULL)
        {

        }
        else if (s->type->name != NULL && t->type->name == NULL)
        {
        }
        else
            return verifyTupleEquivalence(s, t);
    }
    return 1;
}

int compareSchemaToTuple(EXP *s, EXP *t)
{
    SCHEMA *schema = get_symbol(mst, s->type->name)->val.schemaS;

    VARIABLE *currV = schema->variables;
    while (currV != NULL)
    {
        if(!has
        currV = currV->next;
    }

}

int compareSchemaStructuralEquivalence(EXP *s, EXP *t)
{
    SCHEMA *schemaS = get_symbol(mst, s->type->name)->val.schemaS;
    SCHEMA *schemaT = get_symbol(mst, t->type->name)->val.schemaS;

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
    }
    return 0;
}

int verifyTupleEquivalence(EXP *s, EXP *t)
{
    if (s->type->kind != tupleK || t->type->kind != tupleK)
        return 0;

    FIELDVALUE * currFieldValueS = s->val.tupleE.fieldValues;
    while (currFieldValueS != NULL)
    {
        if (!hasFieldValue(currFieldValueS, t))
            return 0;
        currFieldValueS = currFieldValue->next;
    }
    return 1;
}

//TODO fill in types of fieldvalues in tuples

int hasFieldValue(FIELDVALUE *s, EXP *t)
{
    FIELDVALUE * currFieldValueT = t->val.tupleE.fieldValues;
    while(currFiledValueValueT != NULL)
    {
        if(strcmp(s->name, currFieldValueT->name)==0)
        {
            if(s->exp->type->kind == currFieldValueT->exp->type->kind)
                return 1;
            else
                return 0;
        }
        currFieldValueT = currFieldValueT->next;
    }
    return 0;
}

int checkBOOL(TYPE *t, int lineno)
{
    if(t->kinf != boolK)
    {
        reportError("char type expected", lineno); //TODO reportError
        return 0;
    }
    return 1;
}

int checkINT(TYPE *t, int lineno)
{
    if(t->kind != intK)
    {
        reportError("int type expected", lineno);
        return 0;
    }
    return 1;
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

void typeSTATEMENT(STATEMENT *s, TYPE *returntype, SymbolTable *table)
{
    if(s == NULL)
        return;
    typeSTATEMENT(s->next, returntype, table);
    
    switch(s->kind)
    {
        case skipK:
            break;
        case sequenceK:
            typeSTATEMENT(s->val.sequenceS.first, returntype, table);
            typeSTATEMENT(s->val.sequenceS.second, returntype, table);
        case showK:  //TODO always accept strings?
        case exitK:
            break;
        case returnK:
            if(s->val.returnS != NULL)

            if(s->val.returnS->type->kind == )
            {
                if(returnt)
            }
        case blockK:
        case whileK:
        case expK:
    }
}

