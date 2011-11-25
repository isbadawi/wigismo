#include "code.h"
#include "tree.h"
#include "type.h"
#include "resource.h"
#include <stdio.h>
#include<string.h>

FILE *out;
static int _indent = 0;
static int _id = 0;
static char *session = NULL;


static char* _ops[30];
static char* _defaults[10];

void init_ops_and_defaults(void)
{
    _ops[orK] = "or";
    _ops[andK] = "and";
    _ops[eqK] = "==";
    _ops[ltK] = "<";
    _ops[gtK] = ">";
    _ops[leqK] = "<=";
    _ops[geqK] = ">=";
    _ops[neqK] = "!=";
    _ops[plusK] = "+";
    _ops[minusK] = "-";
    _ops[timesK] = "*";
    _ops[divK] = "/";
    _ops[modK] = "%";
    _defaults[intK] = "0";
    _defaults[boolK] = "False";
    _defaults[stringK] = "''";
    _defaults[tupleK] = "{}";
}

void indent()
{
    _indent += 4;
}

void dedent() 
{
    _indent -= 4;
}

void print_indent()
{
    int i;
    for (i = 0; i < _indent; i++)
        fprintf(out, " ");
}

void print_header(char*);
void print_footer(SESSION*);
void codeHTML(HTML*);
ID *gaps_in(HTMLBODY*);
void print_gaps(ID*);
void print_attributes(ATTRIBUTE*);
void codeHTMLBODY(HTMLBODY*);
void codeFUNCTION(FUNCTION*);
void codeARGUMENT(ARGUMENT*);
void codeSTATEMENT(STATEMENT*);
void codeEXP(EXP*);
void codeFIELDVALUE(FIELDVALUE*);
void codeID(ID*);
void codeSESSION(SESSION*);

void codeSERVICE(SERVICE *service, FILE *_out)
{
      out = _out;
      init_ops_and_defaults();
      print_header(service->name);
      codeHTML(service->htmls);
      codeFUNCTION(service->functions);
      codeSESSION(service->sessions);
      print_footer(service->sessions);
}

void print_footer(SESSION *s)
{
    SESSION* t = s;
    fprintf(out, "def wigismo_restart():\n");
    fprintf(out, "    state = wigismo.Store(wigismo.sessionid)\n");
    fprintf(out, "    l.update(state.get('locals'))\n");
    fprintf(out, "    globals()[state.get('start')](wigismo.sessionid)\n\n");

    while (t != NULL)
    {
        fprintf(out, "if wigismo.sessionid == '%s':\n", s->name);
        fprintf(out, "    session_%s('%s$' + wigismo.random_string(20))\n", s->name, s->name);
        fprintf(out, "    sys.exit(0)\n");
        fprintf(out, "if wigismo.sessionid.startswith('%s$'):\n", s->name);
        fprintf(out, "    wigismo_restart();\n");
        fprintf(out, "    sys.exit(0)\n");
        t = t->next;
    }
    fprintf(out, "print 'Content-type: text/html'\n");
    fprintf(out, "print\n");
    fprintf(out, "print '<title>Illegal request</title>'\n");
    fprintf(out, "print '<p>Try one of these:</p>'\n");
    fprintf(out, "print '<ul>'\n");
    while (s != NULL)
    {
        fprintf(out, "print '<li><a href=\"?%s\">%s</a></li>'\n", s->name, s->name);
        s = s->next;
    }
    fprintf(out, "print '</ul>'\n");
    fprintf(out, "sys.exit(0)\n");
}

void print_header(char *service)
{
    fprintf(out, "#!/usr/bin/python\n"
                 "import sys\n"
                 "import wigismo\n");

    fprintf(out, "import cgitb\ncgitb.enable()\n\n");

    fprintf(out, "g = wigismo.Store('%s_globals.pck')\n", service);
    fprintf(out, "l = {}\n\n");
}

void codeHTML(HTML *html)
{
    if (html == NULL)
        return;

    codeHTML(html->next);
    
    fprintf(out, "def output_%s(", html->name);
    print_gaps(gaps_in(html->htmlbodies));
    fprintf(out, "):\n");
    indent();
    codeHTMLBODY(html->htmlbodies);
    dedent();
    fprintf(out, "\n");
}

void codeHTMLBODY(HTMLBODY *htmlbody)
{
    if (htmlbody == NULL)
        return;

    codeHTMLBODY(htmlbody->next);
    
    print_indent();
    fprintf(out, "sys.stdout.write('");
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
                else if (htmlbody->val.whateverH[i] == '\\')
                    fprintf(out, "%s", "\\\\");
                else if (htmlbody->val.whateverH[i] == '\'')
                    fprintf(out, "%s", "\\\'");
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
            fprintf(out, ">')\n");
            codeHTMLBODY(htmlbody->val.selectH.htmlbodies);
            fprintf(out, "sys.stdout.write('</select>')\n");
            break;
    }
    if (htmlbody->kind != selectK)
    {
        fprintf(out, "'");
        if (htmlbody->kind == gapK)
            fprintf(out, " %% %s", htmlbody->val.gapH);
        fprintf(out, ")\n");
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
    fprintf(out, "%s=''", id->name);
}

void codeFUNCTION(FUNCTION *f)
{
    if (f == NULL)
        return;
    codeFUNCTION(f->next);

    fprintf(out, "def %s(", f->name);
    codeARGUMENT(f->arguments);
    fprintf(out, "):\n");
    indent();
    codeSTATEMENT(f->statements);
    dedent();
    fprintf(out, "\n");
}

void codeARGUMENT(ARGUMENT *a)
{
    if (a == NULL)
        return;
    if (a->next != NULL)
        fprintf(out, ", ");
    fprintf(out, "%s", a->name);
}

void codePLUG(PLUG *p)
{
    if (p == NULL)
        return;
    codePLUG(p->next);

    if (p->next != NULL)
        fprintf(out, ", ");

    fprintf(out, "%s=", p->name);
    codeEXP(p->exp);
}

int has_show(STATEMENT *s)
{
    if (s == NULL)
        return 0;

    switch (s->kind)
    {
        case skipK:
        case exitK:
        case returnK:
        case expK:
            return 0;
        case ifK:
            return has_show(s->val.ifS.body);
        case ifelseK:
            return has_show(s->val.ifelseS.thenpart) ||
                   has_show(s->val.ifelseS.elsepart);
        case whileK:
            return has_show(s->val.whileS.body);
        case blockK:
            return has_show(s->val.blockS.body);
        case sequenceK:
            return has_show(s->val.sequenceS.first) ||
                   has_show(s->val.sequenceS.second);
        case showK:
            return 1;
    }

}

void find_shows(STATEMENT *s)
{
    if (s == NULL)
        return;
    switch (s->kind)
    {
        case skipK:
        case showK:
        case exitK:
        case expK:
        case returnK:
            break;
        case sequenceK:
            find_shows(s->val.sequenceS.first);
            find_shows(s->val.sequenceS.second);
            break;
        case blockK:
            find_shows(s->val.blockS.body);
            break;
        case ifK:
            s->val.ifS.has_show = has_show(s->val.ifS.body);
            find_shows(s->val.ifS.body);
            break;
        case ifelseK:
            s->val.ifelseS.then_has_show = has_show(s->val.ifelseS.thenpart);
            s->val.ifelseS.else_has_show = has_show(s->val.ifelseS.elsepart);
            find_shows(s->val.ifelseS.thenpart);
            find_shows(s->val.ifelseS.elsepart);
            break;
        case whileK:
            s->val.whileS.has_show = has_show(s->val.whileS.body);
            find_shows(s->val.whileS.body);
            break;
    }
}

void codeSTATEMENT(STATEMENT *s)
{
    if (s == NULL)
        return;

    if (s->kind != sequenceK && s->kind != blockK)
        print_indent();

    switch (s->kind)
    {
        case skipK:
            fprintf(out, "pass");
            fprintf(out, "\n");
            break;
        case sequenceK:
            codeSTATEMENT(s->val.sequenceS.first);
            codeSTATEMENT(s->val.sequenceS.second);
            break;
        case showK:
            fprintf(out, "wigismo.output(sessionid, lambda: output_%s(",
                         s->val.exitS.document->name);
            codePLUG(s->val.exitS.document->plugs);
            fprintf(out, "))\n");
            print_indent();
            fprintf(out, "state = wigismo.Store(sessionid)\n");
            print_indent();
            fprintf(out, "l.set('locals', locals)\n");
            print_indent();
            int id = _id++;
            fprintf(out, "l.set('start', 'session_%s_show_%d')\n", session, id);
            print_indent();
            fprintf(out, "sys.exit(0)\n\n");
            _indent = 0;
            fprintf(out, "def session_%s_show_%d(sessionid):\n", session, id);
            indent();
            break;
        case exitK:
            fprintf(out, "wigismo.output(sessionid, lambda: output_%s(",
                         s->val.exitS.document->name);
            codePLUG(s->val.exitS.document->plugs);
            fprintf(out, "), exit=True)\n");
            print_indent();
            fprintf(out, "sys.exit(0)");
            fprintf(out, "\n\n");
            break;
        case returnK:
            fprintf(out, "return ");
            codeEXP(s->val.returnS);
            fprintf(out, "\n");
            break;
        case blockK:
            codeSTATEMENT(s->val.blockS.body);
            break;
        case ifK:
            if (s->val.ifS.has_show == 0)
            {
                fprintf(out, "if ");
                codeEXP(s->val.ifS.condition);
                fprintf(out, ":\n");
                indent();
                codeSTATEMENT(s->val.ifS.body);
                dedent();
            }
            else
            {
                int id = s->val.ifS.afterid;
                fprintf(out, "if not (");
                codeEXP(s->val.ifS.condition);
                fprintf(out, "):\n");
                indent();
                print_indent();
                fprintf(out, "session_%s_%d(sessionid)\n", session, id);
                dedent();
                codeSTATEMENT(s->val.ifS.body);
                print_indent();
                fprintf(out, "session_%s_%d(sessionid)\n\n", session, id);
                _indent = 0;
                fprintf(out, "def session_%s_%d(sessionid):\n", session, id);
                indent();
            }
            break;
        case ifelseK:
            if (s->val.ifelseS.then_has_show == 0 &&
                s->val.ifelseS.else_has_show == 0)
            {
                fprintf(out, "if ");
                codeEXP(s->val.ifelseS.condition);
                fprintf(out, ":\n");
                indent();
                codeSTATEMENT(s->val.ifelseS.thenpart);
                fprintf(out, "else:\n");
                codeSTATEMENT(s->val.ifelseS.elsepart);
                dedent();
            }
            break;
        case whileK:
            if (s->val.whileS.has_show == 0)
            {
                fprintf(out, "while ");
                codeEXP(s->val.whileS.condition);
                fprintf(out, ":\n");
                indent();
                codeSTATEMENT(s->val.whileS.body);
                dedent();
                break;
            }
        case expK:
            codeEXP(s->val.expS);
            fprintf(out, "\n");
            break;
    }
}

void codeEXP(EXP *e)
{
    if (e == NULL)
        return;
    codeEXP(e->next);
    if (e->next != NULL)
        fprintf(out, ", ");


    switch (e->kind)
    {
        case idK:
            if (e->val.idE.idsym->kind == variableSym)
            {
                VARIABLE *v = e->val.idE.idsym->val.variableS;
                fprintf(out, "%s.get('%s_%d', %s)", 
                             v->global ? "g" : "l",
                             v->name, v->id, _defaults[v->type->kind]);
            }
            else
                fprintf(out, "%s", e->val.idE.idsym->val.argumentS->name);
            break;
        case idtupleK:
            if (e->val.idtupleE.idsym->kind == variableSym)
            {
                VARIABLE *v = e->val.idtupleE.idsym->val.variableS;
                fprintf(out, "%s.get('%s_%d', %s).get('%s', %s)", 
                             v->global ? "g" : "l",
                             v->name, v->id, _defaults[v->type->kind],
                             e->val.idtupleE.field, 
                             _defaults[typeSchemaVar(e->val.idtupleE.schema, e->val.idtupleE.field)->kind]);  
            }
            else
                fprintf(out, "%s.get('%s', %s)", 
                             e->val.idtupleE.idsym->val.argumentS->name, 
                             e->val.idtupleE.field,
                            _defaults[typeSchemaVar(e->val.idtupleE.schema, e->val.idtupleE.field)->kind]);
            break;
        case assignK:
            if (e->val.assignE.leftsym->kind == variableSym)
            {
                VARIABLE *v = e->val.assignE.leftsym->val.variableS;
                if (v->global)
                    fprintf(out, "g.set('%s_%d', ", v->name, v->id);
                else
                    fprintf(out, "wigismo.set(l, '%s_%d', ", v->name,v->id);
                codeEXP(e->val.assignE.right);
                fprintf(out, ")");
            }
            else
            {
                fprintf(out, "%s = ", e->val.assignE.leftsym->val.argumentS->name);
                codeEXP(e->val.assignE.right);
            }
            break;
        case assigntupleK:
            if (e->val.assigntupleE.leftsym->kind == variableSym)
            {
                VARIABLE *v = e->val.assigntupleE.leftsym->val.variableS;
                if (v->global)
                    fprintf(out, "g.set_key('%s_%d', '%s', ",
                                 v->name, v->id, e->val.assigntupleE.field);
                else
                    fprintf(out, "wigismo.set_key(l, '%s_%d', '%s', ",
                                 v->name, v->id, e->val.assigntupleE.field);
                codeEXP(e->val.assigntupleE.right);
                fprintf(out, ")");
            }
            else
            {
                fprintf(out, "%s['%s'] = ", e->val.assigntupleE.name, e->val.assigntupleE.field);
                codeEXP(e->val.assigntupleE.right);
            }
            break;
        case orK:
        case andK:
        case eqK:
        case ltK:
        case gtK:
        case leqK:
        case geqK:
        case neqK:
        case plusK:
        case minusK:
        case timesK:
        case divK:
        case modK:
            codeEXP(e->val.binaryE.left);
            fprintf(out, " %s ", _ops[e->kind]);
            codeEXP(e->val.binaryE.right);
            break;
        case notK:
            fprintf(out, "not ");
            codeEXP(e->val.unaryE);
            break;
        case uminusK:
            fprintf(out, "-");
            codeEXP(e->val.unaryE);
        case combineK:
            fprintf(out, "wigismo.tuple_combine(");
            codeEXP(e->val.binaryE.left);
            fprintf(out, ", ");
            codeEXP(e->val.binaryE.right);
            fprintf(out, ")");
            break;
        case keepK:
            fprintf(out, "wigismo.tuple_keep(");
            codeEXP(e->val.keepE.left);
            fprintf(out, ", ");
            codeID(e->val.keepE.ids);
            fprintf(out, ")");
            break;
        case discardK:
            fprintf(out, "wigismo.tuple_discard(");
            codeEXP(e->val.discardE.left);
            fprintf(out, ", ");
            codeID(e->val.discardE.ids);
            fprintf(out, ")");
            break;
        case callK:
            fprintf(out, "%s(", e->val.callE.name);
            codeEXP(e->val.callE.args);
            fprintf(out, ")");
            break;
        case intconstK:
            fprintf(out, "%d", e->val.intconstE);
            break;
        case boolconstK:
            fprintf(out, "%s", e->val.boolconstE ? "True" : "False");
            break;
        case stringconstK:
            fprintf(out, "\"%s\"", e->val.stringconstE);
            break;
        case tupleconstK:
            fprintf(out, "{");
            codeFIELDVALUE(e->val.tupleE.fieldvalues);
            fprintf(out, "}");
            break;
    }
}

void codeFIELDVALUE(FIELDVALUE *fv)
{
    if (fv == NULL)
        return;

    codeFIELDVALUE(fv->next);
    if (fv->next != NULL)
        fprintf(out, ", ");
    fprintf(out, "'%s': ", fv->name);
    codeEXP(fv->exp);
}

void codeID(ID *id)
{
    if (id == NULL)
        return;
    codeID(id->next);
    if (id->next != NULL)
        fprintf(out, ", ");
    fprintf(out, "'%s'", id->name);
}

void codeSESSION(SESSION *s)
{
    if (s == NULL)
        return;
    codeSESSION(s->next);

    fprintf(out, "def session_%s(sessionid):\n", s->name);
    indent();
    session = s->name;
    resSESSION(s);
    find_shows(s->statements);
    codeSTATEMENT(s->statements);
    dedent();
}
