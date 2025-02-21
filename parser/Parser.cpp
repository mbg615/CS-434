/*
 * Parser Source File
 * ------------------
 * This file implements the recursive descent parser for the language,
 * providing the `parse()` function to generate an Abstract Syntax Tree (AST)
 * from the tokenized input.
 *
 * The parser processes a sequence of tokens produced by the scanner and
 * organizes them into a structured representation based on the language's
 * grammar rules. It follows a top-down parsing approach, handling expressions,
 * operators, and precedence through recursive function calls.
 *
 * The resulting AST can be used for further processing, such as code generation
 * or interpretation.
 */

#include <memory>
#include "../Token.hpp"

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class Number : ASTNode {
public:
    int value;
    explicit Number(int val) : value(val) {}
};

class BinaryOp : ASTNode {
public:
    std::unique_ptr<ASTNode> left;
    char op;
    std::unique_ptr<ASTNode> right;

    BinaryOp(std::unique_ptr<ASTNode> l, char oper, std::unique_ptr<ASTNode> r) : left(std::move(l)), op(oper), right(std::move(r)) {}
};

class Parser {
    
};

int main() {
    return 0;
}
