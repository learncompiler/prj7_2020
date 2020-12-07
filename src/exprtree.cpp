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
pExprTree Variable::replace(pVariable lhs, pExprTree rhs) const
{
    return lhs.get() == this ? rhs : nullptr;
}