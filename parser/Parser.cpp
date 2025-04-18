/*
 * Parser Source File
 * ------------------
 * This file implements the syntax analyzer (parser) for the language,
 * providing the grammar rules and semantic actions to build the AST.
 *
 * The parser takes tokens produced by the scanner and organizes them
 * according to the language’s grammar. It constructs the abstract syntax tree
 * (AST) representation of the program, which serves as the foundation
 * for further stages such as semantic analysis and code generation.
 */

#include <memory>

#include "ast.hpp"
#include "Parser.hpp"

using ASTPtr = std::unique_ptr<AST>;

Parser::Parser(const std::string &filename) : lexer(filename) {}

void Parser::advance() {
    currentToken = lexer.lex();
}

bool Parser::isTypeSpecifier(const Token &token) {
    return token.getToken() == TokenType::INT || token.getToken() == TokenType::VOID;
}

bool Parser::isStmtStart(const Token &token) {
    // Check for any statement types
    return token.getToken() == TokenType::LEFT_BRACE ||
    token.getToken() == TokenType::IF ||
    token.getToken() == TokenType::WHILE ||
    token.getToken() == TokenType::RETURN ||
    token.getToken() == TokenType::IDENTIFIER ||
    token.getToken() == TokenType::INT_LITERAL ||
    token.getToken() == TokenType::LEFT_PAREN;
}

bool Parser::isRelOp(const Token &token) {
    return token.getToken() == TokenType::EQUALS ||
           token.getToken() == TokenType::NOT_EQUALS ||
           token.getToken() == TokenType::GREATER ||
           token.getToken() == TokenType::GREATER_EQUALS ||
           token.getToken() == TokenType::LESS ||
           token.getToken() == TokenType::LESS_EQUALS;
}

void Parser::expect(TokenType expectedType) {
    if(currentToken.getToken() != expectedType) {
        throw std::runtime_error("Expected token " + toString(expectedType) +
                                 ", got " + currentToken.getLexeme());
    }
    advance();
}

void Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    program->declarations = parseDeclarationList();
    ast = std::move(program);
}

std::vector<ASTPtr> Parser::parseDeclarationList() {
    std::vector<ASTPtr> declarations;

    while(isTypeSpecifier(currentToken)) {
        Type type = parseTypeSpecifier();
        std::string id = currentToken.getLexeme();
        expect(TokenType::IDENTIFIER);
        declarations.push_back(parseDeclaration(type, id));
    }

    return declarations;
}

Type Parser::parseTypeSpecifier() {
    if(currentToken.getToken() == TokenType::INT) {
        advance();
        return Type::INT;
    } else if(currentToken.getToken() == TokenType::VOID) {
        advance();
        return Type::VOID;
    } else {
        throw std::runtime_error("Expected type specifier: int or void");
    }
}

ASTPtr Parser::parseDeclaration(Type type, const std::string &id) {
    if(currentToken.getToken() == TokenType::IDENTIFIER) {
        advance();
        return std::make_unique<VarDeclarationNode>(type, id);
    } else if(currentToken.getToken() == TokenType::LEFT_PAREN) {
        return parseFunDeclaration(type, id);
    } else {
        throw std::runtime_error("Unexpected token in declaration after identifier: " + currentToken.toString());
    }
}

ASTPtr Parser::parseFunDeclaration(Type t, const std::string &id) {
    expect(TokenType::LEFT_PAREN);

    std::vector<std::unique_ptr<ParamNode>> params = parseParams();

    expect(TokenType::RIGHT_PAREN);

    auto body = parseCompoundStmt();

    return std::make_unique<FunDeclNode>(t, id, std::move(params), std::move(body));
}

std::vector<std::unique_ptr<ParamNode>> Parser::parseParams() {
    std::vector<std::unique_ptr<ParamNode>> params;

    if(currentToken.getToken() != TokenType::RIGHT_PAREN) {
        params.push_back(parseParam());
        while(currentToken.getToken() == TokenType::COMMA) {
            advance();
            params.push_back(parseParam());
        }
    }

    return params;
}

std::unique_ptr<ParamNode> Parser::parseParam() {
    Type type = parseTypeSpecifier();
    std::string id = currentToken.getLexeme();
    expect(TokenType::IDENTIFIER);

    return std::make_unique<ParamNode>(type, id);
}

ASTPtr Parser::parseCompoundStmt() {
    expect(TokenType::LEFT_BRACE);

    std::vector<ASTPtr> localDecls = parseLocalDeclarations();
    std::vector<ASTPtr> stmts = parseStmtList();

    expect(TokenType::RIGHT_BRACE);

    return std::make_unique<CompoundStmtNode>(std::move(localDecls), std::move(stmts));
}

std::vector<ASTPtr> Parser::parseLocalDeclarations() {
    std::vector<ASTPtr> localDecls;

    while(isTypeSpecifier(currentToken)) {
        Type type = parseTypeSpecifier();
        std::string id = currentToken.getLexeme();
        expect(TokenType::IDENTIFIER);
        localDecls.push_back(parseVarDeclaration(type, id));
    }

    return localDecls;
}

ASTPtr Parser::parseVarDeclaration(Type t, const std::string &id) {
    if(currentToken.getToken() == TokenType::SEMICOLON) {
        advance();
        return std::make_unique<VarDeclarationNode>(t, id);
    } else {
        throw std::runtime_error("Invalid variable declaration syntax.");
    }
}

std::vector<ASTPtr> Parser::parseStmtList() {
    std::vector<ASTPtr> stmtList;

    while(isStmtStart(currentToken)) {
        stmtList.push_back(parseStmt());
    }

    return stmtList;
}

ASTPtr Parser::parseStmt() {
    switch (currentToken.getToken()) {
        case TokenType::IF:
            return parseSelectionStmt();
        case TokenType::WHILE:
            return parseIterStmt();
        case TokenType::RETURN:
            return parseReturnStmt();
        case TokenType::LEFT_BRACE:
            return parseCompoundStmt();
        default:
            return parseExprStmt();
    }
}

ASTPtr Parser::parseExprStmt() {
    if(currentToken.getToken() == TokenType::SEMICOLON) {
        advance();
        return std::make_unique<ExprStmtNode>();
    }

    ASTPtr expr = parseExpr();
    expect(TokenType::SEMICOLON);
    return std::make_unique<ExprStmtNode>(std::move(expr));
}

ASTPtr Parser::parseExpr() {
    if(currentToken.getToken() == TokenType::IDENTIFIER) {
        std::string id = currentToken.getLexeme();
        advance();

        if(currentToken.getToken() == TokenType::ASSIGN) {
            advance();
            ASTPtr rightExpr = parseExpr();
            return std::make_unique<AssignExprNode>(id, std::move(rightExpr));
        } else {
            return std::make_unique<VarExprNode>(id);
        }
    }

    return parseSimpleExpr();
}

ASTPtr Parser::parseSelectionStmt() {
    expect(TokenType::IF);
    expect(TokenType::LEFT_PAREN);

    ASTPtr cond = parseExpr();

    expect(TokenType::RIGHT_PAREN);

    ASTPtr thenStmt = parseStmt();

    ASTPtr elseStmt = nullptr;
    if(currentToken.getToken() == TokenType::ELSE) {
        advance();
        elseStmt = parseStmt();
    }

    return std::make_unique<IfStmtNode>(std::move(cond), std::move(thenStmt), std::move(elseStmt));
}

ASTPtr Parser::parseIterStmt() {
    expect(TokenType::WHILE);
    expect(TokenType::LEFT_PAREN);

    ASTPtr cond = parseExpr();

    expect(TokenType::RIGHT_PAREN);

    ASTPtr body = parseStmt();

    return std::make_unique<WhileStmtNode>(std::move(cond), std::move(body));
}

ASTPtr Parser::parseReturnStmt() {
    expect(TokenType::RETURN);

    if(currentToken.getToken() == TokenType::SEMICOLON) {
        advance();
        return std::make_unique<ReturnStmtNode>();
    }

    ASTPtr expr = parseExpr();
    expect(TokenType::SEMICOLON);

    return std::make_unique<ReturnStmtNode>(std::move(expr));
}

ASTPtr Parser::parseSimpleExpr() {
    ASTPtr left = parseAdditiveExpr();

    if(isRelOp(currentToken)) {
        TokenType op = currentToken.getToken();
        advance();
        ASTPtr right = parseAdditiveExpr();

        return std::make_unique<BinExprNode>(op, std::move(left), std::move(right));
    }

    return left;
}

ASTPtr Parser::parseAdditiveExpr() {
    ASTPtr left = parseTerm();

    while (currentToken.getToken() == TokenType::PLUS || currentToken.getToken() == TokenType::MINUS) {
        TokenType op = currentToken.getToken();
        advance();
        ASTPtr right = parseTerm();
        left = std::make_unique<BinExprNode>(op, std::move(left), std::move(right));
    }

    return left;
}

ASTPtr Parser::parseTerm() {
    ASTPtr left = parseFactor();  // Start with a factor

    while (currentToken.getToken() == TokenType::ASTERISK || currentToken.getToken() == TokenType::FORWARD_SLASH) {
        TokenType op = currentToken.getToken();
        advance();
        ASTPtr right = parseFactor();
        left = std::make_unique<BinExprNode>(op, std::move(left), std::move(right));
    }

    return left;
}

ASTPtr Parser::parseFactor() {
    if (currentToken.getToken() == TokenType::LEFT_PAREN) {
        advance();
        ASTPtr expr = parseExpr();
        expect(TokenType::RIGHT_PAREN);
        return expr;
    }

    if (currentToken.getToken() == TokenType::INT_LITERAL) {
        int value = std::stoi(currentToken.getLexeme());
        advance();
        return std::make_unique<NumberExprNode>(value);
    }

    if (currentToken.getToken() == TokenType::IDENTIFIER) {
        std::string id = currentToken.getLexeme();
        advance();

        if (currentToken.getToken() == TokenType::LEFT_PAREN) {
            return parseCall(currentToken.getLexeme());
        } else {
            return std::make_unique<VarExprNode>(id);
        }

        return std::make_unique<VarExprNode>(id);
    }

    throw std::runtime_error("Unexpected token in factor: " + currentToken.getLexeme());
};

ASTPtr Parser::parseCall(const std::string& id) {
    expect(TokenType::LEFT_PAREN);

    std::vector<ASTPtr> arguments;

    if (currentToken.getToken() != TokenType::RIGHT_PAREN) {
        arguments.push_back(parseExpr());
        while (currentToken.getToken() == TokenType::COMMA) {
            advance();
            arguments.push_back(parseExpr());
        }
    }

    expect(TokenType::RIGHT_PAREN);

    return std::make_unique<CallExprNode>(id, std::move(arguments));
}

int main() {
    Parser myParser("test.c");
    myParser.parse();
    return 0;
}
