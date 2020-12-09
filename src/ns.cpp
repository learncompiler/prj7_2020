#include <sstream>
#include "ns.h"
using std::string;
using std::ostringstream;

Namespace::Namespace(Namespace *arg_parent): parent(arg_parent)
{
    id = namespace_cnt++;
}

pVariable Namespace::resolve(string name)
{
    auto found = vars.find(name);
    if (found == vars.end()) return nullptr; else return found->second;
}

pVariable Namespace::recursive_resolve(string name)
{
    pVariable cur = resolve(name);
    if (cur) return cur;
    if (parent) return parent->recursive_resolve(name);
    return nullptr;
}

std::string Namespace::to_string()
{
    ostringstream builder;
    builder << "ns" << id;
    return builder.str();
}

pVariable Namespace::new_var(string name)
{
    if (vars.find(name) != vars.end())
        throw std::runtime_error("duplicate variable name " + name + " in " + to_string());
    pVariable created_var = pVariable(new Variable(to_string() + "::" + name, this));
    vars[name] = created_var;
    return created_var;
}

int Namespace::namespace_cnt = 0;