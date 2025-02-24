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

#include <iostream>
#include <memory>
#include <vector>
#include "../Token.hpp"

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

using ASTPtr = std::unique_ptr<ASTNode>;

class Number : ASTNode {
public:
    int value;
    explicit Number(int val) : value(val) {}
};

class BinaryOp : ASTNode {
public:
    ASTPtr left;
    char op;
    ASTPtr right;

    BinaryOp(ASTPtr l, char oper, ASTPtr r) : left(std::move(l)), op(oper), right(std::move(r)) {}
};



class Parser {
private:
    size_t tokenIndex = 0;
    Token currentToken = {TokenType::END_OF_FILE, "Initial Value", -1, -1};
    std::vector<Token> tokenStream;

    Token consume(TokenType expectedType) {
        // todo if (currentToken.getToken() != expectedType) throw std::runtime_error("Expected token of type " + expectedType. + " but got " + currentToken.toString());
    }

    ASTPtr parseExpr() {

        switch(currentToken.getToken()) {
            case TokenType::PLUS:
                ASTPtr
                break;
            case TokenType::MINUS:
                break;
            case TokenType::INT_LITERAL:
                break;
            default:
                throw std::runtime_error("Invalid Token");
        }

    }

    ASTPtr parseTerm() {

        switch(currentToken.getToken()) {
            case TokenType::ASTERISK:
                break;
            case TokenType::FORWARD_SLASH:
                break;
            case TokenType::INT_LITERAL:
                break;
            default:
                throw std::runtime_error("Invalid Token");
        }

    }

    ASTPtr parseFactor() {

        switch(currentToken.getToken()) {
            case TokenType::LEFT_PAREN:
                break;
            case TokenType::INT_LITERAL:
                int value =




                break;
            default:
                throw std::runtime_error("Invalid Token");
        }

    }

public:
    
};

int main() {
    return 0;
}
