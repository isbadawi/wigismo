%{

#include<stdlib.h>
#include<string.h>
#include"tree.h"

extern SERVICE *theservice;

%}

%start service

%union {
    struct SERVICE *service;
    struct HTML *html;
    struct HTMLBODY *htmlbody;
    struct ATTRIBUTE *attribute;
    struct SCHEMA *schema;
    struct ID *id;
    struct VARIABLE *variable;
    struct FUNCTION *function;
    struct SESSION *session;
    struct TYPE *type;
    struct INPUT *input;
    struct DOCUMENT *document;
    struct PLUG *plug;
    struct RECEIVE *receive;
    struct ARGUMENT *argument;
    struct STATEMENT *statement;
    struct EXP *exp;
    struct FIELDVALUE *fieldvalue;
    int intconst;
    int boolconst;
    char *stringconst;
};

%token tSERVICE tCONST tHTML tSESSION tSHOW tEXIT tINT tBOOL tSTRING tVOID 
       tSCHEMA tTUPLE tIF tELSE tWHILE tRETURN tINPUT tSELECT
       tPLUG tRECEIVE tEQ tLEQ tGEQ tNEQ tAND tOR tKEEP tDISCARD tCOMBINE 
       tUMINUS tNAME tTYPE tHTMLOPEN tHTMLCLOSE tOPENINGTAG tCLOSINGTAG
       tOPENINGGAP tCLOSINGGAP tERROR

%token <intconst> tINTCONST
%token <boolconst> tBOOLCONST
%token <stringconst> tSTRINGCONST tIDENTIFIER tMETA tWHATEVER tTEXT tRADIO

%type <service> service
%type <html> htmls html
%type <htmlbody> nehtmlbodies htmlbody
%type <attribute> inputattr inputattrs attribute neattributes attributes
%type <schema> schemas neschemas schema
%type <id> identifiers
%type <variable> variable nevariables field fields nefields
%type <type> type simpletype
%type <function> function functions nefunctions
%type <argument> argument nearguments arguments
%type <session> session sessions;
%type <statement> stm stms nestms compoundstm stmnoshortif
%type <receive> receive
%type <document> document
%type <exp> exp exps neexps
%type <fieldvalue> fieldvalue fieldvalues nefieldvalues
%type <plug> plug plugs
%type <input> input neinputs inputs
%type <stringconst> lvalue attr inputtype

%right '='               
%left tOR
%left tAND 
%nonassoc tEQ tLEQ tGEQ tNEQ '<' '>'
%left '+' '-'
%left '*' '/' '%'
%left tKEEP tDISCARD
%right tCOMBINE
%right '!' tUMINUS

%% /* productions */

service : tSERVICE '{' htmls schemas nevariables functions sessions '}' /* NEW */
          { theservice = makeSERVICE($3, $4, $5, $6, $7); }
        | tSERVICE '{' htmls schemas functions sessions '}'             /* NEW */
          { theservice = makeSERVICE($3, $4, NULL, $5, $6); }
;

htmls : html 
          { $$ = $1; }
        | htmls html
          { $$ = $2; $$->next = $1; }
;

html : tCONST tHTML tIDENTIFIER '=' tHTMLOPEN nehtmlbodies tHTMLCLOSE ';' /* NEW */
          { $$ = makeHTML($3, $6); }
     | tCONST tHTML tIDENTIFIER '=' tHTMLOPEN tHTMLCLOSE ';'              /* NEW */
          { $$ = makeHTML($3, NULL); }
;

nehtmlbodies : htmlbody 
                { $$ = $1; }
             | nehtmlbodies htmlbody
                { $$ = $2; $$->next = $1; }
;

htmlbody : '<' tIDENTIFIER attributes '>'
            { $$ = makeHTMLBODYopen($2, $3); }
         | '<' tIDENTIFIER attributes tCLOSINGTAG
            { $$ = makeHTMLBODYselfclosing($2, $3); }
         | tOPENINGTAG tIDENTIFIER '>'
            { $$ = makeHTMLBODYclose($2); }
         | tOPENINGGAP tIDENTIFIER tCLOSINGGAP
            { $$ = makeHTMLBODYgap($2); }
         | tWHATEVER
            { $$ = makeHTMLBODYwhatever($1); }
         | tMETA
            { $$ = makeHTMLBODYmeta($1); }
         | '<' tINPUT inputattrs '>'
            { $$ = makeHTMLBODYinput($3); }
         | '<' tSELECT inputattrs '>' nehtmlbodies tOPENINGTAG tSELECT '>' /* NEW */
            { $$ = makeHTMLBODYselect($3, $5); }
         | '<' tSELECT inputattrs '>' tOPENINGTAG tSELECT '>'              /* NEW */
            { $$ = makeHTMLBODYselect($3, NULL); }
;

inputattrs : inputattr 
             { $$ = $1; }
           | inputattrs inputattr
             { $$ = $2; $$->next = $1; }
;

inputattr : tNAME '=' attr
             { $$ = makeATTRIBUTE("name", $3); }
          | tTYPE '=' inputtype
             { $$ = makeATTRIBUTE("type", $3); }
          | attribute
             { $$ = $1; }
;

inputtype : tTEXT 
            { $$ = $1; }
          | tRADIO
            { $$ = $1; }

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
          | attr '=' attr
             { $$ = makeATTRIBUTE($1, $3); }
;

attr : tIDENTIFIER | tSTRINGCONST
;

schemas : /* empty */ 
          { $$ = NULL; }
        | neschemas
          { $$ = $1; }
;

neschemas : schema 
              { $$ = $1; }
          | neschemas schema
              { $$ = $2; $$->next = $1; }
;

schema : tSCHEMA tIDENTIFIER '{' fields '}'
          { $$ = makeSCHEMA($2, $4); }
;

fields : /* empty */ 
          { $$ = NULL; }
       | nefields
          { $$ = $1; }
;

nefields : field 
            { $$ = $1; }
         | nefields field
            { $$ = $2; $$->next = $1; }
;

field : simpletype tIDENTIFIER ';'
        { $$ = makeVARIABLE($1, $2); }
;

/* NEW: variables production deleted */
nevariables : variable 
               { $$ = $1; }
            | nevariables variable 
               { $$ = $2; 
                 VARIABLE *v = $$;
                 while (v->next != NULL)
                     v = v->next;
                 v->next = $1; }
;

variable : type identifiers ';'
            { $$ = makeVARIABLES($1, $2); }
;

identifiers : tIDENTIFIER 
              { $$ = makeID($1); }
            | identifiers ',' tIDENTIFIER
              { $$ = makeID($3); $$->next = $1; }
;

simpletype : tINT 
              { $$ = makeTYPEint(); }
            | tBOOL 
              { $$ = makeTYPEbool(); }
            | tSTRING 
              { $$ = makeTYPEstring(); }
            | tVOID
              { $$ = makeTYPEvoid(); }
;
type : simpletype 
        { $$ = $1; }
     | tTUPLE tIDENTIFIER
        { $$ = makeTYPEtuple($2); }
;

functions : /* empty */ 
            { $$ = NULL; }
          | nefunctions
            { $$ = $1; }
;

nefunctions : function 
               { $$ = $1; }
            | nefunctions function
               { $$ = $2; $$->next = $1; }
;

function : type tIDENTIFIER '(' arguments ')' compoundstm
             { $$ = makeFUNCTION($1, $2, $4, $6); }
;

arguments : /* empty */ 
            { $$ = NULL; }
          | nearguments
            { $$ = $1; }
;
nearguments : argument 
               { $$ = $1; }
            | nearguments ',' argument
               { $$ = $3; $$->next = $1; }
;

argument : type tIDENTIFIER
            { $$ = makeARGUMENT($1, $2); }
;

sessions : session 
           { $$ = $1; }
         | sessions session
           { $$ = $2; $$->next = $1; }
;

session : tSESSION tIDENTIFIER '(' ')' compoundstm
           { $$ = makeSESSION($2, $5); }
;

stms : /* empty */ 
       { $$ = NULL; }
     | nestms
       { $$ = $1; }
;
nestms : stm 
         { $$ = $1; }
       | nestms stm
         { $$ = makeSTATEMENTsequence($1, $2); }
;
stm : ';'
      { $$ = makeSTATEMENTskip(); }
    | tSHOW document receive ';'
      { $$ = makeSTATEMENTshow($2, $3); }
    | tEXIT document ';'
      { $$ = makeSTATEMENTexit($2); }
    | tRETURN ';'
      { $$ = makeSTATEMENTreturn(NULL); }
    | tRETURN exp ';'
      { $$ = makeSTATEMENTreturn($2); }
    | tIF '(' exp ')' stm
      { $$ = makeSTATEMENTif($3, $5); }
    | tIF '(' exp ')' stmnoshortif tELSE stm
      { $$ = makeSTATEMENTifelse($3, $5, $7); }
    | tWHILE '(' exp ')' stm
      { $$ = makeSTATEMENTwhile($3, $5); }
    | compoundstm
      { $$ = $1; }
    | exp ';'
      { $$ = makeSTATEMENTexp($1); }
;

stmnoshortif : ';'
      { $$ = makeSTATEMENTskip(); }
    | tSHOW document receive ';'
      { $$ = makeSTATEMENTshow($2, $3); }
    | tEXIT document ';'
      { $$ = makeSTATEMENTexit($2); }
    | tRETURN ';'
      { $$ = makeSTATEMENTreturn(NULL); }
    | tRETURN exp ';'
      { $$ = makeSTATEMENTreturn($2); }
    | tIF '(' exp ')' stmnoshortif tELSE stmnoshortif
      { $$ = makeSTATEMENTifelse($3, $5, $7); }
    | tWHILE '(' exp ')' stmnoshortif
      { $$ = makeSTATEMENTwhile($3, $5); }
    | compoundstm
      { $$ = $1; }
    | exp ';'
      { $$ = makeSTATEMENTexp($1); }
;

document : tIDENTIFIER 
           { $$ = makeDOCUMENT($1, NULL); }
         | tPLUG tIDENTIFIER '[' plugs ']'
           { $$ = makeDOCUMENT($2, $4); }
;
receive : /* empty */
          { $$ = NULL; }
        | tRECEIVE '[' inputs ']'
          { $$ = makeRECEIVE($3); }
;

compoundstm : '{' nevariables stms '}' /* NEW */
               { $$ = makeSTATEMENTblock($3, $2); }
            | '{' stms '}'             /* NEW */
               { $$ = makeSTATEMENTblock($2, NULL); }
;

plugs : plug 
        { $$ = $1; }
      | plugs ',' plug
        { $$ = $3; $$->next = $1; }
;

plug : tIDENTIFIER '=' exp
        { $$ = makePLUG($1, $3); }
;

inputs : /* empty */ 
         { $$ = NULL; }
       | neinputs
         { $$ = $1; }
;

neinputs : input 
           { $$ = $1; }
         | neinputs ',' input
           { $$ = $3; $$->next = $1; }
;

input : lvalue '=' tIDENTIFIER
     { $$ = makeINPUT($1, $3); } 
;

exp : lvalue
     { $$ = makeEXPlvalue($1); }
    | lvalue '=' exp
     { $$ = makeEXPassign($1, $3); }
    | exp tEQ exp
     { $$ = makeEXPeq($1, $3); }
    | exp tNEQ exp
     { $$ = makeEXPneq($1, $3); }
    | exp '<' exp
     { $$ = makeEXPlt($1, $3); }
    | exp '>' exp
     { $$ = makeEXPgt($1, $3); }
    | exp tLEQ exp
     { $$ = makeEXPleq($1, $3); }
    | exp tGEQ exp
     { $$ = makeEXPgeq($1, $3); }
    | '!' exp
     { $$ = makeEXPnot($2); }
    | '-' exp %prec tUMINUS
     { $$ = makeEXPuminus($2); }
    | exp '+' exp
     { $$ = makeEXPplus($1, $3); }
    | exp '-' exp
     { $$ = makeEXPminus($1, $3); }
    | exp '*' exp
     { $$ = makeEXPtimes($1, $3); }
    | exp '/' exp
     { $$ = makeEXPdiv($1, $3); }
    | exp '%' exp
     { $$ = makeEXPmod($1, $3); }
    | exp tAND exp
     { $$ = makeEXPand($1, $3); } 
    | exp tOR exp
     { $$ = makeEXPor($1, $3); }
    | exp tCOMBINE exp
     { $$ = makeEXPcombine($1, $3); }
    | exp tKEEP tIDENTIFIER
     { $$ = makeEXPkeep($1, makeID($3)); }
    | exp tKEEP '(' identifiers ')' /* NEW */
     { $$ = makeEXPkeep($1, $4); }
    | exp tDISCARD tIDENTIFIER         /* NEW */
     { $$ = makeEXPdiscard($1, makeID($3)); }
    | exp tDISCARD '(' identifiers ')' /* NEW */
     { $$ = makeEXPdiscard($1, $4); }
    | tIDENTIFIER '(' exps ')'
     { $$ = makeEXPcall($1, $3); }
    | tINTCONST
     { $$ = makeEXPintconst($1); }
    | tBOOLCONST
     { $$ = makeEXPboolconst($1); }
    | tSTRINGCONST
     { $$ = makeEXPstringconst($1); }
    | tTUPLE '{' fieldvalues '}'
     { $$ = makeEXPtuple($3); }
    | '(' exp ')'                  /* NEW */
     { $$ = $2; }
;
exps : /* empty */ 
       { $$ = NULL; }
     | neexps
       { $$ = $1; }
;
neexps : exp 
         { $$ = $1; }
       | neexps ',' exp
         { $$ = $3; $$->next = $1; }
;
lvalue : tIDENTIFIER 
       | tIDENTIFIER '.' tIDENTIFIER
         { $$ = (char*)malloc(strlen($1) + strlen($3) + 2);
           strcpy($$, $1);
           strcat($$, ".");
           strcat($$, $3);
         }  
;
fieldvalues : /* empty */ 
              { $$ = NULL; }
            | nefieldvalues
              { $$ = $1; }
;
nefieldvalues : fieldvalue 
                { $$ = $1; }
              | fieldvalues ',' fieldvalue
                { $$ = $3; $$->next = $1; }
;
fieldvalue : tIDENTIFIER '=' exp
              { $$ = makeFIELDVALUE($1, $3); }
;
