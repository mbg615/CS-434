#include "Parser.hpp"

#include <stdexcept>
#include <vector>

Parser::Parser(Lexer &lexer) : lexer(lexer) {
    advance();
}

void Parser::advance() {
    currentToken = lexer.lex();
}

void Parser::expect(TokenType expectedType) {
    if(currentToken.getToken() != expectedType) {
        throw std::runtime_error("Unexpected Token");
    }
}

std::vector<ASTPtr> Parser::parseProgram() {
    std::vector<ASTPtr> stmts;

    while (currentToken.getToken() != TokenType::END_OF_FILE) {
        stmts.push_back(parseStmt());
    }

    return stmts;
}

ASTPtr Parser::parseExpr() {
    auto node = parseTerm();
    while (currentToken.getToken() == TokenType::PLUS || currentToken.getToken() == TokenType::MINUS) {
        TokenType op = currentToken.getToken();
        advance();
        auto rhs = parseTerm();
        node = std::make_unique<BinExprNode>(op, std::move(node), std::move(rhs));
    }
    return node;
}

ASTPtr Parser::parseTerm() {
    auto node = parseFactor();
    while (currentToken.getToken() == TokenType::ASTERISK || currentToken.getToken() == TokenType::FORWARD_SLASH) {
        TokenType op = currentToken.getToken();
        advance();
        auto rhs = parseFactor();
        node = std::make_unique<BinExprNode>(op, std::move(node), std::move(rhs));
    }
    return node;
}

ASTPtr Parser::parseFactor() {
    if (currentToken.getToken() == TokenType::INT_LITERAL) {
        int value = std::stoi(currentToken.getLexeme());
        advance();
        return std::make_unique<LiteralExprNode>(value);
    }
    else if (currentToken.getToken() == TokenType::STRING_LITERAL) {
        std::string value = currentToken.getLexeme();
        advance();
        return std::make_unique<LiteralExprNode>(std::move(value));
    }
    else if (currentToken.getToken() == TokenType::LEFT_PAREN) {
        advance();
        auto expr = parseExpr();
        expect(TokenType::RIGHT_PAREN);
        advance();
        return expr;
    }
    else {
        throw std::runtime_error("Unexpected token in factor");
    }
}

ASTPtr Parser::parseStmt() {
    switch (currentToken.getToken()) {
        case TokenType::IF:
            return parseIfStmt();

        case TokenType::WHILE:
            return parseWhileStmt();

        case TokenType::RETURN:
            // TODO: parse return-statement
            throw std::runtime_error("parseReturnStatement() not implemented yet");

        case TokenType::LEFT_BRACE:
            return parseBlock();

        case TokenType::INT:
            // TODO: parse variable declaration
            throw std::runtime_error("parseVariableDeclaration() not implemented yet");

        default:
            // Fallback: parse expression statement
            auto expr = parseExpr();
            expect(TokenType::SEMICOLON);
            advance();
            return std::make_unique<ExprStmtNode>(std::move(expr));
    }
}

ASTPtr Parser::parseBlock() {
    expect(TokenType::LEFT_BRACE);
    advance();

    std::vector<ASTPtr> stmts;

    while(currentToken.getToken() != TokenType::RIGHT_BRACE) {
        stmts.push_back(parseStmt());
    }

    expect(TokenType::RIGHT_BRACE);
    advance();

    return std::make_unique<BlockNode>(std::move(stmts));
}

ASTPtr Parser::parseIfStmt() {
    expect(TokenType::IF);
    advance();

    expect(TokenType::LEFT_PAREN);
    advance();

    auto condition = parseExpr();

    expect(TokenType::RIGHT_PAREN);
    advance();

    auto thenBranch = parseStmt();

    ASTPtr elseBranch = nullptr;
    if(currentToken.getToken() == TokenType::ELSE) {
        advance();
        elseBranch = parseStmt();
    }

    return std::make_unique<IfNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

ASTPtr Parser::parseWhileStmt() {
    expect(TokenType::WHILE);
    advance();

    expect(TokenType::LEFT_PAREN);
    advance();

    auto cond = parseExpr();

    expect(TokenType::RIGHT_PAREN);
    advance();

    auto body = parseStmt();
    return std::make_unique<WhileNode>(std::move(cond), std::move(body));
}
