#ifndef __wig_tree_h
#define __wig_tree_h

typedef enum{htmlSym, variableSym, sessionSym, functionSym, argumentSym, schemaSym} SymbolKind;

typedef struct SYMBOL 
{
    char *name;
    SymbolKind kind;
    union {
        struct HTML *htmlS;
        struct VARIABLE *variableS;
        struct SESSION *sessionS;
        struct FUNCTION *functionS;
        struct ARGUMENT *argumentS;
        struct SCHEMA *schemaS;
    } val;
    struct SYMBOL *next;
} SYMBOL;

typedef struct SERVICE 
{
    struct HTML *htmls;
    struct SCHEMA *schemas;
    struct VARIABLE *variables;
    struct FUNCTION *functions;
    struct SESSION *sessions;
} SERVICE;

typedef struct SESSION
{
    int lineno;
    char *name;
    struct STATEMENT *statements;
    struct SESSION *next;
} SESSION;

typedef struct HTML
{
    int lineno;
    char *name;
    struct HTMLBODY *htmlbodies;
    struct HTML *next;
} HTML;

typedef struct HTMLBODY
{
    int lineno;
    enum {opentagK, closetagK, selfclosingK, gapK, whateverK, metaK, inputK, 
          selectK} kind;
    union
    {
        struct {char *name;
                struct ATTRIBUTE *attributes; } tagH;
        struct {struct ATTRIBUTE *attributes; } inputH;
        struct {struct ATTRIBUTE *attributes;
                struct HTMLBODY *htmlbodies; } selectH;
        char *gapH;
        char *whateverH;
        char *metaH;
    } val;
    struct HTMLBODY *next;
} HTMLBODY;

typedef struct ATTRIBUTE
{
    char *name;
    char *value;
    struct ATTRIBUTE *next;
} ATTRIBUTE;

typedef struct PLUG
{
    int lineno;
    char *name;
    struct EXP *exp;
    struct PLUG *next;
} PLUG;

typedef struct RECEIVE
{
    struct INPUT *inputs;
    struct RECEIVE *next;
} RECEIVE;

typedef struct INPUT
{
    int lineno;
    char *lhs;
    char *rhs;
    struct INPUT *next;
} INPUT;

typedef struct SCHEMA
{
    int lineno;
    char *name;
    struct VARIABLE *variables;
    struct SCHEMA *next;
} SCHEMA;

typedef struct ID
{
    int lineno;
    char *name;
    struct ID *next;
} ID;

typedef struct VARIABLE
{
    int lineno;
    char *name;
    struct TYPE *type;
    struct VARIABLE *next;
} VARIABLE;

typedef struct FUNCTION
{
    int lineno;
    char *name;
    struct TYPE* returntype;
    struct ARGUMENT *arguments;
    struct STATEMENT *statements;
    struct FUNCTION *next;
} FUNCTION;

typedef struct TYPE 
{
    int lineno;
    enum {intK, boolK, stringK, voidK, tupleK} kind;
    char *name;
} TYPE;

typedef struct STATEMENT
{
    int lineno;
    enum {skipK, sequenceK, showK, exitK, returnK, blockK, ifK, ifelseK, 
          whileK, expK} kind;
    union
    {
        struct EXP *expS;
        struct EXP *returnS;
        struct {struct STATEMENT *first;
                struct STATEMENT *second;} sequenceS;
        struct {struct EXP *condition;
                struct STATEMENT *body; } ifS;
        struct {struct EXP *condition;
                struct STATEMENT *thenpart;
                struct STATEMENT *elsepart; } ifelseS;
        struct {struct STATEMENT *body;
                struct VARIABLE *variables;
                struct SymbolTable *table;} blockS;
        struct {struct EXP *condition;
                struct STATEMENT *body;} whileS;
        struct {struct DOCUMENT *document;
                struct RECEIVE *receives;} showS;
        struct {struct DOCUMENT *document;} exitS;
    } val;
} STATEMENT;

typedef struct DOCUMENT
{
    int lineno;
    char *name;
    PLUG *plugs;
} DOCUMENT;

typedef struct EXP 
{
    int lineno;
    TYPE *type;
    enum {idK, idtupleK, assignK, assigntupleK, orK, andK, eqK, ltK, gtK, 
          leqK, geqK, neqK, plusK, minusK, timesK, divK, modK, notK, 
          uminusK, combineK, keepK, discardK, callK, intconstK, boolconstK,
          stringconstK, tupleconstK} kind;
    union
    {
        struct {char *name;
                SYMBOL *idsym; } idE; 
        struct {char *name;
                char *field;
                SCHEMA *schema; } idtupleE; 
        struct {char *left;
                SYMBOL *leftsym; 
                struct EXP *right; } assignE;
        struct {char *name;
                char *field;
                SCHEMA *leftschema; 
                struct EXP *right; } assigntupleE;
        struct {struct EXP *left;
                struct EXP *right; } binaryE;
        struct {struct EXP *left;
                ID *ids; } keepE;
        struct {struct EXP *left;
                ID *ids; } discardE;
        struct EXP *unaryE;
        struct {char *name;
                struct EXP *args; } callE;
        int intconstE;
        int boolconstE;
        char *stringconstE;
        struct {struct FIELDVALUE* fieldvalues;} tupleE;
    } val;
    struct EXP *next;
} EXP;

typedef struct ARGUMENT
{
    int lineno;
    struct TYPE *type;
    char *name;
    struct ARGUMENT *next;
} ARGUMENT;

typedef struct FIELDVALUE
{
    char *name;
    EXP *exp;
    struct FIELDVALUE *next;
} FIELDVALUE;

SERVICE *makeSERVICE(HTML *htmls, SCHEMA *schemas, VARIABLE *variables, FUNCTION *functions, SESSION *sessions);
HTML *makeHTML(char *name, HTMLBODY *htmlbodies); 
HTMLBODY *makeHTMLBODYopen(char *name, ATTRIBUTE *attributes);
HTMLBODY *makeHTMLBODYclose(char *name);
HTMLBODY *makeHTMLBODYselfclosing(char *name, ATTRIBUTE *attributes);
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
DOCUMENT *makeDOCUMENT(char *name, PLUG *plugs);
STATEMENT *makeSTATEMENTskip(void);
STATEMENT *makeSTATEMENTsequence(STATEMENT *first, STATEMENT *second);
STATEMENT *makeSTATEMENTshow(DOCUMENT *document, RECEIVE *receives);
STATEMENT *makeSTATEMENTexit(DOCUMENT *document);
STATEMENT *makeSTATEMENTreturn(EXP *exp);
STATEMENT *makeSTATEMENTblock(STATEMENT *body, VARIABLE *variables);
STATEMENT *makeSTATEMENTif(EXP *exp, STATEMENT *body);
STATEMENT *makeSTATEMENTifelse(EXP *exp, STATEMENT *then, STATEMENT *elsepart);
STATEMENT *makeSTATEMENTwhile(EXP *exp, STATEMENT *body);
STATEMENT *makeSTATEMENTexp(EXP *exp);
RECEIVE *makeRECEIVE(INPUT *inputs);
PLUG *makePLUG(char *name, EXP *exp);
INPUT *makeINPUT(char *lhs, char *rhs);
EXP *makeEXPlvalue(char *lvalue);
EXP *makeEXPassign(char *lvalue, EXP *exp);
EXP *makeEXPeq(EXP *left, EXP *right);
EXP *makeEXPneq(EXP *left, EXP *right);
EXP *makeEXPlt(EXP *left, EXP *right);
EXP *makeEXPgt(EXP *left, EXP *right);
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
EXP *makeEXPkeep(EXP *left, ID *ids);
EXP *makeEXPdiscard(EXP *left, ID *ids);
EXP *makeEXPcall(char *name, EXP *exps);
EXP *makeEXPintconst(int i);
EXP *makeEXPboolconst(int b);
EXP *makeEXPstringconst(char *string);
EXP *makeEXPtuple(FIELDVALUE *fieldvalues);
FIELDVALUE *makeFIELDVALUE(char *name, EXP *exp);

#endif
