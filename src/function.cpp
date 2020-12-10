#include <stdexcept>
#include "function.h"
#include "utils.h"

Function::Function(std::string arg_name): name(arg_name)
{
    Namespace *root_ns = new Namespace(nullptr);
    namespaces.push_back(root_ns);
    blocks.push_back(new Block(root_ns));
}

Function::~Function()
{
    for (Block *block : blocks) delete block;
    for (Namespace *ns : namespaces) delete ns;
}

Namespace* Function::new_ns(Namespace *parent_ns)
{
    Namespace *cur = new Namespace(parent_ns);
    namespaces.push_back(cur);
    return cur;
}

Block* Function::new_block(Namespace *ns)
{
    Block *cur = new Block(ns);
    blocks.push_back(cur);
    return cur;
}

Block* Function::cur_block()
{
    return *std::prev(blocks.end());
}

Namespace* Function::cur_ns()
{
    return cur_block()->ns;
}

void Function::add_action(Action *action)
{
    cur_block()->actions.push_back(action);
}

void Function::verify()
{
    // TODO: dfs to generate basic path
    std::vector<Action*> basic_path;
    for (Block *block : blocks)
        for (Action *action : block->actions)
            basic_path.push_back(action);
    if (!verify_basic_path(basic_path))
        throw VerificationError("Verification failed in a basic path in function " + name);
}

void Function::add_precond(pExprTree cond)
{
    precond.push_back(cond);
}

void Function::add_postcond(pExprTree cond)
{
    postcond.push_back(cond);
}

void Function::add_param(pVariable param_v)
{
    params.push_back(param_v);
}

const std::vector<pExprTree>& Function::get_precond()
{
    return precond;
}

const std::vector<pExprTree>& Function::get_postcond()
{
    return postcond;
}

const std::vector<pVariable>& Function::get_params()
{
    return params;
}

Namespace* Function::root_ns()
{
    return *namespaces.begin();
}
