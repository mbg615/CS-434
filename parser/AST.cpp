#include "AST.hpp"

#include <iostream>

std::ostream* AST::out = &std::cout;

void AST::setOutputStream(std::ostream *stream) {
    out = stream;

}

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
        case TokenType::PLUS: *out << "add\n"; break;
        case TokenType::MINUS: *out << "sub\n"; break;
        case TokenType::ASTERISK: *out << "mul\n"; break;
        case TokenType::FORWARD_SLASH: *out << "div\n"; break;
        case TokenType::EQUALS: *out << "eq\n"; break;
        case TokenType::NOT_EQUALS: *out << "neq\n"; break;
        case TokenType::LESS: *out << "lt\n"; break;
        case TokenType::GREATER: *out << "gt\n"; break;
        case TokenType::GREATER_EQUALS: *out << "gte\n"; break;
        case TokenType::LESS_EQUALS: *out << "lte\n"; break;
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
        *out << "push " << std::get<int>(value) << "\n";
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
    *out << "brz " << elseLabel << "\n";
    thenBranch->emitStackCode();

    if(elseBranch) {
        *out << "jump " << endLabel << "\n";
        *out << elseLabel << "\n";
        elseBranch->emitStackCode();
        *out << "jump " << endLabel << "\n";
        *out << endLabel << "\n";
    } else {
        *out << elseLabel << "\n";
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

    *out << "jump " << startLabel << "\n";
    *out << startLabel << "\n";
    cond->emitStackCode();
    *out << "brz " << endLabel << "\n";
    body->emitStackCode();
    *out << "jump " << startLabel << "\n";
    *out << endLabel << "\n";
}

VarDeclNode::VarDeclNode(std::string varName, ASTPtr initializer, int offset) : name(std::move(varName)), initializer(std::move(initializer)), offset(offset) {}

void VarDeclNode::emit() const {
    std::cout << "Declare: " << name << " as: ";
    initializer->emit();
}

void VarDeclNode::emitStackCode() const {
    initializer->emitStackCode();
    *out << "push " << offset << "\n";
    *out << "store bp" << "\n";

}

VarExprNode::VarExprNode(std::string varName, int offset) : offset(offset), name(std::move(varName)) {}

void VarExprNode::emit() const {
    std::cout << "Var " << name << "\n";
}

void VarExprNode::emitStackCode() const {
    *out << "push " << offset << "\n";
    *out << "load bp\n";
}

AssignNode::AssignNode(int offset, ASTPtr expr) : offset(offset), expr(std::move(expr)) {}

void AssignNode::emit() const {
    std::cout << "Assign at offset: " << offset << "\n";
    expr->emit();
}

void AssignNode::emitStackCode() const {
    expr->emitStackCode();           // evaluate RHS and leave result on stack
    *out << "push " << offset << "\n";  // push the variable offset
    *out << "store bp\n";        // store the result into bp + offset
}

ReturnNode::ReturnNode(ASTPtr expr) : expr(std::move(expr)) {}

void ReturnNode::emit() const {
    std::cout << "Return: ";
    if(expr) {
        expr->emit();
    } else {
        std::cout << "NULL\n";
    }
}

void ReturnNode::emitStackCode() const {
    if(expr) {
        expr->emit();
        *out << "retv\n";
    } else {
        *out << "ret\n";
    }
}
