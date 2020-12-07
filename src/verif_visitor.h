#pragma once

#include "function.h"

class VerifVisitor : public mdverifBaseVisitor
{
public:
    using Node = antlr4::tree::ParseTree;
    virtual antlrcpp::Any visitChildren(Node *ctx) override;
    
    virtual antlrcpp::Any visitProgram(MiniDecafParser::ProgramContext *ctx) override;

    virtual antlrcpp::Any visitFunction(MiniDecafParser::FunctionContext *ctx) override;

    virtual antlrcpp::Any visitCondition(MiniDecafParser::ConditionContext *ctx) override;

    virtual antlrcpp::Any visitPrecondition(MiniDecafParser::PreconditionContext *ctx) override;

    virtual antlrcpp::Any visitPostcondition(MiniDecafParser::PostconditionContext *ctx) override;

    virtual antlrcpp::Any visitType(MiniDecafParser::TypeContext *ctx) override;

    virtual antlrcpp::Any visitCompound_statement(MiniDecafParser::Compound_statementContext *ctx) override;

    virtual antlrcpp::Any visitBlock_item(MiniDecafParser::Block_itemContext *ctx) override;

    virtual antlrcpp::Any visitStmt1(MiniDecafParser::Stmt1Context *ctx) override;

    virtual antlrcpp::Any visitStmt2(MiniDecafParser::Stmt2Context *ctx) override;

    virtual antlrcpp::Any visitStmt3(MiniDecafParser::Stmt3Context *ctx) override;

    virtual antlrcpp::Any visitStmt4(MiniDecafParser::Stmt4Context *ctx) override;

    virtual antlrcpp::Any visitDeclaration(MiniDecafParser::DeclarationContext *ctx) override;

    virtual antlrcpp::Any visitAssignment(MiniDecafParser::AssignmentContext *ctx) override;

    virtual antlrcpp::Any visitExpression(MiniDecafParser::ExpressionContext *ctx) override;

    virtual antlrcpp::Any visitLogical_imply1(MiniDecafParser::Logical_imply1Context *ctx) override;

    virtual antlrcpp::Any visitLogical_imply2(MiniDecafParser::Logical_imply2Context *ctx) override;

    virtual antlrcpp::Any visitLogical_or1(MiniDecafParser::Logical_or1Context *ctx) override;

    virtual antlrcpp::Any visitLogical_or2(MiniDecafParser::Logical_or2Context *ctx) override;

    virtual antlrcpp::Any visitLogical_and1(MiniDecafParser::Logical_and1Context *ctx) override;

    virtual antlrcpp::Any visitLogical_and2(MiniDecafParser::Logical_and2Context *ctx) override;

    virtual antlrcpp::Any visitEquality1(MiniDecafParser::Equality1Context *ctx) override;

    virtual antlrcpp::Any visitEquality2(MiniDecafParser::Equality2Context *ctx) override;

    virtual antlrcpp::Any visitRelational1(MiniDecafParser::Relational1Context *ctx) override;

    virtual antlrcpp::Any visitRelational2(MiniDecafParser::Relational2Context *ctx) override;

    virtual antlrcpp::Any visitAdditive1(MiniDecafParser::Additive1Context *ctx) override;

    virtual antlrcpp::Any visitAdditive2(MiniDecafParser::Additive2Context *ctx) override;

    virtual antlrcpp::Any visitMultiplicative1(MiniDecafParser::Multiplicative1Context *ctx) override;

    virtual antlrcpp::Any visitMultiplicative2(MiniDecafParser::Multiplicative2Context *ctx) override;

    virtual antlrcpp::Any visitUnary1(MiniDecafParser::Unary1Context *ctx) override;

    virtual antlrcpp::Any visitUnary2(MiniDecafParser::Unary2Context *ctx) override;

    virtual antlrcpp::Any visitPrimary1(MiniDecafParser::Primary1Context *ctx) override;

    virtual antlrcpp::Any visitPrimary2(MiniDecafParser::Primary2Context *ctx) override;

    virtual antlrcpp::Any visitPrimary3(MiniDecafParser::Primary3Context *ctx) override;
};