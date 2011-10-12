%{

#include<stdlib.h>
#include"tree.h"

extern SERVICE *theservice;

%}

%start service

%union {
    struct SERVICE *service;
    struct HTML *html;
    struct HTMLBODY *htmlbody;
    struct SCHEMA *schema;
    struct VARIABLE *variable;
    struct FUNCTION *function;
    struct SESSION *session;
    struct FORMAL *formal;
    struct TYPE *type;
    struct INPUT *input;
    struct PLUG *plug;
    struct RECEIVE *receive;
    struct ARGUMENT *argument;
    struct STATEMENT *statement;
    struct EXP *exp;
    int intconst;
    int boolconst;
    char *stringconst;
};

%token tSERVICE tCONST tHTML tSESSION tSHOW tEXIT tINT tBOOL tSTRING tVOID tSCHEMA
       tTUPLE tIF tELSE tWHILE tRETURN tINPUT tSELECT tTEXT tRADIO tPLUG tRECEIVE
       tEQ tLEQ tGEQ tNEQ tAND tOR tKEEP tDISCARD tCOMBINE tUMINUS

%token <intconst> tINTCONST
%token <boolconst> tBOOLCONST
%token <stringconst> tSTRINGCONST tIDENTIFIER tMETA tWHATEVER

%type <service> service
%type <html> htmls html
%type <htmlbody> nehtmlbodies htmlbody
%type <schema> schemas neschemas schema
%type <variable> variable nevariables
%type <type> type simpletype
%type <function> function functions nefunctions
%type <argument> argument nearguments arguments
%type <session> session sessions;
%type <statement> stm stms nestms compoundstm stm stmnoshortif
%type <exp> exp
%type <plug> plug plugs
%type <input> input neinputs inputs

%right '='
%left '+' '-'
%left '*' '/' '%'
%left tAND tOR
%left tKEEP tDISCARD
%right tCOMBINE
%right '!' tUMINUS
%nonassoc tEQ tLEQ tGEQ tNEQ '<' '>'

%% /* productions */

service : tSERVICE '{' htmls schemas nevariables functions sessions '}' /* NEW */
          { $$ = makeSERVICE($3, $4, $5, $6, $7); }
        | tSERVICE '{' htmls schemas functions sessions '}'             /* NEW */
          { $$ = makeSERVICE($3, $4, NULL, $5, $6); }
;

htmls : html 
          { $$ = $1; }
        | htmls html
          { $$ = $2; $$->next = $1; }
;

html : tCONST tHTML tIDENTIFIER "=" "<html>" nehtmlbodies "</html>" ";" /* NEW */
          { $$ = makeHTML($3, $6); }
     | tCONST tHTML tIDENTIFIER "=" "<html>" "</html>" ";"              /* NEW */
          { $$ = makeHTML($3, NULL); }
;

nehtmlbodies : htmlbody 
                { $$ = $1; }
             | nehtmlbodies htmlbody
                { $$ = $2; $$->next = $1; }
;

htmlbody : "<" tIDENTIFIER attributes ">"
            { $$ = makeHTMLBODYopen($2, $3); }
         | "</" tIDENTIFIER ">"
            { $$ = makeHTMLBODYclose($2); }
         | "<[" tIDENTIFIER "]>"
            { $$ = makeHTMLBODYgap($2); }
         | tWHATEVER
            { $$ = makeHTMLBODYwhatever($1); }
         | tMETA
            { $$ = makeHTMLBODYmeta($1); }
         | "<" tINPUT inputattrs ">"
            { $$ = makeHTMLBODYinput($2, $3); }
         | "<" tSELECT inputattrs ">" nehtmlbodies "</" tSELECT ">" /* NEW */
            { $$ = makeHTMLBODYselect($3, $5); }
         | "<" tSELECT inputattrs ">" "</" tSELECT ">"              /* NEW */
            { $$ = makeHTMLBODYselect($3, NULL); }
;

inputattrs : inputattr 
             { $$ = $1; }
           | inputattrs inputattr
             { $$ = $2; $$->next = $1; }
;

inputattr : "name" "=" attr
             { makeATTRIBUTE($1, $3); }
          | "type" "=" inputtype
             { makeATTRIBUTE($1, $3); }
          | attribute
             { $$ = $1; }
;

inputtype : tTEXT | tRADIO
;

attributes : /* empty */ 
             { $$ = NULL; }
           | neattributes
             { $$ = $1; }
;

neattributes : attribute 
                { $$ = $1; }
             | neattributes attribute
                { $$ = $2; $$->next = $1; }
;

attribute : attr 
             { $$ = makeATTRIBUTE($1, NULL); }
          | attr "=" attr
             { $$ = makeATTRIBUTE($1, $3); }
;

attr : tIDENTIFIER | tSTRINGCONST
;

schemas: /* empty */ | neschemas
;
neschemas: schema | neschemas schema
;
schema : tSCHEMA tIDENTIFIER "{" fields "}"
;
fields : /* empty */ | nefields
;
nefields : field | nefields field
;
field : simpletype tIDENTIFIER ";"
;

/* NEW: variables production deleted */
nevariables : variable | nevariables variable 
;
variable : type identifiers ";"
;
identifiers : tIDENTIFIER | identifiers "," tIDENTIFIER
;

simpletype : tINT | tBOOL | tSTRING | tVOID
;
type : simpletype | tTUPLE tIDENTIFIER
;

functions : /* empty */ | nefunctions
;
nefunctions : function | nefunctions function
;
function : type tIDENTIFIER "(" arguments ")" compoundstm
;
arguments : /* empty */ | nearguments
;
nearguments : argument | nearguments "," argument
;
argument : type tIDENTIFIER
;

sessions : session | sessions session
;
session : tSESSION tIDENTIFIER "(" ")" compoundstm
;

stms : /* empty */ | nestms
;
nestms : stm | nestms stm
;
stm : ";"
    | tSHOW document receive ";"
    | tEXIT document ";"
    | tRETURN ";"
    | tRETURN exp ";"
    | tIF "(" exp ")" stm
    | tIF "(" exp ")" stmnoshortif tELSE stm
    | tWHILE "(" exp ")" stm
    | compoundstm
    | exp ";"
;

stmnoshortif : ";"
    | tSHOW document receive ";"
    | tEXIT document ";"
    | tRETURN ";"
    | tRETURN exp ";"
    | tIF "(" exp ")" stmnoshortif tELSE stmnoshortif
    | tWHILE "(" exp ")" stmnoshortif
    | compoundstm
    | exp ";"
;

document : tIDENTIFIER 
         | tPLUG tIDENTIFIER "[" plugs "]"
;
receive : /* empty */
        | tRECEIVE "[" inputs "]"
;
compoundstm : "{" nevariables stms "}" /* NEW */
            | "{" stms "}"             /* NEW */
;
plugs : plug | plugs "," plug
;
plug : tIDENTIFIER "=" exp
;
inputs : /* empty */ | neinputs
;
neinputs : input | neinputs "," input
;
input : lvalue "=" tIDENTIFIER
;

exp : lvalue
    | lvalue '=' exp
    | exp tEQ exp
    | exp tNEQ exp
    | exp '<' exp
    | exp '>' exp
    | exp tLEQ exp
    | exp tGEQ exp
    | '!' exp
    | '-' exp %prec tUMINUS
    | exp '+' exp
    | exp '-' exp
    | exp '*' exp
    | exp '/' exp
    | exp '%' exp
    | exp tAND exp
    | exp tOR exp
    | exp tCOMBINE exp
    | exp tKEEP tIDENTIFIER
    | exp tKEEP "(" identifiers ")" /* NEW */
    | exp tDISCARD tIDENTIFIER         /* NEW */
    | exp tDISCARD "(" identifiers ")" /* NEW */
    | tIDENTIFIER "(" exps ")"
    | tINTCONST
    | tBOOLCONST
    | tSTRINGCONST
    | tTUPLE "{" fieldvalues "}"
    | "(" exp ")"                  /* NEW */
;
exps : /* empty */ | neexps
;
neexps : exp | neexps "," exp
;
lvalue : tIDENTIFIER | tIDENTIFIER "." tIDENTIFIER
;
fieldvalues : /* empty */ | nefieldvalues
;
nefieldvalues : fieldvalue | fieldvalues "," fieldvalue
;
fieldvalue : tIDENTIFIER "=" exp
;
