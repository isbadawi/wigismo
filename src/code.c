#include "code.h"
#include "tree.h"
#include <stdio.h>

FILE *out;


void print_indent();
void print_header(char*);
void codeHTML(HTML*);
ID *gaps_in(HTMLBODY*);
void print_gaps(ID*);

void codeSERVICE(SERVICE *service, FILE *_out)
{
      out = _out;
      print_header(service->name);
      codeHTML(service->htmls);
}

void print_indent()
{
    fprintf(out, "    ");
}

void print_header(char *service)
{
    fprintf(out, "#!/usr/bin/python\n"
                 "import sys\n"
                 "import runtime\n");

    fprintf(out, "import cgitb\ncgitb.enable()\n\n");

    fprintf(out, "g = runtime.Store('%s_globals.pck')\n\n", service);
}

void codeHTML(HTML *html)
{
    if (html == NULL)
        return;

    codeHTML(html->next);
    
    fprintf(out, "def output_%s(", html->name);
    print_gaps(gaps_in(html->htmlbodies));
    fprintf(out, "):\n");
/*    codeHTMLBODY(html->htmlbodies); */
}

ID *gaps_in(HTMLBODY *body)
{
    if (body == NULL)
        return NULL;
    
    if (body->kind == gapK)
    {
        ID *id = makeID(body->val.gapH);
        id->next = gaps_in(body->next);
        return id;
    }
    return gaps_in(body->next);
}

void print_gaps(ID *id)
{
    if (id == NULL)
        return;
    print_gaps(id->next);
    if (id->next != NULL)
        fprintf(out, ", ");
    fprintf(out, "%s", id->name);
}


