
%{
#include "y.tab.h"
#include <string.h>
#include "tree.h"

extern int lineno;
%}
%%
[ \t]+                   /* ignore */;
\n                       lineno++;
\/\/[^\n]*\n             lineno++;
\/\*					 {
	int c;
    while(1)
	{
		while ((c = input()) != '*' && c != EOF )
			if (c == '\n') lineno++;    /* eat up text of comment */
		if (c == '*')
		{
			while ((c = input()) == '*')
				;
			if (c == '/')
				break;    /* found the end */
		}
	    if ( c == EOF )
		{
		    yyerror("EOF in comment"); 
			break;
		}
	}
}
service					 return tSERVICE;
session					 return tSESSION;
receive					 return tRECEIVE;
plug					 return tPLUG;
string					 return tSTRING;
html			         return tHTML;
show					 return tSHOW;
exit					 return tEXIT;
schema					 return tSCHEMA;
tuple				     return tTUPLE;
bool                     return tBOOL;
const                    return tCONST;
else                     return tELSE;
if                       return tIF;
int                      return tINT;
return                   return tRETURN;
void                     return tVOID;
while                    return tWHILE;
"="                      return '=';
">"                      return '>';
"<"                      return '<';
"!"                      return '!';
"=="                     return tEQ;
"<="                     return tLEQ;
">="                     return tGEQ;
"!="                     return tNEQ;
"&&"                     return tAND;
"||"                     return tOR;
"\+"					 return tKEEP;
"\-"					 return tDISCARD;
"<<"					 return tCOMBINE;
"+"                      return '+';
"-"                      return '-';
"*"                      return '*';
"/"                      return '/';
"%"                      return '%';
"{"                      return '{';
"}"                      return '}';
";"                      return ';';
"("                      return '(';
")"                      return ')';
"["                      return '[';
"]"                      return ']';
","                      return ',';
"."                      return '.';
0|([1-9][0-9]*)        { yylval.intconst = atoi(yytext);
                         return tINTCONST; }
true                   { yylval.boolconst = 1;
                         return tBOOLCONST; }
false                  { yylval.boolconst = 0;
                         return tBOOLCONST; }
\"([^\"])*\"           { yylval.stringconst = (char*)malloc(strlen(yytext)-1);
                         yytext[strlen(yytext)-1] = '\0';
                         sprintf(yylval.stringconst,"%s",yytext+1);
                         return tSTRINGCONST; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.stringconst = (char *)malloc(strlen(yytext)+1);
                         sprintf(yylval.stringconst,"%s",yytext); 
                         return tIDENTIFIER; }
.                        return tERROR;
%%
