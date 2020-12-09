#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <cstdint>
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
    virtual pExprTree replace(pVariable v, pExprTree expr) const = 0;
    // replace with FP-style
    // return newly created node
    // return nullptr for unmodified node
    // (cannot get origin shared_ptr and cannot create a new one)
    virtual pExprTree clone_tree() const = 0;
};

class BoolExpr : public ExprTree
{
public:
    virtual ExprTree::ValueType value_type() const override;
};

class IntExpr : public ExprTree
{
public:
    virtual ExprTree::ValueType value_type() const override;
};

class Namespace;
class Variable : public IntExpr
{
    std::string name;
    Namespace *ns;
    Variable(std::string arg_name, Namespace *arg_ns);
public:
    virtual z3::expr z3_expr(ExprContext &ctx) const override;
    virtual pExprTree replace(pVariable v, pExprTree expr) const override;
    virtual pExprTree clone_tree() const override;
    friend class Namespace;
};

class IntLiteral : public IntExpr
{
    int32_t value;
public:
    IntLiteral(int32_t arg_value);
    virtual z3::expr z3_expr(ExprContext &ctx) const override;
    virtual pExprTree replace(pVariable v, pExprTree expr) const override;
    virtual pExprTree clone_tree() const override;
};

class BinaryLogic : public BoolExpr // (bool, bool) -> bool
{
public:
    enum BinaryLogicOp
    {
        and_logic,
        or_logic,
        imply,
    };
private:
    BinaryLogicOp op;
    pExprTree lhs, rhs;
public:
    BinaryLogic(BinaryLogicOp arg_op, pExprTree arg_lhs, pExprTree arg_rhs);
    virtual z3::expr z3_expr(ExprContext &ctx) const override;
    virtual pExprTree replace(pVariable v, pExprTree expr) const override;
    virtual pExprTree clone_tree() const override;
};

class BinaryArith : public IntExpr // (int, int) -> int
{
public:
    enum BinaryArithOp
    {
        add,
        minus,
        mul,
        div,
        mod
    };
private:
    BinaryArithOp op;
    pExprTree lhs, rhs;
public:
    BinaryArith(BinaryArithOp arg_op, pExprTree arg_lhs, pExprTree arg_rhs);
    virtual z3::expr z3_expr(ExprContext &ctx) const override;
    virtual pExprTree replace(pVariable v, pExprTree expr) const override;
    virtual pExprTree clone_tree() const override;
};

class BinaryCmp : public BoolExpr // (int, int) -> bool
{
public:
    enum BinaryCmpOp
    {
        eq,
        neq,
        le,
        lt,
        ge,
        gt
    };
private:
    BinaryCmpOp op;
    pExprTree lhs, rhs;
public:
    BinaryCmp(BinaryCmpOp arg_op, pExprTree arg_lhs, pExprTree arg_rhs);
    virtual z3::expr z3_expr(ExprContext &ctx) const override;
    virtual pExprTree replace(pVariable v, pExprTree expr) const override;
    virtual pExprTree clone_tree() const override;
};

class Unary : public ExprTree
{
public:
    enum UnaryOp
    {
        neg,         // -
        bitwise_not, // ~
        logic_not    // !
    };
private:
    UnaryOp op;
    pExprTree child;
public:
    Unary(UnaryOp arg_op, pExprTree arg_child);
    virtual ExprTree::ValueType value_type() const override;
    virtual z3::expr z3_expr(ExprContext &ctx) const override;
    virtual pExprTree replace(pVariable v, pExprTree expr) const override;
    virtual pExprTree clone_tree() const override;
};
