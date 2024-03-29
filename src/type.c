#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "error.h"
#include "symbol.h"
#include "type.h"

void initTypes();
int equalTYPE(TYPE *s, TYPE *t);
int compareSchemaStructuralEquivalence(TYPE *s, TYPE *t);
int hasVariableInSchema(VARIABLE *v, SCHEMA *s);
void checkBOOL(TYPE *t, int lineno);
void checkINT(TYPE *t, int lineno);
int verifyTupleEquivalence(EXP *s, EXP *t);
int hasFieldValue(FIELDVALUE *s, EXP *t);
void typeFUNCTION(FUNCTION *);
void typeSESSION(SESSION *);
void typeSTATEMENT(STATEMENT *, TYPE *);
void typeEXP(EXP *);
void typeFIELDVALUE(FIELDVALUE *);
int verifyIdsInTuple(EXP *exp, ID *ids);
int tupleContainsId(ID *id, SCHEMA *s);
int verifyTupleCombine(EXP *left, EXP *right);
int compatibleTuples(VARIABLE *l, SCHEMA *r);
TYPE *typeVar(SYMBOL *s);


TYPE *intTYPE, *boolTYPE, *stringTYPE;

void initTypes(void)
{
    intTYPE = makeTYPEint();
    boolTYPE = makeTYPEbool();
    stringTYPE = makeTYPEstring();
}

int equalTYPE(TYPE *s, TYPE *t)
{
    if (s == NULL || t == NULL)
        return 0;
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
        reportError("bool type expected", lineno);
}

void checkINT(TYPE *t, int lineno)  
{
    if(t->kind != intK)
        reportError("int type expected", lineno);
}

void typeSERVICE(SERVICE *s)
{
   initTypes();
   typeFUNCTION(s->functions);
   typeSESSION(s->sessions);
}

void typeFUNCTION(FUNCTION *f)
{
    if(f == NULL)
        return;
    typeFUNCTION(f->next);
    typeSTATEMENT(f->statements, f->returntype);
}

void typeINPUT(INPUT *i)
{
    if (i == NULL)
        return;
    typeINPUT(i->next);
    if (strchr(i->lhs, '.') != NULL)
        return;
    if (typeVar(i->leftsym)->kind == tupleK)
        reportError("tuples cannot be received", i->lineno);
}

void typeRECEIVE(RECEIVE *r)
{
    if (r == NULL)
        return;
    typeRECEIVE(r->next);
    typeINPUT(r->inputs);
}

void typePLUG(PLUG *p)
{
    if (p == NULL)
        return;
    typePLUG(p->next);
    typeEXP(p->exp);
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
            break;
        case showK: 
            typePLUG(s->val.showS.document->plugs);
            typeRECEIVE(s->val.showS.receives);
            break;
        case exitK:
            typePLUG(s->val.exitS.document->plugs);
            break;
        case returnK:
            if (s->val.returnS == NULL)
                break;
            typeEXP(s->val.returnS);
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
            typeEXP(s->val.ifS.condition);
            checkBOOL(s->val.ifS.condition->type, s->lineno);
            typeSTATEMENT(s->val.ifS.body, returntype);
            break;
        case ifelseK:
            typeEXP(s->val.ifelseS.condition);
            checkBOOL(s->val.ifelseS.condition->type, s->lineno);
            typeSTATEMENT(s->val.ifelseS.thenpart, returntype);
            typeSTATEMENT(s->val.ifelseS.elsepart, returntype);
            break;
        case whileK:
            typeEXP(s->val.whileS.condition);
            checkBOOL(s->val.whileS.condition->type, s->lineno);
            typeSTATEMENT(s->val.whileS.body, returntype);
            break;
        case expK:
            typeEXP(s->val.expS);
            break;
    }
}

TYPE* typeVar(SYMBOL *s)
{
    if (s->kind == variableSym)
        return s->val.variableS->type;
    else if (s->kind == argumentSym)
        return s->val.argumentS->type;
    return NULL;
}

TYPE* typeSchemaVar(SCHEMA *s, char *var)
{
    VARIABLE *v;
    for (v = s->variables; v != NULL; v = v->next)
        if (!strcmp(v->name, var))
            return v->type;
    return NULL;
}

VARIABLE *variable_from_fieldvalue(FIELDVALUE *fv)
{
    VARIABLE *v;
    if (fv == NULL)
        return NULL;
    v = makeVARIABLE(fv->exp->type, fv->name);
    v->next = variable_from_fieldvalue(fv->next);
    return v;
}

char *generate_schema_name(void)
{
    static int calls = 0;
    char *schema_name = (char*)malloc(15);
    /* identifiers can't start with a number, so this won't conflict with anything */
    sprintf(schema_name, "<%d>", calls++);
    return schema_name;
}

SCHEMA *generate_schema(FIELDVALUE *fv)
{
    char *name = generate_schema_name();
    return makeSCHEMA(name, variable_from_fieldvalue(fv));
}                                    
 
VARIABLE *vars_from_ids(SCHEMA *s, ID *id)
{
    VARIABLE *v;
    if (id == NULL)
        return NULL;
    v = makeVARIABLE(typeSchemaVar(s, id->name), id->name);
    v->next = vars_from_ids(s, id->next);
    return v;
}    

SCHEMA *make_keep_result(EXP *e)
{
    SCHEMA *schema = get_symbol(mst, e->val.keepE.left->type->name)->val.schemaS;
    ID *id = e->val.keepE.ids;
    return makeSCHEMA(generate_schema_name(), vars_from_ids(schema, id));
}
 
VARIABLE *copyVARIABLES(VARIABLE *vars)
{
    VARIABLE *v;
    if (vars == NULL)
        return vars;
    v = makeVARIABLE(vars->type, vars->name);
    v->next = copyVARIABLES(vars->next);
    return v;
} 

int id_in(char *id, ID *ids) {
    if (ids == NULL)
        return 0;
    if (!strcmp(id, ids->name))
        return 1;
    return id_in(id, ids->next);
}

SCHEMA *make_discard_result(EXP *e)
{
    SCHEMA *schema = get_symbol(mst, e->val.discardE.left->type->name)->val.schemaS;
    ID *ids = e->val.discardE.ids;
    VARIABLE *rootV = copyVARIABLES(schema->variables);
    VARIABLE *prev = rootV;
    VARIABLE *v;
    while (rootV != NULL && id_in(rootV->name, ids))
        rootV = rootV->next;
    if (rootV == NULL)
        return makeSCHEMA(generate_schema_name(), rootV);
    for (v = rootV->next; v != NULL; v = v->next)
    {
        if (id_in(v->name, ids))
            prev->next = v->next;
        else
            prev = v;
    }
    return makeSCHEMA(generate_schema_name(), rootV);
}

int var_in(VARIABLE *v, VARIABLE *vars)
{
    if (vars == NULL)
        return 0;
    if (!strcmp(v->name, vars->name))
        return 1;
    return var_in(v, vars->next);
}

SCHEMA *make_combine_result(EXP *e)
{
    SCHEMA *left = get_symbol(mst, e->val.binaryE.left->type->name)->val.schemaS;
    SCHEMA *right = get_symbol(mst, e->val.binaryE.right->type->name)->val.schemaS;
    /* all the variables in left */
    VARIABLE *vars = copyVARIABLES(left->variables);
    VARIABLE *v;
    for (v = right->variables; v != NULL; v = v->next)
    {
        if (!var_in(v, vars))
        {
            VARIABLE *temp = vars;
            vars = makeVARIABLE(v->type, v->name);
            vars->next = temp;
        }
    }
    return makeSCHEMA(generate_schema_name(), vars);
}

int checkARGUMENTS(ARGUMENT *arguments, EXP *exps)
{
    if (arguments == NULL && exps == NULL)
        return 1;
    if (arguments == NULL && exps != NULL)
        return 0;
    if (arguments != NULL && exps == NULL)
        return 0;
    if (!equalTYPE(arguments->type, exps->type))
        return 0;
    return checkARGUMENTS(arguments->next, exps->next);
}

void typeEXP(EXP *exp)
{
    SCHEMA *schema;
    SYMBOL *s;
    if (exp == NULL)
        return;
    typeEXP(exp->next);
    switch (exp->kind)
    {
        case idK:
            exp->type = typeVar(exp->val.idE.idsym);
            break;
        case idtupleK:
            exp->type = typeSchemaVar(exp->val.idtupleE.schema, exp->val.idtupleE.field);
            break;
        case assignK:
            typeEXP(exp->val.assignE.right);
            if (!equalTYPE(typeVar(exp->val.assignE.leftsym), exp->val.assignE.right->type))
                reportError("invalid assignment", exp->lineno);
            exp->type = exp->val.assignE.right->type;
            break;
        case assigntupleK:
            typeEXP(exp->val.assigntupleE.right);
            if (!equalTYPE(typeSchemaVar(exp->val.assigntupleE.schema, exp->val.assigntupleE.field), 
                           exp->val.assigntupleE.right->type))
                reportError("invalid assignment", exp->lineno);
            exp->type = exp->val.assigntupleE.right->type;
            break;
        case orK:
        case andK:
            typeEXP(exp->val.binaryE.left);
            typeEXP(exp->val.binaryE.right);
            checkBOOL(exp->val.binaryE.left->type, exp->lineno);
            checkBOOL(exp->val.binaryE.right->type, exp->lineno);
            exp->type = boolTYPE;
            break;
        case ltK:
        case gtK:
        case leqK:
        case geqK:
        {
            TYPE *left;
            TYPE *right;
            typeEXP(exp->val.binaryE.left);
            typeEXP(exp->val.binaryE.right);
            left = exp->val.binaryE.left->type;
            right = exp->val.binaryE.right->type;
            if (!((left->kind == intK && right->kind == intK) ||
                  (left->kind == stringK && right->kind == stringK)))
                reportError("non-comparable types", exp->lineno);
            exp->type = boolTYPE;
            break;
        }
        case eqK:
        case neqK:
            typeEXP(exp->val.binaryE.left);
            typeEXP(exp->val.binaryE.right);
            if (!equalTYPE(exp->val.binaryE.left->type, exp->val.binaryE.right->type))
                reportError("non-comparable types", exp->lineno);
            exp->type = boolTYPE;
            break;
        case plusK:
        {
            TYPE *left;
            TYPE *right;
            typeEXP(exp->val.binaryE.left);
            typeEXP(exp->val.binaryE.right);
            left = exp->val.binaryE.left->type;
            right = exp->val.binaryE.right->type; 
            if (left->kind == intK && right->kind == intK)
                exp->type = intTYPE;
            else if (left->kind == stringK && right->kind == stringK)
                exp->type = stringTYPE;
            else if (left->kind == intK && right->kind == stringK)
                exp->type = stringTYPE;
            else if (left->kind == stringK && right->kind == intK)
                exp->type = stringTYPE;
            else
            {
                reportError("invalid types for +", exp->lineno);
                exp->type = intTYPE;
            }
            break;
        }
        case minusK:
        case timesK:
        case divK:
        case modK:
            typeEXP(exp->val.binaryE.left);
            typeEXP(exp->val.binaryE.right);
            checkINT(exp->val.binaryE.left->type, exp->lineno);
            checkINT(exp->val.binaryE.right->type, exp->lineno);
            exp->type = intTYPE;
            break;
        case uminusK:
            typeEXP(exp->val.unaryE);
            checkINT(exp->val.unaryE->type, exp->lineno);
            exp->type = intTYPE;
            break;
        case combineK:
        {
            TYPE *left;
            TYPE *right;
            typeEXP(exp->val.binaryE.left);
            typeEXP(exp->val.binaryE.right);
            left = exp->val.binaryE.left->type;
            right = exp->val.binaryE.right->type; 
            if (left->kind == tupleK && right->kind == tupleK)
            {
                if (verifyTupleCombine(exp->val.binaryE.left, exp->val.binaryE.right))
                {
                    schema = make_combine_result(exp);
                    exp->type = makeTYPEtuple(schema->name);
                    s = put_symbol(mst, schema->name, schemaSym);
                    s->val.schemaS = schema;
                }
                else
                    reportError("incompatible tuples", exp->lineno);
            }
            else
                reportError("invalid operand types for <<", exp->lineno);
            break;
        }
        case keepK:
            typeEXP(exp->val.keepE.left);
            if(exp->val.keepE.left->type->kind == tupleK)
            {
                if(verifyIdsInTuple(exp->val.keepE.left, exp->val.keepE.ids))
                {
                    schema = make_keep_result(exp);
                    exp->type = makeTYPEtuple(schema->name);
                    s = put_symbol(mst, schema->name, schemaSym);
                    s->val.schemaS = schema;
                }                        
                else
                    reportError("invalid ids for \\+", exp->lineno);
            }
            else
                reportError("invalid left operand for \\+", exp->lineno);
            break;
        case discardK:
            typeEXP(exp->val.discardE.left);
            if(exp->val.discardE.left->type->kind == tupleK)
            {
                if (verifyIdsInTuple(exp->val.discardE.left, exp->val.discardE.ids))
                {
                    schema = make_discard_result(exp);
                    exp->type = makeTYPEtuple(schema->name);
                    s = put_symbol(mst, schema->name, schemaSym);
                    s->val.schemaS = schema;
                }
                else
                    reportError("invalid ids for \\-", exp->lineno);
            }
            else
                reportError("invalid left operand for \\-", exp->lineno);
            break;
        case notK:
            typeEXP(exp->val.unaryE);
            checkBOOL(exp->val.unaryE->type, exp->lineno);
            exp->type = boolTYPE;
            break;
        case callK:
        {
            FUNCTION *f = get_symbol(mst, exp->val.callE.name)->val.functionS;
            typeEXP(exp->val.callE.args);
            if (!checkARGUMENTS(f->arguments, exp->val.callE.args))
                reportStrError("wrong signature for %s", f->name, exp->lineno);
            exp->type = f->returntype;
            break;
        }
        case intconstK:
            exp->type = intTYPE;
            break;
        case boolconstK:
            exp->type = boolTYPE;
            break;
        case stringconstK:
            exp->type = stringTYPE;
            break;
        case tupleconstK:
            typeFIELDVALUE(exp->val.tupleE.fieldvalues);
            schema = generate_schema(exp->val.tupleE.fieldvalues);
            exp->type = makeTYPEtuple(schema->name);
            s = put_symbol(mst, schema->name, schemaSym);
            s->val.schemaS = schema;
            break;
    }
}

int verifyIdsInTuple(EXP *exp, ID *ids)
{
    SCHEMA *schema = get_symbol(mst, exp->type->name)->val.schemaS;
    ID *currID = ids;
    
    while (currID != NULL)
    {
        if (!tupleContainsId(currID, schema))
            return 0;
        currID = currID->next;
    }
    return 1;
}

int tupleContainsId(ID *id, SCHEMA *s)
{
    VARIABLE *currV = s->variables;
    
    while (currV != NULL)
    {
        if (strcmp(id->name, currV->name) == 0)
            return 1;
        currV = currV->next;
    }
    return 0;
}

int verifyTupleCombine(EXP *left, EXP *right)
{
    SCHEMA *schemaL = get_symbol(mst, left->type->name)->val.schemaS;
    SCHEMA *schemaR = get_symbol(mst, right->type->name)->val.schemaS;
    VARIABLE *currLeftV = schemaL->variables;                      
   
    while(currLeftV != NULL)
    {
        if(!compatibleTuples(currLeftV, schemaR))
            return 0;
        currLeftV = currLeftV->next;
    }
    return 1;
}

int compatibleTuples(VARIABLE *l, SCHEMA *r)
{
    VARIABLE *currRightV = r->variables;

    while(currRightV != NULL)
    {
        if(!strcmp(l->name, currRightV->name))
            if(l->type->kind != currRightV->type->kind)
                return 0;
        currRightV = currRightV->next;
    }
    return 1;
}

void typeSESSION(SESSION *s)
{
    if (s == NULL)
        return;
    typeSESSION(s->next);
    typeSTATEMENT(s->statements, NULL);
}

void typeFIELDVALUE(FIELDVALUE *fv)
{
    if (fv == NULL)
        return;
    typeFIELDVALUE(fv->next);
    typeEXP(fv->exp);
}
