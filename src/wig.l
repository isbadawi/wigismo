
%{
#include "y.tab.h"
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "error.h"

extern int lineno;
int init = 0;
%}

%s WIG HTML HTMLINPUT HTMLRHS HTMLTAG GAP TYPERHS

%%
	if (!init) {
		init = 1;
		BEGIN WIG;
	}
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
<WIG>\<html\>			 BEGIN HTML; return tHTMLOPEN;
<HTML>\<\/html\> 		 BEGIN WIG;  return tHTMLCLOSE;
<HTML>\<				 BEGIN HTMLTAG; return '<';
<HTML>\<\/				 BEGIN HTMLTAG; return tOPENINGTAG;
<HTML>"\<["			     BEGIN GAP; return tOPENINGGAP; 
<HTMLTAG>\>		   	     BEGIN HTML; return '>';
<HTMLTAG>\/\>		     BEGIN HTML; return tCLOSINGTAG;
<HTMLTAG>input			 BEGIN HTMLINPUT; return tINPUT;
<HTMLTAG>select			 return tSELECT;
<HTMLINPUT>name   	 	 BEGIN HTMLTAG; return tNAME; 
<HTMLINPUT>type/=	     BEGIN TYPERHS; return tTYPE; 
<TYPERHS>"="			 ;
<TYPERHS>text		     BEGIN HTMLINPUT; return tTEXT; 
<TYPERHS>radio	         BEGIN HTMLINPUT; return tRADIO; 
<TYPERHS>"\"text\""		 BEGIN HTMLINPUT; return tTEXT; 
<TYPERHS>"\"radio\""	 BEGIN HTMLINPUT; return tRADIO; 
<HTMLTAG>"="             BEGIN HTMLRHS; return '=';
<HTMLRHS>[a-zA-Z0-9_]+	         { BEGIN HTMLTAG; 
						 yylval.stringconst = (char*)malloc(yyleng+1);
                         sprintf(yylval.stringconst,"%s",yytext); 
                         return tIDENTIFIER; }   
<HTMLRHS>\"([^\"])*\"    { BEGIN HTMLTAG; 
                         yylval.stringconst = (char*)malloc(yyleng-1);
                         yytext[yyleng-1] = '\0';
                         sprintf(yylval.stringconst,"%s",yytext+1);
                         return tSTRINGCONST; }   
<GAP>"]\>"				 BEGIN HTML; return tCLOSINGGAP;
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
\"([^\"])*\"           { yylval.stringconst = (char*)malloc(yyleng-1);
                         yytext[yyleng-1] = '\0';
                         sprintf(yylval.stringconst,"%s",yytext+1);
                         return tSTRINGCONST; }
<HTML>\<!--[^-]*--\>	{yylval.stringconst = (char*)malloc(yyleng-6);
						 yytext[yyleng-3] = '\0';
                         sprintf(yylval.stringconst, "%s", yytext + 4);
						 int i;
						 for (i = 4; i < yyleng - 3; i++)
							 if (yytext[i] == '\n') lineno++;
						 return tMETA; }
<HTML>[^\<\>]*	       { yylval.stringconst = (char*)malloc(yyleng+1);
                         sprintf(yylval.stringconst, "%s", yytext);
						 int i;
						 for (i = 0; i < yyleng; i++)
							 if (yytext[i] == '\n') lineno++;
						 return tWHATEVER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.stringconst = (char*)malloc(yyleng+1);
                         sprintf(yylval.stringconst,"%s",yytext); 
                         return tIDENTIFIER; }
.                        return tERROR;
%%
