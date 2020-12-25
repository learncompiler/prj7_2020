#include <memory>
#include <string>
#include <cstdint>
#include "verif_visitor.h"
#include "log.h"
#include "utils.h"

static const std::string result_name = "result";

antlrcpp::Any VerifVisitor::visitChildren(VerifVisitor::Node *ctx)
{
    size_t n = ctx->children.size();
    for (size_t i = 0; i < n; ++i) ctx->children[i]->accept(this);
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitProgram(mdverifParser::ProgramContext *ctx)
{
    Log::debug << "VerifVisitor::visitProgram\n";
    cur_func = nullptr;
    cur_state = normal;
    return visitChildren(ctx);
}

antlrcpp::Any VerifVisitor::visitFunction(mdverifParser::FunctionContext *ctx)
{
    Log::debug << "VerifVisitor::visitFunction\n";
    cur_func = new Function(ctx->Identifier()->getText());
    ctx->parameter_list()->accept(this);
    cur_func->cur_ns()->new_var(result_name);
    for (mdverifParser::ConditionContext *i : ctx->condition()) i->accept(this);
    for (pExprTree i : cur_func->get_precond())
        cur_func->add_action(new Assume(i));
    cur_state = normal;
    ctx->compound_statement()->accept(this);
    if (!cur_func->cur_block()->terminate)
    {
        for (pExprTree i : cur_func->get_postcond())
            cur_func->add_action(new Assert(i));
        cur_func->cur_block()->terminate = true;
    }
    cur_func->verify();
    delete cur_func;
    cur_func = nullptr;
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitParameter_list(mdverifParser::Parameter_listContext *ctx)
{
    Log::debug << "VerifVisitor::visitParameter_list\n";
    for (auto i : ctx->Identifier())
        cur_func->add_param(cur_func->cur_ns()->new_var(i->getText()));
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitCondition(mdverifParser::ConditionContext *ctx)
{
    Log::debug << "VerifVisitor::visitCondition\n";
    return visitChildren(ctx);
}

antlrcpp::Any VerifVisitor::visitPrecondition(mdverifParser::PreconditionContext *ctx)
{
    Log::debug << "VerifVisitor::visitPrecondition\n";
    cur_state = pre;
    pExprTree precond_expr = ctx->expression()->accept(this);
    cur_func->add_precond(precond_expr);
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitPostcondition(mdverifParser::PostconditionContext *ctx)
{
    Log::debug << "VerifVisitor::visitPostcondition\n";
    cur_state = post;
    pExprTree postcond_expr = ctx->expression()->accept(this);
    cur_func->add_postcond(postcond_expr);
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitType(mdverifParser::TypeContext *ctx)
{
    Log::debug << "VerifVisitor::visitType\n";
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitCompound_statement(mdverifParser::Compound_statementContext *ctx)
{
    Log::debug << "VerifVisitor::visitCompound_statement\n";
    return visitChildren(ctx);
}

antlrcpp::Any VerifVisitor::visitBlock_item(mdverifParser::Block_itemContext *ctx)
{
    Log::debug << "VerifVisitor::visitBlock_item\n";
    return visitChildren(ctx);
}

antlrcpp::Any VerifVisitor::visitStmt1(mdverifParser::Stmt1Context *ctx)
{
    Log::debug << "VerifVisitor::visitStmt1\n";
    if (cur_func->cur_block()->terminate) return nullptr;
    pVariable result = cur_func->cur_ns()->recursive_resolve(result_name);
    cur_func->add_action(new Assign(result, ctx->expression()->accept(this)));
    for (pExprTree i : cur_func->get_postcond())
        cur_func->add_action(new Assert(i));
    cur_func->cur_block()->terminate = true;
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitStmt2(mdverifParser::Stmt2Context *ctx)
{
    Log::debug << "VerifVisitor::visitStmt2\n";
    return visitChildren(ctx);
}

antlrcpp::Any VerifVisitor::visitStmt3(mdverifParser::Stmt3Context *ctx)
{
    Log::debug << "VerifVisitor::visitStmt3\n";
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitStmt4(mdverifParser::Stmt4Context *ctx)
{
    Log::debug << "VerifVisitor::visitStmt4\n";
    Block *cur_block = cur_func->cur_block();
    if(cur_block->terminate) return nullptr;
    Namespace *cur_ns = cur_block->ns;
    cur_block->next = cur_func->new_block(cur_func->new_ns(cur_ns));
    visitChildren(ctx);
    Block *out_block = cur_func->cur_block();
    if (!out_block->terminate) out_block->next = cur_func->new_block(cur_ns);
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitStmt5(mdverifParser::Stmt5Context *ctx)
{
    Log::debug << "VerifVisitor::visitStmt5\n";
    Block *prev_block = cur_func->cur_block();
    if(prev_block->terminate) return nullptr;
    Namespace *prev_ns = prev_block->ns;
    prev_block->conditional = true;
    prev_block->condition = ctx->expression()->accept(this);
    prev_block->next = cur_func->new_block(cur_func->new_ns(prev_ns));
    ctx->statement(0)->accept(this);
    Block *first = cur_func->cur_block();
    if (ctx->statement(1))
    {
        prev_block->alter = cur_func->new_block(cur_func->new_ns(prev_ns));
        ctx->statement(1)->accept(this);
        Block *second = cur_func->cur_block();
        if ((!first->terminate) || (!second->terminate))
        {
            Block *out = cur_func->new_block(prev_ns);
            if (!first->terminate) first->next = out;
            if (!second->terminate) second->next = out;
        }
    } else
    {
        Block *out = cur_func->new_block(prev_ns);
        prev_block->alter = out;
        if (!first->terminate) first->next = out;
    }
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitStmt6(mdverifParser::Stmt6Context *ctx)
{
    Log::debug << "VerifVisitor::visitStmt6\n";
    pExprTree loop_cond = ctx->expression()->accept(this);
    std::vector<pExprTree> invariants;
    std::vector<std::vector<pExprTree>> rank_functions;
    for (mdverifParser::Loop_specContext *i : ctx->loop_spec())
    {
        antlrcpp::Any invar_or_rank = i->accept(this);
        // visitLoop_spec returns pExprTree for loop invariant
        // returns std::vector<pExprTree> for rank function
        if (invar_or_rank.is<pExprTree>()) invariants.push_back(invar_or_rank);
        else rank_functions.push_back(invar_or_rank);
    }
    gen_loop([]()->void {},
        loop_cond,
        [ctx, this]()->void {ctx->statement()->accept(this);},
        []()->void {},
        invariants,
        rank_functions
    );
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitStmt7(mdverifParser::Stmt7Context *ctx)
{
    Log::debug << "VerifVisitor::visitStmt7\n";
    pExprTree loop_cond = ctx->expression()->accept(this);
    std::vector<pExprTree> invariants;
    std::vector<std::vector<pExprTree>> rank_functions;
    for (mdverifParser::Loop_specContext *i : ctx->loop_spec())
    {
        antlrcpp::Any invar_or_rank = i->accept(this);
        if (invar_or_rank.is<pExprTree>()) invariants.push_back(invar_or_rank);
        else rank_functions.push_back(invar_or_rank);
    }
    gen_loop([ctx, this]()->void {ctx->assignment(0)->accept(this);},
        loop_cond,
        [ctx, this]()->void {ctx->statement()->accept(this);},
        [ctx, this]()->void {ctx->assignment(1)->accept(this);},
        invariants,
        rank_functions
    );
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitStmt8(mdverifParser::Stmt8Context *ctx)
{
    Log::debug << "VerifVisitor::visitStmt8\n";
    pExprTree loop_cond = ctx->expression()->accept(this);
    std::vector<pExprTree> invariants;
    std::vector<std::vector<pExprTree>> rank_functions;
    for (mdverifParser::Loop_specContext *i : ctx->loop_spec())
    {
        antlrcpp::Any invar_or_rank = i->accept(this);
        if (invar_or_rank.is<pExprTree>()) invariants.push_back(invar_or_rank);
        else rank_functions.push_back(invar_or_rank);
    }
    gen_loop([ctx, this]()->void {ctx->declaration()->accept(this);},
        loop_cond,
        [ctx, this]()->void {ctx->statement()->accept(this);},
        [ctx, this]()->void {ctx->assignment()->accept(this);},
        invariants,
        rank_functions
    );
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitLoop_spec(mdverifParser::Loop_specContext *ctx)
{
    Log::debug << "VerifVisitor::visitLoop_spec\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitLoop_invariant(mdverifParser::Loop_invariantContext *ctx)
{
    Log::debug << "VerifVisitor::visitLoop_invariant\n";
    return ctx->expression()->accept(this);
}

antlrcpp::Any VerifVisitor::visitRanking_function(mdverifParser::Ranking_functionContext *ctx)
{
    Log::debug << "VerifVisitor::visitRanking_function\n";
    std::vector<pExprTree> rank_fs;
    for (mdverifParser::ExpressionContext *i : ctx->expression())
        rank_fs.push_back(i->accept(this));
    return rank_fs;
}

antlrcpp::Any VerifVisitor::visitDeclaration(mdverifParser::DeclarationContext *ctx)
{
    Log::debug << "VerifVisitor::visitDeclaration\n";
    if (cur_func->cur_block()->terminate) return nullptr;
    pExprTree init_value;
    if (ctx->expression()) init_value = ctx->expression()->accept(this);
    std::string cur_name = ctx->Identifier()->getText();
    if (cur_name == result_name)
        throw SyntaxError("variable defination named " + result_name + "is forbidden");
    if (cur_func->cur_ns()->resolve(cur_name))
        throw SyntaxError("duplicate variable name " + cur_name);
    pVariable defined_var = cur_func->cur_ns()->new_var(cur_name);
    if (init_value)
        cur_func->add_action(new Assign(defined_var, init_value));
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitAssignment(mdverifParser::AssignmentContext *ctx)
{
    Log::debug << "VerifVisitor::visitAssignment\n";
    if (ctx->Identifier()->getText() == result_name)
        throw SyntaxError("cannot assign to " + result_name);
    pVariable lhs = cur_func->cur_ns()->recursive_resolve(ctx->Identifier()->getText());
    if (!lhs) throw SyntaxError("cannot find variable " + ctx->Identifier()->getText());
    if (cur_func->get_params_set().find(lhs) != cur_func->get_params_set().end())
        throw SyntaxError("forbid assignment to parameter");
    if (cur_func->cur_block()->terminate) return nullptr;
    cur_func->add_action(new Assign(lhs, ctx->expression()->accept(this)));
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitExpression(mdverifParser::ExpressionContext *ctx)
{
    Log::debug << "VerifVisitor::visitExpression\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitLogical_imply1(mdverifParser::Logical_imply1Context *ctx)
{
    Log::debug << "VerifVisitor::visitLogical_imply1\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitLogical_imply2(mdverifParser::Logical_imply2Context *ctx)
{
    Log::debug << "VerifVisitor::visitLogical_imply2\n";
    pExprTree lhs = ctx->children[0]->accept(this), rhs = ctx->children[2]->accept(this);
    return pExprTree(new BinaryLogic(BinaryLogic::imply, lhs, rhs));
}

antlrcpp::Any VerifVisitor::visitLogical_or1(mdverifParser::Logical_or1Context *ctx)
{
    Log::debug << "VerifVisitor::visitLogical_or1\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitLogical_or2(mdverifParser::Logical_or2Context *ctx)
{
    Log::debug << "VerifVisitor::visitLogical_or2\n";
    pExprTree lhs = ctx->children[0]->accept(this), rhs = ctx->children[2]->accept(this);
    return pExprTree(new BinaryLogic(BinaryLogic::or_logic, lhs, rhs));
}

antlrcpp::Any VerifVisitor::visitLogical_and1(mdverifParser::Logical_and1Context *ctx)
{
    Log::debug << "VerifVisitor::visitLogical_and1\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitLogical_and2(mdverifParser::Logical_and2Context *ctx)
{
    Log::debug << "VerifVisitor::visitLogical_and2\n";
    pExprTree lhs = ctx->children[0]->accept(this), rhs = ctx->children[2]->accept(this);
    return pExprTree(new BinaryLogic(BinaryLogic::and_logic, lhs, rhs));
}

antlrcpp::Any VerifVisitor::visitEquality1(mdverifParser::Equality1Context *ctx)
{
    Log::debug << "VerifVisitor::visitEquality1\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitEquality2(mdverifParser::Equality2Context *ctx)
{
    Log::debug << "VerifVisitor::visitEquality2\n";
    pExprTree lhs = ctx->children[0]->accept(this), rhs = ctx->children[2]->accept(this);
    BinaryCmp::BinaryCmpOp op = ctx->children[1]->getText() == "==" ? BinaryCmp::eq : BinaryCmp::neq;
    return pExprTree(new BinaryCmp(op, lhs, rhs));
}

antlrcpp::Any VerifVisitor::visitRelational1(mdverifParser::Relational1Context *ctx)
{
    Log::debug << "VerifVisitor::visitRelational1\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitRelational2(mdverifParser::Relational2Context *ctx)
{
    Log::debug << "VerifVisitor::visitRelational2\n";
    pExprTree lhs = ctx->children[0]->accept(this), rhs = ctx->children[2]->accept(this);
    std::string op_s = ctx->children[1]->getText();
    BinaryCmp::BinaryCmpOp op = op_s == "<" ? BinaryCmp::lt :
                                op_s == ">" ? BinaryCmp::gt :
                                op_s == "<=" ? BinaryCmp::le :
                                BinaryCmp::ge;
    return pExprTree(new BinaryCmp(op, lhs, rhs));
}

antlrcpp::Any VerifVisitor::visitAdditive1(mdverifParser::Additive1Context *ctx)
{
    Log::debug << "VerifVisitor::visitAdditive1\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitAdditive2(mdverifParser::Additive2Context *ctx)
{
    Log::debug << "VerifVisitor::visitAdditive2\n";
    pExprTree lhs = ctx->children[0]->accept(this), rhs = ctx->children[2]->accept(this);
    BinaryArith::BinaryArithOp op = ctx->children[1]->getText() == "+" ? BinaryArith::add : BinaryArith::minus;
    return pExprTree(new BinaryArith(op, lhs, rhs));
}

antlrcpp::Any VerifVisitor::visitMultiplicative1(mdverifParser::Multiplicative1Context *ctx)
{
    Log::debug << "VerifVisitor::visitMultiplicative1\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitMultiplicative2(mdverifParser::Multiplicative2Context *ctx)
{
    Log::debug << "VerifVisitor::visitMultiplicative2\n";
    pExprTree lhs = ctx->children[0]->accept(this), rhs = ctx->children[2]->accept(this);
    std::string op_s = ctx->children[1]->getText();
    BinaryArith::BinaryArithOp op = op_s == "*" ? BinaryArith::mul :
                       op_s == "/" ? BinaryArith::div :
                       BinaryArith::mod;
    return pExprTree(new BinaryArith(op, lhs, rhs));
}

antlrcpp::Any VerifVisitor::visitUnary1(mdverifParser::Unary1Context *ctx)
{
    Log::debug << "VerifVisitor::visitUnary1\n";
    return ctx->children[0]->accept(this);
}

antlrcpp::Any VerifVisitor::visitUnary2(mdverifParser::Unary2Context *ctx)
{
    Log::debug << "VerifVisitor::visitUnary2\n";
    pExprTree expr = ctx->children[1]->accept(this);
    std::string op_s = ctx->children[0]->getText();
    Unary::UnaryOp op = op_s == "-" ? Unary::neg :
                        op_s == "~" ? Unary::bitwise_not :
                        Unary::logic_not;
    return pExprTree(new Unary(op, expr));
}

static int32_t parse_int32(const std::string &s)
{
    const int32_t MAX = 0x7fffffff;
    int64_t res = 0;
    for (char ch : s)
    {
        res = res * 10 + ch - '0';
        if (res > MAX) throw SyntaxError("integer literal out of range");
    }
    return static_cast<int32_t>(res);
}

antlrcpp::Any VerifVisitor::visitPrimary1(mdverifParser::Primary1Context *ctx)
{
    Log::debug << "VerifVisitor::visitPrimary1\n";
    int32_t int_literal = parse_int32(ctx->Integer()->getText());
    return pExprTree(new IntLiteral(int_literal));
}

antlrcpp::Any VerifVisitor::visitPrimary2(mdverifParser::Primary2Context *ctx)
{
    Log::debug << "VerifVisitor::visitPrimary2\n";
    return ctx->expression()->accept(this);
}

antlrcpp::Any VerifVisitor::visitPrimary3(mdverifParser::Primary3Context *ctx)
{
    Log::debug << "VerifVisitor::visitPrimary3\n";
    if (ctx->Identifier()->getText() == result_name && cur_state != post)
        throw SyntaxError("illegal reference to " + result_name);
    pVariable v = cur_func->cur_ns()->recursive_resolve(ctx->Identifier()->getText());
    if (!v) throw SyntaxError("cannot find variable " + ctx->Identifier()->getText());
    return v->clone_tree();
}

void VerifVisitor::gen_loop(std::function<void()> pre, pExprTree cond, std::function<void()> body, std::function<void()> post,
        std::vector<pExprTree> invariants, std::vector<std::vector<pExprTree>> rank_f)
{
    Block *prev_block = cur_func->cur_block();
    Namespace *prev_ns = cur_func->cur_ns();
    if (prev_block->terminate) return;
    prev_block->next = cur_func->new_block(cur_func->new_ns(prev_ns));
    Namespace *in_ns = cur_func->cur_ns();
    Block *in_block = cur_func->cur_block();
    pre();
    for (pExprTree i : invariants)
        in_block->actions.push_back(new Assert(i));
    for (auto &i : rank_f)
        in_block->actions.push_back(new Decrease(i));
    in_block->terminate = true;
    Block *loop_start = cur_func->new_block(cur_func->cur_ns());
    cur_func->add_starting_block(loop_start);
    for (pExprTree i : invariants)
        loop_start->actions.push_back(new Assume(i));
    for (auto &i : rank_f)
        loop_start->actions.push_back(new Decrease(i));
    loop_start->conditional = true;
    loop_start->condition = cond;
    loop_start->next = cur_func->new_block(cur_func->new_ns(in_ns));
    body();
    post();
    Block *loop_end = cur_func->cur_block();
    if (!loop_end->terminate)
    {
        for (pExprTree i : invariants)
            loop_end->actions.push_back(new Assert(i));
        for (auto &i : rank_f)
            loop_end->actions.push_back(new Decrease(i));
        loop_end->terminate = true;
    }
    loop_start->alter = cur_func->new_block(prev_ns);
}