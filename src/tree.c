#include"tree.h"
#include"memory.h"
#include<stdlib.h>

extern int lineno;

SERVICE *makeSERVICE(char *name, HTML *htmls, SCHEMA *schemas, 
                     VARIABLE *variables, FUNCTION *functions,
                     SESSION *sessions)
{
    SERVICE *s = NEW(SERVICE);
    p->name = name;
    p->htmls = htmls;
    p->schemas = schemas;
    p->variables = variables;
    p->functions = functions;
    p->sessions = sessions;
    return s;
}

HTML *makeHTML(char *name, HTMLBODY *htmlbodies)
{
    HTML *h = NEW(HTML);
    h->name = name;
    h->htmlbodies = htmlbodies;
    h->next = NULL;
    return h;
}

HTMLBODY *makeHTMLBODYopen(char *name, ATTRIBUTE *attributes)
{
    HTMLBODY *h = NEW(HTMLBODY);
    h->lineno = lineno;
    h->kind = opentagK;
    h->val.tagH.name = name;
    h->val.tagH.attributes = attributes;
    h->next = NULL;
    return h;
}

HTMLBODY *makeHTMLBODYclose(char *name)
{
    HTMLBODY *h = NEW(HTMLBODY);
    h->lineno = lineno;
    h->kind = closetagK;
    h->val.tagH.name = name;
    h->val.tagH.attributes = NULL;
    h->next = NULL;
    return h;
}

HTMLBODY *makeHTMLBODYgap(char *name)
{
    HTMLBODY *h = NEW(HTMLBODY);
    h->lineno = lineno;
    h->kind = gapK;
    h->val.gapH = name;
    h->next = NULL;
    return h;
}

HTMLBODY *makeHTMLBODYwhatever(char *whatever)
{
    HTMLBODY *h = NEW(HTMLBODY);
    h->lineno = lineno;
    h->kind = whateverK;
    h->val.whateverH = whatever;
    h->next = NULL;
    return h;
}

HTMLBODY *makeHTMLBODYmeta(char *meta)
{
    HTMLBODY *h = NEW(HTMLBODY);
    h->lineno = lineno;
    h->kind = metaK;
    h->val.metaH = meta;
    h->next = NULL;
    return h;
}

HTMLBODY *makeHTMLBODYinput(ATTRIBUTE *attributes)
{
    HTMLBODY *h = NEW(HTMLBODY);
    h->lineno = lineno;
    h->kind = inputK;
    h->val.inputH.attributes = attributes;
    h->next = NULL;
    return h;
}

HTMLBODY *makeHTMLBODYselect(ATTRIBUTE *attributes, HTMLBODY *htmlbodies)
{
    HTMLBODY *h = NEW(HTMLBODY);
    h->lineno = lineno;
    h->kind = selectK;
    h->val.selectH.attributes = attributes;
    h->val.selectH.htmlbodies = htmlbodies;
    h->next = NULL;
    return h;
}

ATTRIBUTE *makeATTRIBUTE(char *name, char *value)
{
    ATTRIBUTE *a = NEW(ATTRIBUTE);
    a->name = name;
    a->value = value;
    a->next = NULL;
    return a;
}

SCHEMA *makeSCHEMA(char *name, VARIABLE *variables)
{
    SCHEMA *s = NEW(SCHEMA);
    s->lineno = lineno;
    s->name = name;
    s->variables = variables;
    s->next = NULL;
    return s;
}

VARIABLE *makeVARIABLE(TYPE *type, char *name)
{
    VARIABLE *v = NEW(VARIABLE);
    v->lineno = lineno;
    v->name = name;
    v->type = type;
    v->next = NULL;
    return v;
}

VARIABLE *makeVARIABLES(TYPE *type, ID *ids)
{
    VARIABLE *v = makeVARIABLE(type, ids->name);
    VARIABLE *current = v;
    ID *id;
    for (id = ids->next; id != NULL; id = id->next)
    {
        current->next = makeVARIABLE(type, id->name);
        current = current->next;
    }
    return v;

}

ID *makeID(char *name)
{
    ID *id = NEW(ID);
    id->name = name;
    id->next = NULL;
    return id;
}

TYPE *makeTYPEint(void)
{
    TYPE *t = NEW(TYPE);
    t->lineno = lineno;
    t->kind = intK;
    t->name = NULL;
    return t;
}

TYPE *makeTYPEbool(void)
{
    TYPE *t = NEW(TYPE);
    t->lineno = lineno;
    t->kind = boolK;
    t->name = NULL;
    return t;
}

TYPE *makeTYPEstring(void)
{                 
    TYPE *t = NEW(TYPE);
    t->lineno = lineno;
    t->kind = stringK;
    t->name = NULL;
    return t;
}

TYPE *makeTYPEvoid(void)
{                   
    TYPE *t = NEW(TYPE);
    t->lineno = lineno;
    t->kind = voidK;
    t->name = NULL;
    return t;
}

TYPE *makeTYPEtuple(char *name)
{
    TYPE *t = NEW(TYPE);
    t->lineno = lineno;
    t->kind = tupleK;
    t->name = name;
    return t;
}

FUNCTION *makeFUNCTION(TYPE *returntype, char *name, ARGUMENT *arguments, STATEMENT *body)
{
}
ARGUMENT *makeARGUMENT(TYPE *type, char *name)
{
}
SESSION *makeSESSION(char *name, STATEMENT *body)
{
}
DOCUMENT *makeDOCUMENT(char *name, PLUG *plugs)
{
}
STATEMENT *makeSTATEMENTskip(void)
{
}
STATEMENT *makeSTATEMENTshow(DOCUMENT *document, RECEIVE *receives)
{
}
STATEMENT *makeSTATEMENTexit(DOCUMENT *document)
{
}
STATEMENT *makeSTATEMENTreturn(EXP *exp)
{
}
STATEMENT *makeSTATEMENTblock(STATEMENT *body, VARIABLE *variables)
{
}
STATEMENT *makeSTATEMENTif(EXP *exp, STATEMENT *body)
{
}
STATEMENT *makeSTATEMENTifelse(EXP *exp, STATEMENT *then, STATEMENT *elsepart)
{
}
STATEMENT *makeSTATEMENTwhile(EXP *exp, STATEMENT *body)
{
}
STATEMENT *makeSTATEMENTexp(EXP *exp)
{
}
RECEIVE *makeRECEIVE(INPUT *inputs)
{
}
PLUG *makePLUG(char *name, EXP *exp)
{
}
EXP *makeEXPlvalue(char *lvalue)
{
}
EXP *makeEXPassign(char *lvalue, EXP *exp)
{
}
EXP *makeEXPeq(EXP *left, EXP *right)
{
}
EXP *makeEXPneq(EXP *left, EXP *right)
{
}
EXP *makeEXPle(EXP *left, EXP *right)
{
}
EXP *makeEXPge(EXP *left, EXP *right)
{
}
EXP *makeEXPleq(EXP *left, EXP *right)
{
}
EXP *makeEXPgeq(EXP *left, EXP *right)
{
}
EXP *makeEXPnot(EXP *exp)
{
}
EXP *makeEXPuminus(EXP *exp)
{
}
EXP *makeEXPplus(EXP *left, EXP *right)
{
}
EXP *makeEXPminus(EXP *left, EXP *right)
{
}
EXP *makeEXPtimes(EXP *left, EXP *right)
{
}
EXP *makeEXPdiv(EXP *left, EXP *right)
{
}
EXP *makeEXPmod(EXP *left, EXP *right)
{
}
EXP *makeEXPand(EXP *left, EXP *right)
{
}
EXP *makeEXPor(EXP *left, EXP *right);
EXP *makeEXPcombine(EXP *left, EXP *right);

