#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <z3++.h>

class ExprContext
{
    z3::context context;
    std::vector<z3::expr> variables;
    std::map<std::string, size_t> variable_pos;
public:
    z3::context& z3_context();
    z3::expr get_variable(std::string name);
};

class ExprTree;
class Variable;
using pExprTree = std::shared_ptr<const ExprTree>;
using pVariable = std::shared_ptr<const Variable>;

class ExprTree
{
public:
    enum ValueType
    {
        bool_type,
        int_type
    };
    virtual ValueType value_type() const = 0;
    virtual z3::expr z3_expr(ExprContext &ctx) const = 0;
    virtual pExprTree replace(pVariable lhs, pExprTree rhs) const = 0;
    // replace with FP-style
    // return newly created node
    // return nullptr for unmodified node
    // (cannot get origin shared_ptr and cannot create a new one)
};

class BoolExpr : public ExprTree
{
public:
    virtual ValueType value_type() const override;
};

class IntExpr : public ExprTree
{
public:
    virtual ValueType value_type() const override;
};

class Namespace;
class Variable : public IntExpr
{
    std::string name;
    Variable(std::string arg_name);
public:
    virtual z3::expr z3_expr(ExprContext &ctx) const override;
    virtual pExprTree replace(pVariable lhs, pExprTree rhs) const override;
    friend class Namespace;
};
