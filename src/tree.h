#ifndef __wig_tree_h
#define __wig_tree_h

typedef struct SERVICE 
{
    char *name;
    struct HTML *htmls;
    struct SCHEMA *schemas;
    struct VARIABLE *variables;
    struct FUNCTION *functions;
    struct SESSION *sessions;
} SERVICE;

typedef struct HTML
{
    char *name;
    struct HTMLBODY *htmlbodies;
    struct HTML *next;
} HTML;

typedef struct HTMLBODY
{
    enum {opentagK, closetagK, gapK, whateverK, metaK, inputK, selectK} kind;
    union
    {
        struct {char *name;
                struct ATTRIBUTE *attributes; } tagH;
        struct {char *name;
                struct ATTRIBUTE *attributes; } inputH;
        struct {struct ATTRIBUTE *attributes;
                struct HTMLBODY *htmlbodies; } selectH;
        char *gapH;
        char *whateverH;
        char *metaH;
    } val;
} HTMLBODY;

typedef struct ATTRIBUTE
{
    char *name;
    char *value;
    struct ATTRIBUTE *next;
} ATTRIBUTE;

typedef struct PLUG
{
    char *name;
    struct EXP *exp;
    struct PLUG *next;
}

typedef struct RECEIVE
{
    char *var;
    char *plug;
    struct RECEIVE *next;
}

typedef struct SCHEMA
{
    int lineno;
    char *name;
    struct VARIABLE *variables;
    struct SCHEMA *next;
} SCHEMA;

typedef struct ID
{
    char *name;
    struct ID *next;
} ID;

typedef struct VARIABLE
{
    int lineno;
    char *name;
    struct TYPE *type;
    VARIABLE *next;
} VARIABLE;

typedef struct FUNCTION
{
    int lineno;
    char *name;
    struct TYPE* returntype;
    struct FORMAL *formals;
    struct STATEMENT *statements;
    struct FUNCTION *next;
} FUNCTION;

typedef struct TYPE 
{
    int lineno;
    enum {intK, boolK, stringK, voidK, tupleK} kind;
    char *name;
    struct TUPLE* tuple;
} TYPE;

typedef struct FORMAL
{
    int lineno;
    char *name;
    struct TYPE *type;
    int offset;
    struct FORMAL *next;
} FORMAL;

typedef struct LOCAL
{
    int lineno;
    char *name;
    struct TYPE *type;
    int offset;
    struct LOCAL *next;
} LOCAL;

typedef struct STATEMENT
{
    int lineno;
    enum {skipK, showK, exitK, returnK, ifK, ifelseK, whileK, expK, whileK} kind;
    union
    {
        struct EXP *expS;
        struct LOCAL *localS;
        struct EXP *returnS;
        struct {struct STATEMENT *first;
                struct STATEMENT *second;} sequenceS;
        struct {struct EXP *condition;
                struct STATEMENT *body;
                int stoplabel; } ifS;
        struct {struct EXP *condition;
                struct STATEMENT *thenpart;
                struct STATEMENT *elsepart;
                int elselabel, stoplabel; } ifelseS;
        struct {struct EXP *condition;
                struct STATEMENT *body;
                int startlabel, stoplabel; } whileS;
        struct {struct HTML* html;
                struct PLUG* plugs; 
                struct RECEIVE* receives;} showS;
        struct {struct HTML* html;
                struct PLUG* plugs; } exitS;
    } val;
} STATEMENT;

typedef struct EXP 
{
    int lineno;
    TYPE *type;
    enum {idK, assignK, orK, andK, eqK, ltK, gtK, leqK, geqK, neqK, plusK,
          minusK, timesK, divK, modK, notK, uminusK, combineK, keepK,
          discardK, callK, intconstK, boolconstK, stringconstK, tupleK} kind;
    union
    {
        struct {char *name;
                SYMBOL *idsym; } idE;
        struct {char *left;
                SYMBOL *leftsym; 
                struct EXP *right; } assignE;
        struct {struct EXP *left;
                struct EXP *right; } binaryE;
        struct EXP *unaryE:
        struct {char *name;
                struct FUNCTION *function;
                struct ARGUMENT *args; } callE;
        int intconstE;
        int boolconstE;
        char *stringconstE;
        struct TUPLE *tupleE;
    } val;
} EXP;

typedef struct ARGUMENT
{
    struct EXP *exp;
    struct ARGUMENT *next;
} ARGUMENT;

typedef struct TUPLE
{
    char *name;
    struct SCHEMA *schema;
    /* ... ? */
} TUPLE;

SERVICE *makeSERVICE(char *name, HTML *htmls, SCHEMA *schemas, VARIABLE *variables, FUNCTION *functions, SESSION *sessions);
HTML *makeHTML(char *name, HTMLBODY *htmlbodies); 
HTMLBODY *makeHTMLBODYopen(char *name, ATTRIBUTE *attributes);
HTMLBODY *makeHTMLBODYclose(char *name);
HTMLBODY *makeHTMLBODYgap(char *name);
HTMLBODY *makeHTMLBODYwhatever(char *whatever);
HTMLBODY *makeHTMLBODYmeta(char *meta);
HTMLBODY *makeHTMLBODYinput(ATTRIBUTE *attributes);
HTMLBODY *makeHTMLBODYselect(ATTRIBUTE *attributes, HTMLBODY *htmlbodies);
ATTRIBUTE *makeATTRIBUTE(char *name, char *value);
SCHEMA *makeSCHEMA(char *name, VARIABLE *variables);
VARIABLE *makeVARIABLE(TYPE *type, char *name);
VARIABLE *makeVARIABLES(TYPE *type, ID *ids);
ID *makeID(char *name);
TYPE *makeTYPEint(void);
TYPE *makeTYPEbool(void);
TYPE *makeTYPEstring(void);
TYPE *makeTYPEvoid(void);
TYPE *makeTYPEtuple(char *name);
FUNCTION *makeFUNCTION(TYPE *returntype, char *name, ARGUMENT *arguments, STATEMENT *body);
ARGUMENT *makeARGUMENT(TYPE *type, char *name);
SESSION *makeSESSION(char *name, STATEMENT *body);
STATEMENT *makeSTATEMENTskip(void);
// FIXME:  ADD DOCUMENT?
STATEMENT *makeSTATEMENTshow(int document, RECEIVE *receives);
STATEMENT *makeSTATEMENTexit(int document);
STATEMENT *makeSTATEMENTreturn(EXP *exp);
STATEMENT *makeSTATEMENTif(EXP *exp, STATEMENT *body);
STATEMENT *makeSTATEMENTifelse(EXP *exp, STATEMENT *then, STATEMENT *elsepart);
STATEMENT *makeSTATEMENTwhile(EXP *exp, STATEMENT *body);
STATEMENT *makeSTATEMENTexp(EXP *exp);
RECEIVE *makeRECEIVE(INPUT *inputs);
PLUG *makePLUG(char *name, EXP *exp);
EXP *makeEXPlvalue(char *lvalue);
EXP *makeEXPassign(char *lvalue, EXP *exp);
EXP *makeEXPeq(EXP *left, EXP *right);
EXP *makeEXPneq(EXP *left, EXP *right);
EXP *makeEXPle(EXP *left, EXP *right);
EXP *makeEXPge(EXP *left, EXP *right);
EXP *makeEXPleq(EXP *left, EXP *right);
EXP *makeEXPgeq(EXP *left, EXP *right);
EXP *makeEXPnot(EXP *exp);
EXP *makeEXPuminus(EXP *exp);
EXP *makeEXPplus(EXP *left, EXP *right);
EXP *makeEXPminus(EXP *left, EXP *right);
EXP *makeEXPtimes(EXP *left, EXP *right);
EXP *makeEXPdiv(EXP *left, EXP *right);
EXP *makeEXPmod(EXP *left, EXP *right);
EXP *makeEXPand(EXP *left, EXP *right);
EXP *makeEXPor(EXP *left, EXP *right);
EXP *makeEXPcombine(EXP *left, EXP *right);
EXP *makeEXPkeep(EXP *left, EXP *right);
EXP *makeEXPdiscard(EXP *left, EXP *right);
EXP *makeEXPcall(char *name, EXP *exps);
EXP *makeEXPintconst(int i);
EXP *makeEXPboolconst(int b);
EXP *makeEXPstringconst(char *string);
EXP *makeEXPtuple(FIELDVALUE *fieldvalues);

#endif
