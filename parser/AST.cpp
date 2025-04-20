#include "AST.hpp"

#include <iostream>

BinExprNode::BinExprNode(TokenType op, ASTPtr l, ASTPtr r) : oper(op), left(std::move(l)), right(std::move(r)) {}

void BinExprNode::emit() const {
    std::cout << "(";
    left->emit();
    std::cout << " " << toString(oper) << " ";
    right->emit();
    std::cout << ")";
}

void BinExprNode::emitStackCode() const {
    left->emitStackCode();
    right->emitStackCode();

    switch(oper) {
        case TokenType::PLUS: std::cout << "add\n"; break;
        case TokenType::MINUS: std::cout << "sub\n"; break;
        case TokenType::ASTERISK: std::cout << "mul\n"; break;
        case TokenType::FORWARD_SLASH: std::cout << "div\n"; break;
        default: throw std::runtime_error("Unknown Operator");
    }
}

LiteralExprNode::LiteralExprNode(int val) : value(val) {}

LiteralExprNode::LiteralExprNode(std::string val) : value(std::move(val)) {}

void LiteralExprNode::emit() const {
    if (std::holds_alternative<int>(value)) {
        std::cout << std::get<int>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        std::cout << "\"" << std::get<std::string>(value) << "\"";
    }
}

void LiteralExprNode::emitStackCode() const {
    if (std::holds_alternative<int>(value)) {
        std::cout << "push " << std::get<int>(value) << "\n";
    }
}
