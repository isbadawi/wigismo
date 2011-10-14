#include"tree.h"
#include"memory.h"
#include<stdlib.h>

extern int lineno;

SERVICE *makeSERVICE(HTML *htmls, SCHEMA *schemas, 
                     VARIABLE *variables, FUNCTION *functions,
                     SESSION *sessions)
{
    SERVICE *s = NEW(SERVICE);
    s->htmls = htmls;
    s->schemas = schemas;
    s->variables = variables;
    s->functions = functions;
    s->sessions = sessions;
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
    FUNCTION *f = NEW(FUNCTION);
    f->lineno = lineno;
    f->name = name;
    f->returntype = returntype;
    f->arguments = arguments;
    f->statements = body;
    f->next = NULL;
    return f;
}

ARGUMENT *makeARGUMENT(TYPE *type, char *name)
{
    ARGUMENT *a = NEW(ARGUMENT);
    a->lineno = lineno;
    a->type = type;
    a->name = name;
    a->next = NULL;
    return a;
}

SESSION *makeSESSION(char *name, STATEMENT *body)
{
    SESSION *s = NEW(SESSION);
    s->lineno = lineno;
    s->name = name;
    s->statements = body;
    s->next = NULL;
    return s;
}

DOCUMENT *makeDOCUMENT(char *name, PLUG *plugs)
{
    DOCUMENT *d = NEW(DOCUMENT);
    d->name = name;
    d->plugs = plugs;
    return d;
}

STATEMENT *makeSTATEMENTskip(void)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = skipK;
    return s;
}

STATEMENT *makeSTATEMENTsequence(STATEMENT *first, STATEMENT *second)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = sequenceK;
    s->val.sequenceS.first = first;
    s->val.sequenceS.second = second;
    return s;
}

STATEMENT *makeSTATEMENTshow(DOCUMENT *document, RECEIVE *receives)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = showK;
    s->val.showS.document = document;
    s->val.showS.receives = receives;
    return s;
}

STATEMENT *makeSTATEMENTexit(DOCUMENT *document)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = exitK;
    s->val.exitS.document = document;
    return s;
}

STATEMENT *makeSTATEMENTreturn(EXP *exp)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = returnK;
    s->val.returnS = exp;
    return s;
}

STATEMENT *makeSTATEMENTblock(STATEMENT *body, VARIABLE *variables)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = blockK;
    s->val.blockS.body = body;
    s->val.blockS.variables = variables;
    return s;
}

STATEMENT *makeSTATEMENTif(EXP *exp, STATEMENT *body)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = ifK;
    s->val.ifS.body = body;
    s->val.ifS.condition = exp;
    return s;
}

STATEMENT *makeSTATEMENTifelse(EXP *exp, STATEMENT *then, STATEMENT *elsepart)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = ifelseK;
    s->val.ifelseS.condition = exp;
    s->val.ifelseS.thenpart = then;
    s->val.ifelseS.elsepart = elsepart;
    return s;
}

STATEMENT *makeSTATEMENTwhile(EXP *exp, STATEMENT *body)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = whileK;
    s->val.whileS.condition = exp;
    s->val.whileS.body = body;
    return s;
}

STATEMENT *makeSTATEMENTexp(EXP *exp)
{
    STATEMENT *s = NEW(STATEMENT);
    s->lineno = lineno;
    s->kind = expK;
    s->val.expS = exp;
    return s;
}

RECEIVE *makeRECEIVE(INPUT *inputs)
{
    RECEIVE *r = NEW(RECEIVE);
    r->inputs = inputs;
    r->next = NULL;
    return r;
}

PLUG *makePLUG(char *name, EXP *exp)
{
    PLUG *p = NEW(PLUG);
    p->name = name;
    p->exp = exp;
    p->next = NULL;
    return p;
}

INPUT *makeINPUT(char *lhs, char *rhs)
{
    INPUT *i = NEW(INPUT);
    i->lhs = rhs;
    i->rhs = rhs;
    i->next = NULL;
    return i;
}

/* TODO for these two: SYMBOL? */
EXP *makeEXPlvalue(char *lvalue)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = idK;
    e->val.idE.name = lvalue;
    e->next = NULL;
    return e;
}

EXP *makeEXPassign(char *lvalue, EXP *exp)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = assignK;
    e->val.assignE.left = lvalue;
    e->val.assignE.right = exp;
    e->next = NULL;
    return e;
}

EXP *makeEXPbinary(int kind, EXP *left, EXP *right)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = kind;
    e->val.binaryE.left = left;
    e->val.binaryE.right = right;
    e->next = NULL;
    return e;
}

EXP *makeEXPunary(int kind, EXP *exp)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = kind;
    e->val.unaryE = exp;
    e->next = NULL;
    return e;
}

EXP *makeEXPeq(EXP *left, EXP *right)
{
    return makeEXPbinary(eqK, left, right);
}

EXP *makeEXPneq(EXP *left, EXP *right)
{
    return makeEXPbinary(neqK, left, right);
}

EXP *makeEXPlt(EXP *left, EXP *right)
{
    return makeEXPbinary(ltK, left, right);
}

EXP *makeEXPgt(EXP *left, EXP *right)
{
    return makeEXPbinary(gtK, left, right);
}

EXP *makeEXPleq(EXP *left, EXP *right)
{
    return makeEXPbinary(leqK, left, right);
}

EXP *makeEXPgeq(EXP *left, EXP *right)
{
    return makeEXPbinary(geqK, left, right);
}

EXP *makeEXPnot(EXP *exp)
{
    return makeEXPunary(notK, exp);
}

EXP *makeEXPuminus(EXP *exp)
{
    return makeEXPunary(uminusK, exp);
}

EXP *makeEXPplus(EXP *left, EXP *right)
{
    return makeEXPbinary(plusK, left, right);
}

EXP *makeEXPminus(EXP *left, EXP *right)
{
    return makeEXPbinary(minusK, left, right);
}

EXP *makeEXPtimes(EXP *left, EXP *right)
{
    return makeEXPbinary(timesK, left, right);
}

EXP *makeEXPdiv(EXP *left, EXP *right)
{
    return makeEXPbinary(divK, left, right);
}

EXP *makeEXPmod(EXP *left, EXP *right)
{
    return makeEXPbinary(modK, left, right);
}

EXP *makeEXPand(EXP *left, EXP *right)
{
    return makeEXPbinary(andK, left, right);
}

EXP *makeEXPor(EXP *left, EXP *right)
{
    return makeEXPbinary(orK, left, right);
}

EXP *makeEXPcombine(EXP *left, EXP *right)
{
    return makeEXPbinary(combineK, left, right);
}

EXP *makeEXPkeep(EXP *left, ID *ids)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = keepK;
    e->val.keepE.left = left;
    e->val.keepE.ids = ids;
    e->next = NULL;
    return e;              
}

EXP *makeEXPdiscard(EXP *left, ID *ids)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = discardK;
    e->val.discardE.left = left;
    e->val.discardE.ids = ids;
    e->next = NULL;
    return e;
}

EXP *makeEXPcall(char *name, EXP *exps)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = callK;
    e->val.callE.name = name;
    e->val.callE.args = exps;
    e->next = NULL;
    return e;
}

EXP *makeEXPintconst(int i)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = intconstK;
    e->val.intconstE = i;
    e->next = NULL;
    return e;
}

EXP *makeEXPboolconst(int b)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = boolconstK;
    e->val.boolconstE = b;
    e->next = NULL;
    return e;
}

EXP *makeEXPstringconst(char *string)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = stringconstK;
    e->val.stringconstE = string;
    e->next = NULL;
    return e;
}

EXP *makeEXPtuple(FIELDVALUE *fieldvalues)
{
    EXP *e = NEW(EXP);
    e->lineno = lineno;
    e->kind = tupleconstK;
    e->val.tupleE.fieldvalues = fieldvalues;
    e->next = NULL;
    return e;
}

FIELDVALUE *makeFIELDVALUE(char *name, EXP *exp)
{
    FIELDVALUE *fv = NEW(FIELDVALUE);
    fv->name = name;
    fv->exp = exp;
    fv->next = NULL;
    return fv;
}
