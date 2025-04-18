//
// Created by Maddox Guthrie on 4/6/25.
//

#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

#include "../Token.hpp"
#include "../lexicalAnalyzer/lexicalAnalyzer.cpp"

using ASTPtr = std::unique_ptr<AST>;

class Parser {
private:
    Token currentToken;
    Lexer lexer;
    ASTPtr ast;

    void advance();
    void expect(TokenType expectedType);

    static bool isTypeSpecifier(const Token &token);
    static bool isStmtStart(const Token &token);
    static bool isRelOp(const Token &token);

public:
    explicit Parser(const std::string &filename);

    void parse();

    std::vector<ASTPtr> parseDeclarationList();
    ASTPtr parseDeclaration(Type t, const std::string &id);
    ASTPtr parseVarDeclaration(Type t, const std::string &id);
    ASTPtr parseFunDeclaration(Type t, const std::string &id);
    Type parseTypeSpecifier();
    std::vector<std::unique_ptr<ParamNode>> parseParams();
    std::unique_ptr<ParamNode> parseParam();
    ASTPtr parseCompoundStmt();
    std::vector<ASTPtr> parseLocalDeclarations();
    std::vector<ASTPtr> parseStmtList();
    ASTPtr parseStmt();
    ASTPtr parseExprStmt();
    ASTPtr parseSelectionStmt();
    ASTPtr parseIterStmt();
    ASTPtr parseReturnStmt();
    ASTPtr parseExpr();
    ASTPtr parseSimpleExpr();
    ASTPtr parseAdditiveExpr();
    ASTPtr parseTerm();
    ASTPtr parseFactor();
    ASTPtr parseCall(const std::string& id);

};

#endif //PARSER_HPP
