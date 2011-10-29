#ifndef __error_h
#define __error_h

void yyerror(char *s);

void reportStrError(const char *msg, char *name, int lineno);

void reportError(const char *msg, int lineno);

#endif
