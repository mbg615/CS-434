#ifndef COMPILER_AST_HPP
#define COMPILER_AST_HPP

#include <memory>
#include <variant>

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


#endif //COMPILER_AST_HPP
