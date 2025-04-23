#include "Parser.hpp"

#include <stdexcept>
#include <vector>

Parser::Parser(Lexer &lexer) : lexer(lexer) {
    advance();
}

void Parser::advance() {
    if (hasBuffered) {
        currentToken = bufferedToken;
        hasBuffered = false;
    } else {
        currentToken = lexer.lex();
    }
//    currentToken = lexer.lex();
//    if(currentToken.getToken() == TokenType::LINE_COMMENT || currentToken.getToken() == TokenType::BLOCK_COMMENT) {
//        advance();
//    }
}

Token Parser::peek() {
    if(!hasBuffered) {
        bufferedToken = lexer.lex();
        hasBuffered = true;
    }
    return bufferedToken;
}

void Parser::expect(TokenType expectedType) {
    if(currentToken.getToken() != expectedType) {
        throw std::runtime_error("Unexpected token: " + toString(currentToken.getToken()) + ", expected: " + toString(expectedType));
    }
}

void Parser::declareVariable(const std::string &varName) {
    if (variableOffsets.find(varName) != variableOffsets.end()) {
        throw std::runtime_error("Variable already declared: " + varName);
    }
    variableOffsets[varName] = currentVarOffset++;
}

int Parser::variableOffset(const std::string& varName) const {
    auto it = variableOffsets.find(varName);
    if (it == variableOffsets.end()) {
        throw std::runtime_error("Undeclared variable: " + varName);
    }
    return it->second;
}

std::vector<ASTPtr> Parser::parseProgram() {
    std::vector<ASTPtr> functions;

    while (currentToken.getToken() != TokenType::END_OF_FILE) {
        functions.push_back(parseFunction());
    }

    return functions;
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
    while (currentToken.getToken() == TokenType::ASTERISK || currentToken.getToken() == TokenType::FORWARD_SLASH || currentToken.getToken() == TokenType::PERCENT) {
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
    else if(currentToken.getToken() == TokenType::IDENTIFIER) {
        std::string varName = currentToken.getLexeme();
        advance();

        if(currentToken.getToken() == TokenType::LEFT_PAREN) {
            advance();

            std::vector<ASTPtr> args;

            if (currentToken.getToken() != TokenType::RIGHT_PAREN) {
                args.push_back(parseComparison());

                while (currentToken.getToken() == TokenType::COMMA) {
                    advance();
                    args.push_back(parseComparison());
                }
            }

            expect(TokenType::RIGHT_PAREN);
            advance();

            return std::make_unique<FunctionCallNode>(varName, std::move(args));

        }

        return std::make_unique<VarExprNode>(varName, variableOffset(varName));
    }
    else if (currentToken.getToken() == TokenType::LEFT_PAREN) {
        advance();
        auto expr = parseComparison();
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
            return parseReturn();

        case TokenType::LEFT_BRACE:
            return parseBlock();

        case TokenType::INT:
            return parseVarDecl();

        case TokenType::IDENTIFIER: {
            Token ident = currentToken;

            Token next = peek();
            if (next.getToken() == TokenType::ASSIGN) {
                return parseAssignment(); // already at IDENTIFIER, safe to proceed
            }

            // Not an assignment → must be a function call or expression
            auto expr = parseComparison();
            expect(TokenType::SEMICOLON);
            advance();
            return std::make_unique<ExprStmtNode>(std::move(expr));
        }
//            return parseAssignment();

        default:
            // Fallback: parse expression statement
            auto expr = parseComparison();
            expect(TokenType::SEMICOLON);
            advance();
            return std::make_unique<ExprStmtNode>(std::move(expr));
    }
}

ASTPtr Parser::parseBlock() {
    expect(TokenType::LEFT_BRACE);
    advance();

    std::vector<ASTPtr> stmts;

    while(currentToken.getToken() == TokenType::INT) {
        stmts.push_back(parseVarDecl());
    }

    while(currentToken.getToken() != TokenType::RIGHT_BRACE) {
        if (currentToken.getToken() == TokenType::INT) {
            throw std::runtime_error("Error: Variable declarations must appear before any statements.");
        }
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

    auto condition = parseComparison();

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

    auto cond = parseComparison();

    expect(TokenType::RIGHT_PAREN);
    advance();

    auto body = parseStmt();
    return std::make_unique<WhileNode>(std::move(cond), std::move(body));
}

ASTPtr Parser::parseVarDecl() {
    expect(TokenType::INT);
    advance();

    if (currentToken.getToken() != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected variable name after type");
    }
    std::string varName = currentToken.getLexeme();
    advance();

    ASTPtr initializer = nullptr;
    if (currentToken.getToken() == TokenType::ASSIGN) {
        advance();
        initializer = parseComparison();
    }

    expect(TokenType::SEMICOLON);
    advance();

    declareVariable(varName);
    if (!initializer) initializer = std::make_unique<LiteralExprNode>(0);

    return std::make_unique<VarDeclNode>(varName, std::move(initializer), variableOffset(varName));
}

ASTPtr Parser::parseAssignment() {
    if (currentToken.getToken() != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected variable name in assignment");
    }

    std::string varName = currentToken.getLexeme();
    advance();

    expect(TokenType::ASSIGN);
    advance();

    auto expr = parseComparison();

    expect(TokenType::SEMICOLON);
    advance();

    return std::make_unique<AssignNode>(variableOffset(varName), std::move(expr));

}

ASTPtr Parser::parseComparison() {
    auto node = parseExpr();

    while(currentToken.getToken() == TokenType::EQUALS || currentToken.getToken() == TokenType::NOT_EQUALS || currentToken.getToken() == TokenType::LESS || currentToken.getToken() == TokenType::LESS_EQUALS || currentToken.getToken() == TokenType::GREATER || currentToken.getToken() == TokenType::GREATER_EQUALS) {
        TokenType op = currentToken.getToken();
        advance();
        auto rhs = parseExpr();
        node = std::make_unique<BinExprNode>(op, std::move(node), std::move(rhs));
    }

    return node;
}

ASTPtr Parser::parseReturn() {
    expect(TokenType::RETURN);
    advance();

    ASTPtr expr = nullptr;

    if(currentToken.getToken() != TokenType::SEMICOLON) {
        expr = parseComparison();
    }

    expect(TokenType::SEMICOLON);
    advance();

    return std::make_unique<ReturnNode>(std::move(expr));
}

ASTPtr Parser::parseFunction() {
    std::string returnType;
    if(currentToken.getToken() == TokenType::INT) returnType = "int";
    else if(currentToken.getToken() == TokenType::VOID) returnType = "void";
    else throw std::runtime_error("Expected return type (int or void) at start of function");
    advance();

    expect(TokenType::IDENTIFIER);
    std::string name = currentToken.getLexeme();
    advance();

    expect(TokenType::LEFT_PAREN);
    advance();

    std::vector<std::string> params;
    if(currentToken.getToken() != TokenType::RIGHT_PAREN) {
        // ToDo: Edit this for float support.
        expect(TokenType::INT);
        advance();
        expect(TokenType::IDENTIFIER);
        params.push_back(currentToken.getLexeme());
        advance();

        while(currentToken.getToken() == TokenType::COMMA) {
            advance();
            expect(TokenType::INT);
            advance();
            expect(TokenType::IDENTIFIER);
            params.push_back(currentToken.getLexeme());
            advance();
        }
    }

    expect(TokenType::RIGHT_PAREN);
    advance();

    variableOffsets.clear();
    currentVarOffset = 0;

    for(const auto& parm: params) {
        variableOffsets[parm] = currentVarOffset++;
    }

    auto body = parseBlock();

    return std::make_unique<FunctionNode>(returnType, name, params, std::move(body));
}
