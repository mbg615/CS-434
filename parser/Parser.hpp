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

    ASTPtr parseExpression();
    ASTPtr parseTerm();
    ASTPtr parseFactor();

};


#endif //PARSER_HPP
