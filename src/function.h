#pragma once

#include <vector>
#include <string>
#include "block.h"
#include "ns.h"
#include "action.h"
#include "exprtree.h"

class Function
{
    std::string name;
    std::vector<Namespace*> namespaces;
    std::vector<Block*> blocks;
    std::vector<pExprTree> precond, postcond;
    std::vector<pVariable> params;
public:
    Function(const Function &y) = delete;
    Function& operator=(const Function &y) = delete;
    Function(std::string arg_name);
    ~Function();

    Namespace* new_ns(Namespace *parent_ns);
    Block* new_block(Namespace *ns);
    Block* cur_block();
    Namespace* cur_ns();
    void add_action(Action *action);
    void verify();
    void add_precond(pExprTree cond);
    void add_postcond(pExprTree cond);
    void add_param(pVariable param_v);
    const std::vector<pExprTree>& get_precond();
    const std::vector<pExprTree>& get_postcond();
    const std::vector<pVariable>& get_params();
    Namespace* root_ns();
};