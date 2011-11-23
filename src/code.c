#include "code.h"
#include "tree.h"
#include <stdio.h>
#include<string.h>

FILE *out;


void print_indent();
void print_header(char*);
void codeHTML(HTML*);
ID *gaps_in(HTMLBODY*);
void print_gaps(ID*);
void print_attributes(ATTRIBUTE*);
void codeHTMLBODY(HTMLBODY*);

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
    codeHTMLBODY(html->htmlbodies);
}

void codeHTMLBODY(HTMLBODY *htmlbody)
{
    if (htmlbody == NULL)
        return;

    codeHTMLBODY(htmlbody->next);
    
    print_indent();
    fprintf(out, "print '");
    switch (htmlbody->kind)
    {
        case opentagK:
        case selfclosingK:
            fprintf(out, "<%s ", htmlbody->val.tagH.name);
            print_attributes(htmlbody->val.tagH.attributes);
            if (htmlbody->kind == selfclosingK)
                fprintf(out, "/");
            fprintf(out, ">");
            break;
        case closetagK:
            fprintf(out, "</%s>", htmlbody->val.tagH.name);
            break;
        case gapK:
            fprintf(out, "%%s");                    
            break;
        case whateverK:
        {
            int len = strlen(htmlbody->val.whateverH);
            int i;
            for (i = 0; i < len; ++i)
            {
                if (i == len - 1 && htmlbody->val.whateverH[i] == ' ')
                    break;
                if (htmlbody->val.whateverH[i] == '\n')
                    fprintf(out, "%s", "\\n");
                else
                    fprintf(out, "%c", htmlbody->val.whateverH[i]);
            }
            break;
        }
        case metaK:
            fprintf(out, "<!--%s-->", htmlbody->val.metaH);
            break;
        case inputK:
            fprintf(out, "<input ");
            print_attributes(htmlbody->val.inputH.attributes);
            fprintf(out, ">");
            break;
        case selectK:
            fprintf(out, "<select ");
            print_attributes(htmlbody->val.selectH.attributes);
            fprintf(out, ">',\n");
            codeHTMLBODY(htmlbody->val.selectH.htmlbodies);
            fprintf(out, "print '</select>',\n");
            break;
    }
    if (htmlbody->kind != selectK)
    {
        fprintf(out, "'");
        if (htmlbody->kind == gapK)
            fprintf(out, " %% %s", htmlbody->val.gapH);
        fprintf(out, ",\n");
    }
}

void print_attributes(ATTRIBUTE *a)
{
    if (a == NULL)
        return;

    print_attributes(a->next);
    
    if (a->value == NULL)
        fprintf(out, "%s ", a->name);
    else
        fprintf(out, "%s=\"%s\" ", a->name, a->value);
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


