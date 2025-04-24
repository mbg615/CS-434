/*
 * Scanner Source File
 * -------------------
 * This file implements the lexical analyzer (scanner) for the language,
 * providing the `lex()` function to tokenize input.
 *
 * The scanner reads characters from the input and groups them into meaningful
 * tokens for the Parser to process. It handles keywords, identifiers,
 * literals, operators, and other language constructs.
 */

#include <string>
#include <fstream>
#include <unordered_map>

#include "Lexer.hpp"
#include "../Token.hpp"

void Lexer::loadSource(const std::string &filename) {
    std::ifstream source(filename);
    if(!source) throw std::runtime_error("Could not open file: " + filename);

    source_ = std::string((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
}

char Lexer::advance() {
    if(pos_ < source_.size()) {
        char c = source_[pos_++];
        column_++;

        if(c == '\n') {
            column_ = 1;
            line_++;
        }
        return c;
    }
    return '\0';
}

char Lexer::peek() { return (pos_ < source_.size()) ? source_[pos_] : '\0'; }

bool Lexer::match(char expected) {
    if(pos_ < source_.size() && peek() == expected) {
        pos_++;
        column_++;
        return true;
    }
    return false;
}

Lexer::Lexer(const std::string &source) : pos_(0), line_(1), column_(1) {
    loadSource(source);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-case-range"
Token Lexer::lex() {
    while(pos_ < source_.size()) {
        char c = advance();

        switch(c) {
            case '(':
                return {TokenType::LEFT_PAREN, c, line_, column_};
            case ')':
                return {TokenType::RIGHT_PAREN, c, line_, column_};
            case '{':
                return {TokenType::LEFT_BRACE, c, line_, column_};
            case '}':
                return {TokenType::RIGHT_BRACE, c, line_, column_};
            case '[':
                return {TokenType::LEFT_BRACKET, c, line_, column_};
            case ']':
                return {TokenType::RIGHT_BRACKET, c, line_, column_};
            case '?':
                return {TokenType::TERNARY, c, line_, column_};
            case ',':
                return {TokenType::COMMA, c, line_, column_};
            case ';':
                return {TokenType::SEMICOLON, c, line_, column_};
            case ':':
                return {TokenType::COLON, c, line_, column_};

            case '<':
                return match('=') ? Token{TokenType::LESS_EQUALS, "<=", line_, column_} : Token{TokenType::LESS, '<', line_, column_};
            case '>':
                return match('=') ? Token{TokenType::GREATER_EQUALS, ">=", line_, column_} : Token{TokenType::GREATER, '>', line_, column_};
            case '!':
                return match('=') ? Token{TokenType::NOT_EQUALS, "!=", line_, column_} : Token{TokenType::NOT, '!', line_, column_};
            case '*':
                return match('=') ? Token{TokenType::MULT_EQUALS, "*=", line_, column_} : Token{TokenType::ASTERISK, '*', line_,column_};
            case '=':
                return match('=') ? Token{TokenType::EQUALS, "==", line_, column_} : Token{TokenType::ASSIGN, '=',line_, column_};
            case '%':
                return match('=') ? Token{TokenType::MOD_EQUALS, "%=", line_, column_} : Token{TokenType::PERCENT,'%', line_, column_};
            case '&':
                return match('&') ? Token{TokenType::AND, "&&", line_, column_} : Token{TokenType::ERROR,'&', line_, column_};
            case '|':
                return match('|') ? Token{TokenType::OR, "||", line_, column_} : Token{TokenType::ERROR,'|', line_, column_};

            case '\'': {
                std::string lexeme = "'";
                advance();
                while (pos_ < source_.size()) {
                    if (source_[pos_] == '\\') {
                        lexeme += advance();
                        lexeme += advance();
                    }
                    else if (source_[pos_] == '\'') {
                        lexeme += advance();
                        break;
                    }
                    else {
                        lexeme += advance();
                    }
                }
                return {TokenType::CHAR_LITERAL, lexeme, line_, column_};
            }

            case '"': {
                std::string lexeme = "\"";
                lexeme += advance();
                while (pos_ < source_.size()) {
                    if (source_[pos_] == '\\') {
                        lexeme += advance();
                        lexeme += advance();
                    }
                    else if (source_[pos_] == '\"') {
                        lexeme += advance();
                        break;
                    }
                    else {
                        lexeme += advance();
                    }
                }
                return {TokenType::STRING_LITERAL, lexeme, line_, column_};
            }

            case '/': {
                if (match('/')) {
                    std::string lexeme = "//";
                    while (pos_ < source_.size() && peek() != '\n') {
                        lexeme += peek();
                        advance();
                    }
                    return {TokenType::LINE_COMMENT, lexeme, line_, column_};
                }
                if (match('*')) {
                    std::string lexeme = "/*";
                    while (pos_ < source_.size() - 1) {
                        if (source_[pos_] == '*' && source_[pos_ + 1] == '/') {
                            lexeme += advance();
                            lexeme += advance();
                            break;
                        }
                        lexeme += advance();
                    }
                    return {TokenType::BLOCK_COMMENT, lexeme, line_, column_};
                }
                if (match('=')) {
                    return {TokenType::DIV_EQUALS, "/=", line_, column_};
                }
                return {TokenType::FORWARD_SLASH, "/", line_, column_};
            }

            case '0' ... '9': {
                std::string lexeme(1,c);
                while (pos_ < source_.size() && isdigit(peek())) lexeme += advance();
                if(match('.')) {
                    lexeme += '.';
                    while (pos_ < source_.size() && isdigit(peek())) lexeme += advance();
                    return {TokenType::FLOAT_LITERAL, lexeme, line_, column_};
                }
                return {TokenType::INT_LITERAL, lexeme, line_, column_};
            }

            case '.': {
                if(isdigit(peek())) {
                    std::string lexeme(1,c);
                    while (pos_ < source_.size() && isdigit(peek())) lexeme += advance();
                    return {TokenType::FLOAT_LITERAL, lexeme, line_, column_};
                }
                return {TokenType::DOT, c, line_, column_};
            }

            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_': {
                std::string lexeme(1, c);
                while(pos_ < source_.size() && (std::isalnum(source_[pos_]) || source_[pos_] == '_')) lexeme += advance();
                TokenType type = keywords.contains(lexeme) ? keywords.at(lexeme) : TokenType::IDENTIFIER;
                return {type, lexeme, line_, column_};
            }

            case '+':
                if(match('+')) {
                    return {TokenType::INCREMENT, "++", line_, column_};
                }
                if(match('=')) {
                    return {TokenType::PLUS_EQUALS, "+=", line_, column_};
                }
                return {TokenType::PLUS, "+", line_, column_};

            case '-':
                if(match('-')) {
                    return {TokenType::DECREMENT, "--", line_, column_};
                }
                if(match('=')) {
                    return {TokenType::MINUS_EQUALS, "-=", line_, column_};
                }
                return {TokenType::MINUS, "-", line_, column_};

            default:
                if(c == ' ' || c == '\n' || c == '\t' || c == '\r') break;
                return {TokenType::ERROR, c, line_, column_};
        }
    }
    return {TokenType::END_OF_FILE, "EOF", line_, column_};
}
#pragma clang diagnostic pop
