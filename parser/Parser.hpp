#ifndef PARSER_HPP
#define PARSER_HPP

#include "../Lexer/Lexer.hpp"
#include "AST.hpp"

class Parser {
private:
    Lexer& lexer;
    Token currentToken;
    Token bufferedToken;

    bool hasBuffered = false;

    void advance();
    Token peek();
    void expect(TokenType expectedType);

    std::unordered_map<std::string, int> variableOffsets;
    int currentVarOffset = 0;

    void declareVariable(const std::string& varName);
    [[nodiscard]] int variableOffset(const std::string& varName) const;


public:
    explicit Parser(Lexer &lexer);

    std::vector<ASTPtr> parseProgram();

    ASTPtr parseStmt();
    ASTPtr parseBlock();
    ASTPtr parseIfStmt();
    ASTPtr parseWhileStmt();
    ASTPtr parseVarDecl();
    ASTPtr parseAssignment();
    ASTPtr parseComparison();
    ASTPtr parseReturn();
    ASTPtr parseFunction();
    ASTPtr parseExpr();
    ASTPtr parseTerm();
    ASTPtr parseFactor();
};

#endif //PARSER_HPP
