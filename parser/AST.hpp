#ifndef COMPILER_AST_HPP
#define COMPILER_AST_HPP

#include <memory>
#include <variant>
#include <vector>

#include "../Token.hpp"

class AST {
public:
    virtual ~AST() = default;
    virtual void emit() const = 0;
    virtual void emitStackCode() const = 0;
};

using ASTPtr = std::unique_ptr<AST>;

class BinExprNode : public AST {
private:
    TokenType oper;
    ASTPtr left, right;

public:
    BinExprNode(TokenType op, ASTPtr l, ASTPtr r);
    void emit() const override;
    void emitStackCode() const override;
};

class LiteralExprNode : public AST {
public:
    using Value = std::variant<int, std::string>;

    explicit LiteralExprNode(int val);
    explicit LiteralExprNode(std::string val);

    void emit() const override;
    void emitStackCode() const override;

private:
    Value value;
};

class ExprStmtNode : public AST {
private:
    ASTPtr expr;

public:
    explicit ExprStmtNode(ASTPtr expr);
    void emit() const override;
    void emitStackCode() const override;
};

class BlockNode : public AST {
private:
    std::vector<ASTPtr> stmts;

public:
    explicit BlockNode(std::vector<ASTPtr> stmts);
    void emit() const override;
    void emitStackCode() const override;
};

class IfNode : public AST {
private:
    ASTPtr cond;
    ASTPtr thenBranch, elseBranch;

public:
    IfNode(ASTPtr cond, ASTPtr thenBranch, ASTPtr elseBranch);
    void emit() const override;
    void emitStackCode() const override;
};

class WhileNode : public AST {
private:
    ASTPtr cond;
    ASTPtr body;

public:
    WhileNode(ASTPtr cond, ASTPtr body);
    void emit() const override;
    void emitStackCode() const override;
};


#endif //COMPILER_AST_HPP
