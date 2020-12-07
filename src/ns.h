#pragma once
#include <string>
#include <map>
#include <vector>
#include <string>
#include "exprtree.h"

class Namespace
{
    static int namespace_cnt;
    int id;
    std::map<std::string, pVariable> vars;
    Namespace *parent;
public:
    Namespace(Namespace *arg_parent);
    pVariable resolve(std::string name);
    pVariable recursive_resolve(std::string name);
    std::string to_string();
    pVariable new_var(std::string name);
};