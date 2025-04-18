#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../Token.hpp"

#ifndef AST_HPP
#define AST_HPP

enum class Type {
    INT,
    VOID
};

std::string typeToString(Type);

enum class BinOp {
    ADD,
    SUB,
    MUL,
    DIV,
    EQ,
    NEQ,
    GT,
    GTE,
    LT,
    LTE
};

std::string binToString(BinOp);

class AST {
public:
    virtual ~AST() = default;
    virtual void emitStackCode() const = 0;
    virtual void emit() const = 0;
};

using ASTPtr = std::unique_ptr<AST>;

class ProgramNode : public AST {
public:
    std::vector<ASTPtr> declarations;
    void emitStackCode() const override;
    void emit() const override;
};

class VarDeclarationNode : public AST {
public:
    Type type;
    std::string name;

    VarDeclarationNode(Type t, std::string n) : type(t), name(std::move(n)) {};
    void emitStackCode() const override;
    void emit() const override;
};

class ParamNode : public AST {
public:
    Type type;
    std::string name;

    ParamNode(Type t, std::string n) : type(t), name(std::move(n)) {};
    void emitStackCode() const override;
    void emit() const override;
};

class FunDeclNode : public AST {
public:
    Type returnType;
    std::string name;
    std::vector<std::unique_ptr<ParamNode>> parameters;
    ASTPtr body;

    FunDeclNode(Type returnType, std::string name, std::vector<std::unique_ptr<ParamNode>> parameters, ASTPtr body) : returnType(returnType), name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {};
    void emitStackCode() const override;
    void emit() const override;
};

class CompoundStmtNode : public AST {
public:
    std::vector<ASTPtr> localDeclarations;
    std::vector<ASTPtr> statements;

    CompoundStmtNode(std::vector<ASTPtr> localDeclarations, std::vector<ASTPtr> statements) : localDeclarations(std::move(localDeclarations)), statements(std::move(statements)) {}
    void emitStackCode() const override;
    void emit() const override;
};

class ExprStmtNode : public AST {
public:
    ExprStmtNode() = default;
    explicit ExprStmtNode(ASTPtr expr) : expr(std::move(expr)) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    ASTPtr expr;
};

class AssignExprNode : public AST {
public:
    AssignExprNode(std::string id, std::unique_ptr<AST> rightExpr) : id(std::move(id)), rightExpr(std::move(rightExpr)) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    std::string id;
    ASTPtr rightExpr;
};

class VarExprNode : public AST {
public:
    explicit VarExprNode(std::string id) : id(std::move(id)) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    std::string id;
};

class IfStmtNode : public AST {
public:
    IfStmtNode(ASTPtr condition, ASTPtr thenStmt, ASTPtr elseStmt = nullptr) : condition(std::move(condition)), thenStmt(std::move(thenStmt)), elseStmt(std::move(elseStmt)) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    ASTPtr condition;
    ASTPtr thenStmt;
    ASTPtr elseStmt;
};

class WhileStmtNode : public AST {
public:
    WhileStmtNode(ASTPtr c, ASTPtr b) : condition(std::move(c)), body(std::move(b)) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    ASTPtr condition;
    ASTPtr body;
};

class ReturnStmtNode : public AST {
public:
    ReturnStmtNode() = default;
    explicit ReturnStmtNode(ASTPtr e) : expr(std::move(e)) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    ASTPtr expr;
};

class BinExprNode : public AST {
public:
    BinExprNode(TokenType op, ASTPtr left, ASTPtr right) : oper(op), left(std::move(left)), right(std::move(right)) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    TokenType oper;
    ASTPtr left, right;
};

class NumberExprNode : public AST {
public:
    explicit NumberExprNode(int x) : value(x) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    int value;
};

class CallExprNode : public AST {
public:
    CallExprNode(std::string id, std::vector<ASTPtr> args) : id(std::move(id)), arguments(std::move(args)) {}
    void emitStackCode() const override;
    void emit() const override;

private:
    std::string id;
    std::vector<ASTPtr> arguments;
};

#endif //AST_HPP
