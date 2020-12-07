#pragma once

#include "block.h"
#include "ns.h"
#include "action.h"
#include "exprtree.h"

class Function
{
    std::vector<Namespace*> namespaces;
    std::vector<Block*> blocks;
    std::vector<pExprTree> precond, postcond;
public:
    Function(const Function &y) = delete;
    Function& operator=(const Function &y) = delete;
    ~Function();

    Namespace* new_ns(Namespace* parent_ns);
    Block* new_block(Namespace* ns);
    Block* cur_block();
    Namespace* cur_ns();
    void add_action(Action *action);
    void verify();
    void add_precond(pExprTree cond);
    void add_postcond(pExprTree cond);
    std::vector<pExprTree>& get_postcond();
};