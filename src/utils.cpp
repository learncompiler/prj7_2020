#include "utils.h"
#include "log.h"
#include "action.h"
#include "ns.h"
#include "exprtree.h"

bool verify_basic_path(std::vector<Action*> basic_path)
{
    Namespace tmp_ns(nullptr);
    std::vector<pVariable> tmp_var;
    int dec_cnt = 0;
    pExprTree vc, vc_with_dec, dec_vc;
    for (size_t i = basic_path.size() - 1; i < basic_path.size(); --i)
    {
        basic_path[i]->maintain_vc(vc);
        basic_path[i]->maintain_vc(vc_with_dec);
        if (Decrease *dec = dynamic_cast<Decrease*>(basic_path[i]))
        {
            ++dec_cnt;
            const std::vector<pExprTree>& rank_f = dec->rank_function();
            if (dec_cnt == 1)
            {
                for (size_t i = 0; i < rank_f.size(); ++i)
                    tmp_var.push_back(tmp_ns.new_var(stringify(i)));
                pExprTree dec_vc;
                for (size_t i = 0; i < rank_f.size(); ++i)
                {
                    pExprTree cur = pExprTree(new BinaryCmp(BinaryCmp::lt, rank_f[i], tmp_var[i]));
                    for (size_t j = 0; j < i; ++j)
                        cur = pExprTree(new BinaryLogic(BinaryLogic::and_logic, cur, 
                            pExprTree(new BinaryCmp(BinaryCmp::eq, rank_f[j], tmp_var[j]))));
                    if (dec_vc) dec_vc = pExprTree(new BinaryLogic(BinaryLogic::or_logic, dec_vc, cur));
                    else dec_vc = cur;
                }
                if (vc_with_dec) vc_with_dec = pExprTree(new BinaryLogic(BinaryLogic::and_logic, vc_with_dec, dec_vc));
                else vc_with_dec = dec_vc;
            } else if (dec_cnt == 2)
            {
                if (rank_f.size() != tmp_var.size())
                    throw SyntaxError("rank function should have equal length");
                for (size_t i = 0; i < tmp_var.size(); ++i)
                {
                    pExprTree rep = vc_with_dec->replace(tmp_var[i], rank_f[i]);
                    if (rep) vc_with_dec = rep;
                }
                vc = vc_with_dec;
            } else throw SyntaxError("decrease specification should occur only twice in a basic path");
        }
    }
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