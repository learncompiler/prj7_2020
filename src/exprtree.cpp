#include "exprtree.h"
#include "utils.h"

z3::context& ExprContext::z3_context()
{
    return context;
}

z3::expr get_variable(std::string name)
{
    auto it = variable_pos.find(name);
    if (it != variable_pos.end())
    {
        return variables[it->second];
    } else
    {
        variables.push_back(context.bv_const(name, INT_WIDTH));
        variable_pos[name] = variables.size() - 1;
        return *std::prev(variables.end());
    }
}

ValueType BoolExpr::value_type() const
{
    return ExprTree::bool_type;
}

ValueType IntExpr::value_type() const
{
    return ExprTree::int_type;
}

Variable::Variable(std::string arg_name): name(arg_name) {}
z3::expr Variable::z3_expr(ExprContext &ctx) const
{
    return ctx.get_variable(name);
}
pExprTree Variable::replace(pVariable v, pExprTree expr) const
{
    return v.get() == this ? expr : nullptr;
}

IntLiteral::IntLiteral(int32_t arg_value): value(arg_value) {}
z3::expr IntLiteral::z3_expr(ExprContext &ctx) const
{
    return ctx.z3_context().bv_val(value, INT_WIDTH);
}
pExprTree IntLiteral::replace(pVariable v, pExprTree expr) const
{
    return nullptr;
}

BinaryLogic::BinaryLogic(BinaryLogicOp arg_op, pExprTree arg_lhs, pExprTree arg_rhs):
    op(arg_op), lhs(arg_lhs), rhs(arg_rhs) {}
z3::expr BinaryLogic::z3_expr(ExprContext &ctx) const
{
    z3::expr l_expr = lhs->z3_expr(ctx), r_expr = rhs->z3_expr(ctx);
    if (lhs->value_type() == ExprTree::int_type) l_expr = (l_expr != ctx.z3_context().bv_val(0, INT_WIDTH));
    if (rhs->value_type() == ExprTree::int_type) r_expr = (r_expr != ctx.z3_context().bv_val(0, INT_WIDTH));
    switch(op)
    {
        case and: return l_expr && r_expr;
        case or : return l_expr || r_expr;
        case imply : return z3::implies(l_expr, r_expr);
    }
}
pExprTree BinaryLogic::replace(pVariable v, pExprTree expr) const
{
    pExprTree new_l = lhs->replace(v, expr), new_r = rhs->replace(v, expr);
    if (!new_l) new_l = lhs;
    if (!new_r) new_r = rhs;
    if (new_l != lhs || new_r != rhs) return pExprTree(new BinaryLogic(op, new_l, new_r));
    else return nullptr;
}

BinaryArith::BinaryArith(BinaryArithOp arg_op, pExprTree arg_lhs, pExprTree arg_rhs):
    op(arg_op), lhs(arg_lhs), rhs(arg_rhs) {}
z3::expr BinaryArith::z3_expr(ExprContext &ctx) const
{
    z3::expr l_expr = lhs->z3_expr(ctx), r_expr = rhs->z3_expr(ctx);
    if (lhs->value_type() == ExprTree::bool_type)
        l_expr = z3::ite(l_expr, ctx.z3_context().bv_val(1, INT_WIDTH), ctx.z3_context().bv_val(0, INT_WIDTH));
    if (rhs->value_type() == ExprTree::bool_type)
        r_expr = z3::ite(r_expr, ctx.z3_context().bv_val(1, INT_WIDTH), ctx.z3_context().bv_val(0, INT_WIDTH));
    switch(op)
    {
        case add: return l_expr + r_expr;
        case minus: return l_expr - r_expr;
        case mul: return l_expr * r_expr;
        case div: return l_expr / r_expr;
        case mod: return z3::smod(l_expr, r_expr);
    }
}
pExprTree BinaryArith::replace(pVariable v, pExprTree expr) const
{
    pExprTree new_l = lhs->replace(v, expr), new_r = rhs->replace(v, expr);
    if (!new_l) new_l = lhs;
    if (!new_r) new_r = rhs;
    if (new_l != lhs || new_r != rhs) return pExprTree(new BinaryArith(op, new_l, new_r));
    else return nullptr;
}

BinaryCmp::BinaryCmp(BinaryCmpOp arg_op, pExprTree arg_lhs, pExprTree arg_rhs):
    op(arg_op), lhs(arg_lhs), rhs(arg_rhs) {}
z3::expr BinaryCmp::z3_expr(ExprContext &ctx) const
{
    z3::expr l_expr = lhs->z3_expr(ctx), r_expr = rhs->z3_expr(ctx);
    if (lhs->value_type() == ExprTree::bool_type)
        l_expr = z3::ite(l_expr, ctx.z3_context().bv_val(1, INT_WIDTH), ctx.z3_context().bv_val(0, INT_WIDTH));
    if (rhs->value_type() == ExprTree::bool_type)
        r_expr = z3::ite(r_expr, ctx.z3_context().bv_val(1, INT_WIDTH), ctx.z3_context().bv_val(0, INT_WIDTH));
    switch(op)
    {
        case eq: return l_expr == r_expr;
        case neq: return l_expr != r_expr;
        case le: return l_expr <= r_expr;
        case lt: return l_expr < r_expr;
        case ge: return l_expr >= r_expr;
        case gt: return l_expr > r_expr;
    }
}
pExprTree BinaryCmp::replace(pVariable v, pExprTree expr) const
{
    pExprTree new_l = lhs->replace(v, expr), new_r = rhs->replace(v, expr);
    if (!new_l) new_l = lhs;
    if (!new_r) new_r = rhs;
    if (new_l != lhs || new_r != rhs) return pExprTree(new BinaryCmp(op, new_l, new_r));
    else return nullptr;
}

Unary::Unary(UnaryOp arg_op, pExprTree arg_child):
    op(arg_op), child(arg_child) {}
ExprTree::ValueType Unary::value_type() const
{
    if (op == neg) return ExprTree::int_type;
    else if (op == logic_not) return Expr::bool_type;
    else return child->value_type();
}
z3::expr Unary::z3_expr(ExprContext &ctx) const
{
    z3::expr child_expr = child->z3_expr(ctx);
    if (op == neg)
    {
        if (child->value_type() == bool_type)
            child_expr = z3::ite(child_expr, ctx.z3_context().bv_val(1, INT_WIDTH), ctx.z3_context().bv_val(0, INT_WIDTH));
        return -child_expr;
    } else if (op == logic_not)
    {
        if (child->value_type() == int_type)
            child_expr = (child_expr != 0);
        return !child_expr;
    } else // bitwise_not
    {
        if (child->value_type() == int_type) return ~child_expr;
        else return !child_expr;
    }
}
pExprTree Unary::replace(pVariable v, pExprTree expr) const
{
    pExprTree new_child = child->replace(v, expr);
    if (!new_child) new_child = child;
    if (new_child != child) return pExprTree(new Unary(op, child));
    else return nullptr;
}