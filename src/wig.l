
%{
#include "y.tab.h"
#include <stdio.h>
#include <string.h>
#include "tree.h"
#include "error.h"

extern int lineno;
int init = 0;
int i;
%}

%option nounput
%s WIG HTMLINPUT HTMLTAG GAP
%x TYPERHS HTMLRHS INPUTRHS HTML

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
<WIG>until				 return tUNTIL;
<WIG>for				 return tFOR;
<WIG>\<html\>			 BEGIN HTML; return tHTMLOPEN;
<HTML>\<\/html\> 		 BEGIN WIG;  return tHTMLCLOSE;
<HTML>\<				 BEGIN HTMLTAG; return '<';
<HTML>\<\/				 BEGIN HTMLTAG; return tOPENINGTAG;
<HTML>"\<["			     BEGIN GAP; return tOPENINGGAP; 
<HTMLTAG,HTMLINPUT>\>		   	     BEGIN HTML; return '>';
<HTMLTAG>\/\>		     BEGIN HTML; return tCLOSINGTAG;
<HTMLTAG>input			 BEGIN HTMLINPUT; return tINPUT;
<HTMLTAG>select			 return tSELECT;
<HTMLINPUT>name   	 	 BEGIN INPUTRHS; return tNAME; 
<HTMLINPUT>type/=	     BEGIN TYPERHS; return tTYPE; 
<HTMLINPUT>"="			 BEGIN INPUTRHS; return '=';
<HTMLINPUT>[a-zA-Z_][a-zA-Z0-9_]* { BEGIN INPUTRHS; yylval.stringconst = (char*)malloc(yyleng+1);
                         sprintf(yylval.stringconst,"%s",yytext); 
                         return tIDENTIFIER; }


<TYPERHS,INPUTRHS>"="			 return '=';
<TYPERHS>text		     BEGIN HTMLINPUT; yylval.stringconst = strdup("text"); return tTEXT; 
<TYPERHS>radio	         BEGIN HTMLINPUT; yylval.stringconst = strdup("radio"); return tRADIO; 
<TYPERHS>"\"text\""		 BEGIN HTMLINPUT; yylval.stringconst = strdup("text"); return tTEXT; 
<TYPERHS>"\"radio\""	 BEGIN HTMLINPUT; yylval.stringconst = strdup("radio"); return tRADIO; 
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
<INPUTRHS>\>			 BEGIN HTML; return '>';
<INPUTRHS>\/\>			 BEGIN HTML; return tCLOSINGTAG;
<INPUTRHS>[a-zA-Z0-9_]+	         { BEGIN HTMLINPUT; 
						 yylval.stringconst = (char*)malloc(yyleng+1);
                         sprintf(yylval.stringconst,"%s",yytext); 
                         return tIDENTIFIER; }   
<INPUTRHS>\"([^\"])*\"    { BEGIN HTMLINPUT; 
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
"++"					 return tINC;
"--"					 return tDEC;
"+="					 return tPLUSEQ;
"-="					 return tMINEQ;
"*="					 return tMULTEQ;
"/="					 return tDIVEQ;
"%="					 return tMODEQ;
"&&="					 return tANDEQ;
"||="					 return tOREQ;
0|([1-9][0-9]*)        { yylval.intconst = atol(yytext);
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
						 for (i = 4; i < yyleng - 3; i++)
							 if (yytext[i] == '\n') lineno++;
						 return tMETA; }
<HTML>[^\<\>]*	       { yylval.stringconst = (char*)malloc(yyleng+1);
                         sprintf(yylval.stringconst, "%s", yytext);
						 for (i = 0; i < yyleng; i++)
							 if (yytext[i] == '\n') lineno++;
						 return tWHATEVER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.stringconst = (char*)malloc(yyleng+1);
                         sprintf(yylval.stringconst,"%s",yytext); 
                         return tIDENTIFIER; }
.                        return tERROR;
%%
