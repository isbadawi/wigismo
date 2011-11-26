#include "tree.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

extern char *infile; 

int weedVARIABLE_TYPE(VARIABLE *);
int weedSESSION(SESSION *);
int weedSESSION_RETURN(STATEMENT *);
int weedFUNCTION(FUNCTION *);
int weedSCHEMA(SCHEMA *);
int weedHTML(HTML *);
int weedHTMLBODY(HTMLBODY *);
int weedFUNCTION_ARGUMENTS(FUNCTION *);
int weedARGUMENT_TYPE(ARGUMENT *);
int weedSESSION_END(SESSION *);
int weedSTATEMENTS_END(STATEMENT *);
int weedATTRIBUTE_NAME(ATTRIBUTE *);
int weedINPUT_TYPE_ATTR(ATTRIBUTE *);
int weedFUNCTION_RETURN_TYPE(FUNCTION *);
int weedRETURN_TYPE_VOID(STATEMENT *);
int weedRETURN_TYPE_NE(STATEMENT *);

int weedSERVICE(SERVICE *s)
{
    if (!weedSESSION(s->sessions))
        return 0;

    if (!weedHTML(s->htmls))
        return 0;

    if (!weedFUNCTION(s->functions))
        return 0;

    if (!weedSCHEMA(s->schemas))
        return 0;

    if (!weedVARIABLE_TYPE(s->variables))
        return 0;

    return 1;
}

int weedSCHEMA(SCHEMA *s)
{
    if (s == NULL)
        return 1;

    if (!weedVARIABLE_TYPE(s->variables))
        return 0;

    return weedSCHEMA(s->next);
}

int weedSESSION(SESSION *s)
{
    if (s == NULL)
        return 1;

    if (!weedSESSION_END(s))
        return 0;

    if (!weedVARIABLE_TYPE(s->statements->val.blockS.variables))
        return 0;

    if (!weedSESSION_RETURN(s->statements))
        return 0;

    return weedSESSION(s->next);
}

int weedSESSION_RETURN(STATEMENT *s) 
{
    if (s == NULL)
        return 1;

    switch(s->kind)
    {
        case sequenceK:
            if (!(weedSESSION_RETURN(s->val.sequenceS.first) &&
                  weedSESSION_RETURN(s->val.sequenceS.second)))
                return 0;
            break;
        case returnK:
            fprintf(stderr, "%s:%d return statements not allowed in sessions\n", infile, s->lineno);
            return 0;
        case ifK:
            if (!weedSESSION_RETURN(s->val.ifS.body))
                return 0;
            break;
        case ifelseK:
            if (!(weedSESSION_RETURN(s->val.ifelseS.thenpart) &&
                  weedSESSION_RETURN(s->val.ifelseS.elsepart)))
                return 0;
            break;
        case blockK:
            if (!weedSESSION_RETURN(s->val.blockS.body))
                return 0;
            break;
        case whileK:
            if (!weedSESSION_RETURN(s->val.whileS.body))
                return 0;
            break;
        default:
            return 1;
    }
    return 1;
}

int weedHTML(HTML *h) 
{
    if (h == NULL)
        return 1;

    if (!weedHTMLBODY(h->htmlbodies))
        return 0;

    return weedHTML(h->next);
}

int weedHTMLBODY(HTMLBODY *hb)
{
    if (hb == NULL)
        return 1;

    if (hb->kind == inputK)
    {
        if (!weedATTRIBUTE_NAME(hb->val.inputH.attributes))
        {
            fprintf(stderr, "%s:%d input tag missing name attribute\n", infile, hb->lineno);
            return 0;
        }
        if (!weedINPUT_TYPE_ATTR(hb->val.inputH.attributes))
        {

            fprintf(stderr, "%s:%d input tag missing type attribute\n", infile, hb->lineno);
            return 0;
        }
    }
    if (hb->kind == selectK)
    {
        if (!weedATTRIBUTE_NAME(hb->val.selectH.attributes))
        {
            fprintf(stderr, "%s:%d select tag missing name attribute\n", infile, hb->lineno);
            return 0;
        }
    }

    return weedHTMLBODY(hb->next);
}

int weedFUNCTION(FUNCTION *f)
{
    if (f == NULL)
        return 1;

    if (!weedFUNCTION_SHOW_EXIT(f->statements))
        return 0;
    if (!weedFUNCTION_RETURN_TYPE(f))
        return 0;
    if (!weedFUNCTION_ARGUMENTS(f))
        return 0;
    if (!weedVARIABLE_TYPE(f->statements->val.blockS.variables))
        return 0;
    return 1;
}

int weedFUNCTION_SHOW_EXIT(STATEMENT *s)
{
    if (s == NULL)
        return 1;
    switch(s->kind)
    {
        case skipK:
            return 1;
        case sequenceK:
            return weedFUNCTION_SHOW_EXIT(s->val.sequenceS.first) || 
                   weedFUNCTION_SHOW_EXIT(s->val.sequenceS.second);         
        case showK:
        case exitK:
            fprintf(stderr, "%s:%d show and exit statements not allowed in functions.\n", infile, s->lineno);
            return 0;
        case returnK:
            return 1;
        case blockK:
            return weedFUNCTION_SHOW_EXIT(s->val.blockS.body);
        case ifK:
            return weedFUNCTION_SHOW_EXIT(s->val.ifS.body);            
        case ifelseK:
            return weedFUNCTION_SHOW_EXIT(s->val.ifelseS.thenpart) ||
                   weedFUNCTION_SHOW_EXIT(s->val.ifelseS.elsepart);         
        case whileK:
            return weedFUNCTION_SHOW_EXIT(s->val.whileS.body);
        case expK:
            return 1;
    }
}

int weedVARIABLE_TYPE(VARIABLE *v)
{
    if (v == NULL)
        return 1;

    if (v->type->kind == voidK)
    {
        fprintf(stderr, "%s:%d invalid type void for variable %s.\n", infile, v->lineno, v->name);
        return 0;
    }

    return weedVARIABLE_TYPE(v->next);
}

int weedFUNCTION_ARGUMENTS(FUNCTION *f) 
{
    if (f == NULL)
        return 1;

    if (!weedARGUMENT_TYPE(f->arguments))
        return 0;

    return weedFUNCTION_ARGUMENTS(f->next);
}

int weedARGUMENT_TYPE(ARGUMENT *a)
{
    if (a == NULL)
        return 1;

    if (a->type->kind == voidK)
    {
        fprintf(stderr, "%s:%d invalid type void for argument %s.\n", infile, a->lineno, a->name);
        return 0;
    }

    return weedARGUMENT_TYPE(a->next);
}

int weedSESSION_END(SESSION *s)
{
    if (s == NULL)
        return 1;

    if (!weedSTATEMENTS_END(s->statements))
    {
        fprintf(stderr, "%s missing exit statement for session %s\n", infile, s->name);
        return 0;
    }

    return weedSESSION_END(s->next);
}

int weedSTATEMENTS_END(STATEMENT *s)
{
    if (s == NULL)
        return 0;
    if (s->kind == exitK)
        return 1;
    else if (s->kind == blockK)
        return weedSTATEMENTS_END(s->val.blockS.body);
    else if (s->kind == sequenceK)
        return weedSTATEMENTS_END(s->val.sequenceS.second);
    else if (s->kind == ifelseK)
        return weedSTATEMENTS_END(s->val.ifelseS.thenpart) &&
               weedSTATEMENTS_END(s->val.ifelseS.elsepart);
    return 0;
}

int weedATTRIBUTE_NAME(ATTRIBUTE *a)
{
    if (a == NULL)
        return 0;

    if (!strcmp(a->name, "name"))
        return 1;

    return weedATTRIBUTE_NAME(a->next);
}

int weedINPUT_TYPE_ATTR(ATTRIBUTE *a)
{
    if (a == NULL)
        return 0;

    if (!strcmp(a->name, "type"))
        return 1;

    return weedINPUT_TYPE_ATTR(a->next);
}

int weedFUNCTION_RETURN_TYPE(FUNCTION *f)
{
    if (f == NULL)
        return 1;

    if (f->returntype->kind == voidK)
        return weedRETURN_TYPE_VOID(f->statements->val.blockS.body);
    else
    {
        if (!weedRETURN_TYPE_NE(f->statements->val.blockS.body))
        {
            fprintf(stderr, "%s missing return in non void function %s\n", infile, f->name);
            return 0;
        }
        return 1;
    }

    return weedFUNCTION_RETURN_TYPE(f->next);
}

int weedRETURN_TYPE_VOID(STATEMENT *s)
{
    if (s == NULL)
        return 1;

    if (s->kind == returnK && s->val.returnS != NULL)
    {
        fprintf(stderr, "%s:%d return statement in void function\n", infile, s->lineno);
        return 0;
    }

    if (s->kind == blockK)
        return weedRETURN_TYPE_VOID(s->val.blockS.body);
    if (s->kind == sequenceK)
        return weedRETURN_TYPE_VOID(s->val.sequenceS.first) &&
               weedRETURN_TYPE_VOID(s->val.sequenceS.second);
    return 1;
}

int weedRETURN_TYPE_NE(STATEMENT *s)
{
    if (s == NULL)
        return 0;

    if (s->kind == returnK && s->val.returnS != NULL)
        return 1;
     
    if (s->kind == blockK)
        return weedRETURN_TYPE_NE(s->val.blockS.body);
    if (s->kind == sequenceK)
        return weedRETURN_TYPE_NE(s->val.sequenceS.second);
    if (s->kind == ifelseK)
        return weedRETURN_TYPE_NE(s->val.ifelseS.thenpart) &&
               weedRETURN_TYPE_NE(s->val.ifelseS.elsepart);

    return 0;
}        
