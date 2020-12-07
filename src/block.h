#pragma once

#include "action.h"
#include "ns.h"

struct Block
{
    Namespace *ns;
    std::vector<Action*> actions;
    bool terminate, conditional;
    pExprTree condition;
    Block *next, *alter;
    
    Block(Namespace *arg_ns);
    ~Block();
};