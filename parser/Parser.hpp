#ifndef PARSER_HPP
#define PARSER_HPP

#include "../Lexer/Lexer.hpp"
#include "AST.hpp"

class Parser {
private:
    Lexer& lexer;
    Token currentToken;

    void advance();
    void expect(TokenType expectedType);

public:
    explicit Parser(Lexer &lexer);

    std::vector<ASTPtr> parseProgram();

    ASTPtr parseStmt();
    ASTPtr parseBlock();
    ASTPtr parseIfStmt();
    ASTPtr parseWhileStmt();

    ASTPtr parseExpr();
    ASTPtr parseTerm();
    ASTPtr parseFactor();

};

#endif //PARSER_HPP
