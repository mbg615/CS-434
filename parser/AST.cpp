#include <iostream>

#include "ast.hpp"

std::string typeToString(Type t) {
    switch (t) {
        case Type::INT:
            return "int";
        case Type::VOID:
            return "void";
        default:
            return "unknown";
    }
}

std::string binToString(BinOp o) {
    switch (o) {
        case BinOp::ADD:
            return "int";
        case BinOp::SUB:
            return "void";
        case BinOp::MUL:
            return "int";
        case BinOp::DIV:
            return "void";
        case BinOp::EQ:
            return "int";
        case BinOp::NEQ:
            return "void";
        case BinOp::GT:
            return "int";
        case BinOp::GTE:
            return "void";
        case BinOp::LT:
            return "int";
        case BinOp::LTE:
            return "void";
        default:
            return "unknown";
    }
}

void ProgramNode::emitStackCode() const {
    for(const auto &decl : declarations) {
        decl->emitStackCode();
    }
}

void ProgramNode::emit() const {
    for(const auto &decl : declarations) {
        decl->emit();
    }
}

void VarDeclarationNode::emitStackCode() const {
    std::cout << "Implement stack logic for VarDeclarationNode." << std::endl;
}

void VarDeclarationNode::emit() const {
    std::cout << "Declare variable: " << name << " of type " << typeToString(type) << std::endl;
}

void ParamNode::emitStackCode() const {
    std::cout << "Implement stack logic for ParamNode." << std::endl;
}

void ParamNode::emit() const {
    std::cout << "Param: " << typeToString(type) << " " << name << std::endl;
}

void FunDeclNode::emitStackCode() const {
    std::cout << "Implement stack logic for FunDeclNode." << std::endl;
}

void FunDeclNode::emit() const {
    std::cout << "Function " << name << " returns " << typeToString(returnType) << "\n";
    for (const auto& param : parameters) param->emit();
    body->emit();
}

void CompoundStmtNode::emitStackCode() const {
    std::cout << "Implement stack logic for CompoundStmtNode." << std::endl;
}

void CompoundStmtNode::emit() const {
    std::cout << "{\n";
    for (const ASTPtr &localDecl : localDeclarations) {
        localDecl->emit();
    }
    for (const ASTPtr &stmt : statements) {
        stmt->emit();
    }
    std::cout << "}\n";
}

void ExprStmtNode::emitStackCode() const {
    if(expr) {
        expr->emitStackCode();
    }
}

void ExprStmtNode::emit() const {
    if(expr) {
        expr->emit();
    }
}

void AssignExprNode::emitStackCode() const {
    std::cout << "Implement stack logic for AssignExprNode." << std::endl;
}

void AssignExprNode::emit() const {
    std::cout << id << " = ";
    rightExpr->emit();
}

void VarExprNode::emitStackCode() const {
    std::cout << "Implement stack logic for VarExprNode." << std::endl;
}

void VarExprNode::emit() const {
    std::cout << id;
}

void IfStmtNode::emitStackCode() const {
    std::cout << "Implement stack logic for IfStmtNode." << std::endl;
}

void IfStmtNode::emit() const {
    std::cout << "if (";
    condition->emit();
    std::cout << ") ";
    thenStmt->emit();
    if (elseStmt) {
        std::cout << " else ";
        elseStmt->emit();
    }
}

void WhileStmtNode::emitStackCode() const {
    std::cout << "Implement stack logic for WhileStmtNode." << std::endl;
}

void WhileStmtNode::emit() const {
    std::cout << "while (";
    condition->emit();
    std::cout << ") ";
    body->emit();
}

void ReturnStmtNode::emitStackCode() const {
    if (expr) expr->emitStackCode();
    std::cout << "Implement stack logic for ReturnStmtNode." << std::endl;
}

void ReturnStmtNode::emit() const {
    std::cout << "return";
    if (expr) {
        std::cout << " ";
        expr->emit();
    }
    std::cout << ";" << std::endl;
}

void BinExprNode::emitStackCode() const {
    left->emitStackCode();
    right->emitStackCode();
    std::cout << "Implement stack logic for BinExprNode." << std::endl;
}

void BinExprNode::emit() const {
    std::cout << "(";
    left->emit();
    std::cout << " " << toString(oper) << " ";
    right->emit();
    std::cout << ")";
}

void NumberExprNode::emitStackCode() const {
    std::cout << "PUSH " << value << std::endl;
}

void NumberExprNode::emit() const {
    std::cout << value;
}

void CallExprNode::emitStackCode() const {
    for (const auto& arg : arguments) {
        arg->emitStackCode();
    }
    std::cout << "PUSH" << arguments.size() << std::endl;
    std::cout << "CALL " << id << std::endl;
}

void CallExprNode::emit() const {
    std::cout << id << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        arguments[i]->emit();
        if (i + 1 < arguments.size()) std::cout << ", ";
    }
    std::cout << ")";
}
