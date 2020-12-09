#pragma once

#include "mdverifBaseVisitor.h"
#include "function.h"

class VerifVisitor : public mdverifBaseVisitor
{
    Function *cur_func;
    enum ParsingState
    {
        normal,
        pre,
        post
    } cur_state;
public:
    
    using Node = antlr4::tree::ParseTree;
    virtual antlrcpp::Any visitChildren(Node *ctx) override;
    
    virtual antlrcpp::Any visitProgram(mdverifParser::ProgramContext *ctx) override;

    virtual antlrcpp::Any visitFunction(mdverifParser::FunctionContext *ctx) override;

    virtual antlrcpp::Any visitCondition(mdverifParser::ConditionContext *ctx) override;

    virtual antlrcpp::Any visitPrecondition(mdverifParser::PreconditionContext *ctx) override;

    virtual antlrcpp::Any visitPostcondition(mdverifParser::PostconditionContext *ctx) override;

    virtual antlrcpp::Any visitType(mdverifParser::TypeContext *ctx) override;

    virtual antlrcpp::Any visitCompound_statement(mdverifParser::Compound_statementContext *ctx) override;

    virtual antlrcpp::Any visitBlock_item(mdverifParser::Block_itemContext *ctx) override;

    virtual antlrcpp::Any visitStmt1(mdverifParser::Stmt1Context *ctx) override;

    virtual antlrcpp::Any visitStmt2(mdverifParser::Stmt2Context *ctx) override;

    virtual antlrcpp::Any visitStmt3(mdverifParser::Stmt3Context *ctx) override;

    virtual antlrcpp::Any visitStmt4(mdverifParser::Stmt4Context *ctx) override;

    virtual antlrcpp::Any visitDeclaration(mdverifParser::DeclarationContext *ctx) override;

    virtual antlrcpp::Any visitAssignment(mdverifParser::AssignmentContext *ctx) override;

    virtual antlrcpp::Any visitExpression(mdverifParser::ExpressionContext *ctx) override;

    virtual antlrcpp::Any visitLogical_imply1(mdverifParser::Logical_imply1Context *ctx) override;

    virtual antlrcpp::Any visitLogical_imply2(mdverifParser::Logical_imply2Context *ctx) override;

    virtual antlrcpp::Any visitLogical_or1(mdverifParser::Logical_or1Context *ctx) override;

    virtual antlrcpp::Any visitLogical_or2(mdverifParser::Logical_or2Context *ctx) override;

    virtual antlrcpp::Any visitLogical_and1(mdverifParser::Logical_and1Context *ctx) override;

    virtual antlrcpp::Any visitLogical_and2(mdverifParser::Logical_and2Context *ctx) override;

    virtual antlrcpp::Any visitEquality1(mdverifParser::Equality1Context *ctx) override;

    virtual antlrcpp::Any visitEquality2(mdverifParser::Equality2Context *ctx) override;

    virtual antlrcpp::Any visitRelational1(mdverifParser::Relational1Context *ctx) override;

    virtual antlrcpp::Any visitRelational2(mdverifParser::Relational2Context *ctx) override;

    virtual antlrcpp::Any visitAdditive1(mdverifParser::Additive1Context *ctx) override;

    virtual antlrcpp::Any visitAdditive2(mdverifParser::Additive2Context *ctx) override;

    virtual antlrcpp::Any visitMultiplicative1(mdverifParser::Multiplicative1Context *ctx) override;

    virtual antlrcpp::Any visitMultiplicative2(mdverifParser::Multiplicative2Context *ctx) override;

    virtual antlrcpp::Any visitUnary1(mdverifParser::Unary1Context *ctx) override;

    virtual antlrcpp::Any visitUnary2(mdverifParser::Unary2Context *ctx) override;

    virtual antlrcpp::Any visitPrimary1(mdverifParser::Primary1Context *ctx) override;

    virtual antlrcpp::Any visitPrimary2(mdverifParser::Primary2Context *ctx) override;

    virtual antlrcpp::Any visitPrimary3(mdverifParser::Primary3Context *ctx) override;
};