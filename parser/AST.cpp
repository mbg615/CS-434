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

ExprStmtNode::ExprStmtNode(ASTPtr expr) : expr(std::move(expr)) {};

void ExprStmtNode::emit() const {
    expr->emit();
    // ToDo: Complete logic if needed.
}

void ExprStmtNode::emitStackCode() const {
    expr->emitStackCode();
}

BlockNode::BlockNode(std::vector<ASTPtr> stmts) : stmts(std::move(stmts)) {}

void BlockNode::emit() const {
    std::cout << "{\n";
    for (const auto& stmt : stmts) {
        stmt->emit();
        std::cout << "\n";
    }
    std::cout << "}\n";

}

void BlockNode::emitStackCode() const {
    for (const auto& stmt : stmts) {
        stmt->emitStackCode();
    }
}

IfNode::IfNode(ASTPtr cond, ASTPtr thenBranch, ASTPtr elseBranch) : cond(std::move(cond)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

void IfNode::emit() const {
    std::cout << "if: ";
    cond->emit();
    std::cout << "\nthen: ";
    thenBranch->emit();
    if(elseBranch) {
        std::cout << "\nelse: ";
        elseBranch->emit();
    }
}

void IfNode::emitStackCode() const {
    static int ifCounter = 0;
    int ifId = ifCounter++;

    std::string elseLabel = "else_" + std::to_string(ifId) + ":";
    std::string endLabel = "endif_" + std::to_string(ifId) + ":";

    cond->emitStackCode();
    std::cout << "brz " << elseLabel << "\n";
    thenBranch->emitStackCode();

    if(elseBranch) {
        std::cout << "jump " << endLabel << "\n";
        std::cout << elseLabel << "\n";
        elseBranch->emitStackCode();
        std::cout << "jump " << endLabel << "\n";
        std::cout << endLabel << "\n";
    } else {
        std::cout << elseLabel << "\n";
    }
}

WhileNode::WhileNode(ASTPtr cond, ASTPtr body) : cond(std::move(cond)), body(std::move(body)) {}

void WhileNode::emit() const {
    std::cout << "while: ";
    cond->emit();
    std::cout << "\ndo: ";
    body->emit();
}

void WhileNode::emitStackCode() const {
    static int whileCounter = 0;
    int whileId = whileCounter++;

    std::string startLabel = "while_start_" + std::to_string(whileId) + ":";
    std::string endLabel = "while_end_" + std::to_string(whileId) + ":";

    std::cout << startLabel << "\n";
    cond->emitStackCode();
    std::cout << "brz " << endLabel << "\n";
    body->emitStackCode();
    std::cout << "jump " << startLabel << "\n";
    std::cout << endLabel << "\n";
}
