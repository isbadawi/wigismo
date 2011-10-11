#ifndef __wig_tree_h
#define __wig_tree_h

typedef struct PROGRAM 
{
    char* name;
    struct SERVICE* service;
} PROGRAM;

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
    char *body;
    struct HOLE *holes;
    struct INPUT *inputs;
    struct HTML *next;
} HTML;

typedef struct HOLE
{
    char *name;
    int position;
    struct HOLE *next;
} HOLE;

typedef struct INPUT
{
    char *name;
    struct INPUT *next;
} INPUT;

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
    enum {showK, exitK, returnK, ifK, ifelseK, whileK, expK, whileK} kind;
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
                struct PLUG* plugs; } showS;
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
    } val;
} EXP;

#endif
