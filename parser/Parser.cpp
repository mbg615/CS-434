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

class Number : public ASTNode {
public:
    int value;
    explicit Number(int val) : value(val) {}
};

class BinaryOp : public ASTNode {
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

    void consume() {
        if(tokenIndex < tokenStream.size()) currentToken = tokenStream[tokenIndex++];
        else currentToken = {TokenType::END_OF_FILE, "", -1, -1};
    }

    bool accept(TokenType type) {
        if(currentToken.getToken() == type) {
            consume();
            return true;
        }
        return false;
    }

    ASTPtr parseFactor() {
        if(accept(TokenType::INT_LITERAL)) {
            return std::make_unique<Number>(std::stoi(currentToken.getLexeme()));
        }
        if(accept(TokenType::LEFT_PAREN)) {
            ASTPtr expr = parseExpr();
            if(!accept(TokenType::RIGHT_PAREN)) throw std::runtime_error("Expected closing parenthesis");
            return expr;
        }
        throw std::runtime_error("Unexpected token " + currentToken.toString() + " in factor");
    }

    ASTPtr parseTerm() {
        ASTPtr factor = parseFactor();
        Token prevToken = currentToken;
        while(accept(TokenType::ASTERISK) || accept(TokenType::FORWARD_SLASH)) {
            char op = prevToken.getLexeme()[0];
            ASTPtr right = parseFactor();
            factor = std::make_unique<BinaryOp>(std::move(factor), op, std::move(right));
        }
        return factor;
    }

    ASTPtr parseExpr() {
        ASTPtr term = parseTerm();
        Token prevToken = currentToken;
        while(accept(TokenType::PLUS) || accept(TokenType::MINUS)) {
            char op = prevToken.getLexeme()[0];
            ASTPtr right = parseTerm();
            term = std::make_unique<BinaryOp>(std::move(term), op, std::move(right));
        }
        return term;
    }

public:
    ASTPtr parse() {
        return parseExpr();
    }

};
