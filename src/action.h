#pragma once

#include "exprtree.h"

class Action
{
public:
    virtual void maintain_vc(pExprTree &vc) = 0;
    virtual ~Action() = default;
};

class Assume : public Action
{
    pExprTree expr;
public:
    Assume(pExprTree arg_expr);
    virtual void maintain_vc(pExprTree &vc) override;
};

class Assert : public Action
{
    pExprTree expr;
public:
    Assert(pExprTree arg_expr);
    virtual void maintain_vc(pExprTree &vc) override;
};

class Assign : public Action
{
    pVariable lhs;
    pExprTree rhs;
public:
    Assign(pVariable arg_lhs, pExprTree arg_rhs);
    virtual void maintain_vc(pExprTree &vc) override;
};

class Decrease : public Action
{
    std::vector<pExprTree> ranking;
public:
    Decrease(std::vector<pExprTree> arg_ranking);
    virtual void maintain_vc(pExprTree &vc) override; // assert ranking function >= 0
    const std::vector<pExprTree>& rank_function();
};