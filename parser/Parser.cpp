#include "Parser.hpp"

#include <stdexcept>

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

ASTPtr Parser::parseExpression() {
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
        auto expr = parseExpression();
        expect(TokenType::RIGHT_PAREN);
        advance();
        return expr;
    }
    else {
        throw std::runtime_error("Unexpected token in factor");
    }
}
