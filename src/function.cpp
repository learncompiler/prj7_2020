#include <stdexcept>
#include "function.h"
#include "utils.h"

Function::Function(std::string arg_name): name(arg_name)
{
    Namespace *root_ns = new Namespace(nullptr);
    namespaces.push_back(root_ns);
    Block *in = new Block(root_ns);
    blocks.push_back(in);
    starting_blocks.push_back(in);
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

static void dfs(Block *cur, std::vector<Action*> &path, std::string func_name)
{
    for (Action *action : cur->actions)
        path.push_back(action);
    if (cur->terminate)
    {
        if (!verify_basic_path(path))
            throw VerificationError("Verification failed in a basic path in function " + func_name);
    } else if (cur->conditional)
    {
        size_t old_size = path.size();
        Action *assume1 = new Assume(cur->condition);
        path.push_back(assume1);
        dfs(cur->next, path, func_name);
        path.erase(path.begin() + old_size, path.end());
        delete assume1;
        Action *assume2 = new Assume(pExprTree(new Unary(Unary::logic_not, cur->condition)));
        path.push_back(assume2);
        dfs(cur->alter, path, func_name);
        path.erase(path.begin() + old_size, path.end());
        delete assume2;
    } else dfs(cur->next, path, func_name);
}
void Function::verify()
{
    for (Block *i : starting_blocks)
    {
        std::vector<Action*> tmp;
        dfs(i, tmp, name);
    }
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
    params_set.insert(param_v);
}

void Function::add_starting_block(Block *block)
{
    starting_blocks.push_back(block);
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

const std::set<pVariable>& Function::get_params_set()
{
    return params_set;
}

Namespace* Function::root_ns()
{
    return *namespaces.begin();
}
