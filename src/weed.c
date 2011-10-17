#include "tree.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

extern char *infile; 

int weedVARIABLE_TYPE(VARIABLE *);
int weedFUNCTION(FUNCTION *);
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
    if (!weedSESSION_END(s->sessions))
        return 0;

    if (!weedHTML(s->htmls))
        return 0;

    if (!weedFUNCTION(s->functions))
        return 0;

    if (!weedVARIABLE_TYPE(s->variables))
        return 0;

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
    if (!weedFUNCTION_RETURN_TYPE(f))
        return 0;
    if (!weedFUNCTION_ARGUMENTS(f))
        return 0;
    return 1;
}

int weedVARIABLE_TYPE(VARIABLE *v)
{
    if (v == NULL)
        return 1;

    if (v->type->kind == voidK)
    {
        printf("invalid type void for variable %s.\n", v->name);
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
        printf("invalid type void for argument %s.\n", a->name);
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
        fprintf(stderr, "missing exit statement for session %s\n", s->name);
        return 0;
    }

    return weedSESSION_END(s->next);
}

int weedSTATEMENTS_END(STATEMENT *s)
{
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
        return weedRETURN_TYPE_NE(f->statements->val.blockS.body);

    return weedFUNCTION_RETURN_TYPE(f->next);
}

int weedRETURN_TYPE_VOID(STATEMENT *s)
{
    if (s == NULL)
        return 1;

    if (s->kind == returnK && s->val.returnS != NULL)
    {
        fprintf(stderr, "void function returns something\n");
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
    {
        fprintf(stderr, "non void function returns something\n");
        return 0;
    }

    if (s->kind == returnK && s->val.returnS != NULL)
        return 1;
     
    if (s->kind == blockK)
        return weedRETURN_TYPE_NE(s->val.blockS.body);
    if (s->kind == sequenceK)
        return weedRETURN_TYPE_NE(s->val.sequenceS.second);
    if (s->kind == ifelseK)
        return weedRETURN_TYPE_NE(s->val.ifelseS.thenpart) &&
               weedRETURN_TYPE_NE(s->val.ifelseS.elsepart);

    fprintf(stderr, "non void function returns something\n");
    return 0;
}        
