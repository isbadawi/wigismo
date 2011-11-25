#include"resource.h"
#include<stdlib.h>

int _id = 1;
int nextid() { return _id++; }

void resSTATEMENT(STATEMENT *s)
{
    if (s == NULL)
        return;
    switch (s->kind)
    {
        case returnK:
        case skipK:
        case expK:
        case exitK:
        case showK:
            return;
        case sequenceK:
            resSTATEMENT(s->val.sequenceS.first);
            resSTATEMENT(s->val.sequenceS.second);
            break;
        case blockK:
            resSTATEMENT(s->val.blockS.body);
            break;
        case ifK:
            s->val.ifS.afterid = nextid();
            resSTATEMENT(s->val.ifS.body);
            break;
        case ifelseK:
            resSTATEMENT(s->val.ifelseS.thenpart);
            s->val.ifelseS.elseid = nextid();
            resSTATEMENT(s->val.ifelseS.elsepart);
            s->val.ifelseS.afterelseid = nextid();
            break;
        case whileK:
            s->val.whileS.whileid = nextid();
            resSTATEMENT(s->val.whileS.body);
            s->val.whileS.afterwhileid = nextid();
            break;
    }
}

void resSESSION(SESSION *s)
{
    resSTATEMENT(s->statements);
}
