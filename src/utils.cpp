#include "utils.h"
#include "log.h"
#include "action.h"

bool verify_basic_path(std::vector<Action*> basic_path)
{
    pExprTree vc = nullptr;
    for (size_t i = basic_path.size() - 1; i < basic_path.size(); --i)
        basic_path[i]->maintain_vc(vc);
    if (!vc) return true;
    std::ostringstream builder;
    vc->to_string(builder);
    Log::info << "vc: " << builder.str() << '\n';
    ExprContext context;
    z3::expr z3_vc = vc->z3_expr(context);
    z3::solver s(context.z3_context());
    s.add(!z3_vc);
    return s.check() == z3::unsat;
}