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
    // add function arguments to root ns
    cur_func->cur_ns()->new_var(result_name);
    for (mdverifParser::ConditionContext *i : ctx->condition()) i->accept(this);
    for (pExprTree i : cur_func->get_precond())
        cur_func->add_action(new Assume(i));
    cur_state = normal;
    ctx->compound_statement()->accept(this);
    if (!cur_func->cur_block()->terminate)
        for (pExprTree i : cur_func->get_postcond())
            cur_func->add_action(new Assert(i));
    cur_func->verify();
    delete cur_func;
    cur_func = nullptr;
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
    Block *cur_block = cur_func->cur_block();
    if(cur_block->terminate) return nullptr;
    Namespace *cur_ns = cur_func->cur_ns();
    cur_block->next = cur_func->new_block(cur_func->new_ns(cur_ns));
    visitChildren(ctx);
    Block *out_block = cur_func->cur_block();
    if (!out_block->terminate) out_block->next = cur_func->new_block(cur_ns);
    return nullptr;
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
    return visitChildren(ctx);
}

antlrcpp::Any VerifVisitor::visitDeclaration(mdverifParser::DeclarationContext *ctx)
{
    Log::debug << "VerifVisitor::visitDeclaration\n";
    Log::error << "not implemented\n";
    // TODO
    // variable name cannot be result_name
    return nullptr;
}

antlrcpp::Any VerifVisitor::visitAssignment(mdverifParser::AssignmentContext *ctx)
{
    Log::debug << "VerifVisitor::visitAssignment\n";
    if (ctx->Identifier()->getText() == result_name)
        throw SyntaxError("cannot assign to " + result_name);
    pVariable lhs = cur_func->cur_ns()->recursive_resolve(ctx->Identifier()->getText());
    if (!lhs) throw SyntaxError("cannot find variable " + ctx->Identifier()->getText());
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