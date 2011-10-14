
%{
#include "y.tab.h"
#include <stdio.h>
#include <string.h>
#include "tree.h"

extern int lineno;
%}

%Start WIG HTML HTMLTAG GAP

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
		    yyerror("EOF in comment\n"); 
			break;
		}
	}
}

<WIG>service		     return tSERVICE;
<WIG>session			 return tSESSION;
<WIG>receive			 return tRECEIVE;
<WIG>plug				 return tPLUG;
<WIG>string				 return tSTRING;
<WIG>html			     return tHTML;
<WIG>show				 return tSHOW;
<WIG>exit				 return tEXIT;
<WIG>schema				 return tSCHEMA;
<WIG>tuple				 return tTUPLE;
<WIG>bool                return tBOOL;
<WIG>const               return tCONST;
<WIG>else                return tELSE;
<WIG>if                  return tIF;
<WIG>int                 return tINT;
<WIG>return              return tRETURN;
<WIG>void                return tVOID;
<WIG>while               return tWHILE;
<WIG>"<html>"			 { BEGIN HTML; return tHTMLOPEN; }
<HTML>"</html>"			 { BEGIN WIG; return tHTMLCLOSE; }
<HTML>"<"				 { BEGIN HTMLTAG; return '<'; }
<HTML>"</"				 { BEGIN HTMLTAG; return tOPENINGTAG; }
<HTMLTAG>">"			 { BEGIN HTML; return '>'; }
<HTMLTAG>"/>"		     { BEGIN HTML; return tCLOSINGTAG; }
<HTMLTAG>input			 { return tINPUT; }
<HTMLTAG>select			 { return tSELECT; }
<HTMLTAG>name	  		 { return tNAME; }
<HTMLTAG>type	         { return tTYPE; }
<HTMLTAG>text			 { return tTEXT; }
<HTMLTAG>radio		     { return tRADIO; }
<HTMLTAG>"<["			 { BEGIN GAP; return tOPENINGGAP; }
<GAP>"]>"				 { BEGIN HTMLTAG; return tCLOSINGGAP; }
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
"\\+"					 return tKEEP;
"\\-"					 return tDISCARD;
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
