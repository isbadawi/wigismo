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

#endif
