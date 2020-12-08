#include <utility>
#include "action.h"

Assume::Assume(pExprTree arg_expr): expr(arg_expr) {}
void Assume::maintain_vc(pExprTree &vc)
{
    if(vc) vc = pExprTree(new BinaryLogic(BinaryLogic::imply, expr, vc));
}

Assert::Assert(pExprTree arg_expr): expr(arg_expr) {}
void Assert::maintain_vc(pExprTree &vc)
{
    if (vc) vc = pExprTree(new BinaryLogic(BinaryLogic::and, expr, vc));
    else vc = expr;
}

Assign::Assign(pVariable arg_lhs, pExprTree arg_rhs):
    lhs(arg_lhs), rhs(arg_rhs) {}
void Assign::maintain_vc(pExprTree &vc)
{
    if (vc)
    {
        pExprTree new_vc = vc->replace(lhs, rhs);
        if (new_vc) vc = new_vc;
    }
}

Decrease::Decrease(std::vector<pExprTree> arg_ranking):
    ranking(std::move(arg_ranking)) {}
void Decrease::maintain_vc(pExprTree &vc)
{
    pExprTree zero = new IntLiteral(0);
    for (pExprTree ranking_f : ranking)
        if (vc) vc = pExprTree(new BinaryLogic(BinaryLogic::and, vc, pExprTree(new BinaryCmp(BinaryCmp::ge, ranking_f, zero))));
        else vc = pExprTree(new BinaryCmp(BinaryCmp::ge, ranking_f, zero));
}