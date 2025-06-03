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

#include <iostream>
#include <fstream>
#include <algorithm>

#include "Lexer.hpp"

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
        int startLine = line_;
        int startColumn = column_;
        char c = advance();

        switch(c) {
            case '(':
                return {TokenType::LEFT_PAREN, c, startLine, startColumn};
            case ')':
                return {TokenType::RIGHT_PAREN, c, startLine, startColumn};
            case '{':
                return {TokenType::LEFT_BRACE, c, startLine, startColumn};
            case '}':
                return {TokenType::RIGHT_BRACE, c, startLine, startColumn};
            case '[':
                return {TokenType::LEFT_BRACKET, c, startLine, startColumn};
            case ']':
                return {TokenType::RIGHT_BRACKET, c, startLine, startColumn};
            case '?':
                return {TokenType::TERNARY, c, startLine, startColumn};
            case ',':
                return {TokenType::COMMA, c, startLine, startColumn};
            case ';':
                return {TokenType::SEMICOLON, c, startLine, startColumn};
            case ':':
                return {TokenType::COLON, c, startLine, startColumn};

            case '<':
                return match('=') ? Token{TokenType::LESS_EQUALS, "<=", startLine, startColumn} : Token{TokenType::LESS, '<', startLine, startColumn};
            case '>':
                return match('=') ? Token{TokenType::GREATER_EQUALS, ">=", startLine, startColumn} : Token{TokenType::GREATER, '>', startLine, startColumn};
            case '!':
                return match('=') ? Token{TokenType::NOT_EQUALS, "!=", startLine, startColumn} : Token{TokenType::NOT, '!', startLine, startColumn};
            case '*':
                return match('=') ? Token{TokenType::MULT_EQUALS, "*=", startLine, startColumn} : Token{TokenType::ASTERISK, '*', startLine,startColumn};
            case '=':
                return match('=') ? Token{TokenType::EQUALS, "==", startLine, startColumn} : Token{TokenType::ASSIGN, '=',startLine, startColumn};
            case '%':
                return match('=') ? Token{TokenType::MOD_EQUALS, "%=", startLine, startColumn} : Token{TokenType::PERCENT,'%', startLine, startColumn};
            case '&':
                return match('&') ? Token{TokenType::AND, "&&", startLine, startColumn} : Token{TokenType::ERROR,'&', startLine, startColumn};
            case '|':
                return match('|') ? Token{TokenType::OR, "||", startLine, startColumn} : Token{TokenType::ERROR,'|', startLine, startColumn};

            case '\'': {
                std::string lexeme = "'";

                if(peek() == '\'') {
                    lexeme += advance();
                } else {
                    if(peek() == '\\') {
                        lexeme += advance();
                        std::string valid = "abfnrtv?0'\"\\";
                        if(valid.find(peek()) != std::string::npos) {
                            lexeme += advance();
                        } else {
                            // ToDo: Generate a warning token "unknown escape sequence"
                            return {TokenType::ERROR, "Unknown escape sequence: \\" + std::string(1, peek()), startLine, startColumn};
                        }
                    } else lexeme += advance();

                    if(peek() == '\'') lexeme += advance();
                    else return {TokenType::ERROR, "Multi-character character constant", startLine, startColumn};
                }


                return {TokenType::CHAR_LITERAL, lexeme, startLine, startColumn};
            }

            case '"': {
                std::string lexeme = "\"";
                while (pos_ < source_.size()) {
                    if (source_[pos_] == '\\') {
                        lexeme += advance();
                        // ToDo: Only support certain escape sequences.
                        lexeme += advance();
                    }
                    else if (source_[pos_] == '\"') {
                        lexeme += advance();
                        break;
                    }
                    else {
                        lexeme += advance();
                    }

                    if(line_ != startLine) {
                        return {TokenType::ERROR, "Unterminated string literal", startLine, startColumn};
                    }
                }
                if(pos_ == source_.size()) {
                    return {TokenType::ERROR, "Unterminated string literal", startLine, startColumn};
                }
                return {TokenType::STRING_LITERAL, lexeme, startLine, startColumn};
            }

            case '/': {
                if (match('/')) {
                    std::string lexeme = "//";
                    while (pos_ < source_.size() && peek() != '\n') {
                        lexeme += peek();
                        advance();
                    }
                    return {TokenType::LINE_COMMENT, lexeme, startLine, startColumn};
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
                    if(!lexeme.ends_with("*/")) {
                        return {TokenType::ERROR, "Unterminated block comment", startLine, startColumn};
                    }
                    return {TokenType::BLOCK_COMMENT, lexeme, startLine, startColumn};
                }
                if (match('=')) {
                    return {TokenType::DIV_EQUALS, "/=", startLine, startColumn};
                }
                return {TokenType::FORWARD_SLASH, "/", startLine, startColumn};
            }

            case '0' ... '9': {
                std::string lexeme(1,c);
                while (pos_ < source_.size() && isdigit(peek())) lexeme += advance();
                if(match('.')) {
                    lexeme += '.';
                    while (pos_ < source_.size() && isdigit(peek())) lexeme += advance();
                    return {TokenType::FLOAT_LITERAL, lexeme, startLine, startColumn};
                }
                return {TokenType::INT_LITERAL, lexeme, startLine, startColumn};
            }

            case '.': {
                if(isdigit(peek())) {
                    std::string lexeme(1,c);
                    while (pos_ < source_.size() && isdigit(peek())) lexeme += advance();
                    return {TokenType::FLOAT_LITERAL, lexeme, startLine, startColumn};
                }
                return {TokenType::DOT, c, startLine, startColumn};
            }

            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_': {
                std::string lexeme(1, c);
                while(pos_ < source_.size() && (std::isalnum(source_[pos_]) || source_[pos_] == '_')) lexeme += advance();
                TokenType type = keywords.contains(lexeme) ? keywords.at(lexeme) : TokenType::IDENTIFIER;
                return {type, lexeme, startLine, startColumn};
            }

            case '+':
                if(match('+')) {
                    return {TokenType::INCREMENT, "++", startLine, startColumn};
                }
                if(match('=')) {
                    return {TokenType::PLUS_EQUALS, "+=", startLine, startColumn};
                }
                return {TokenType::PLUS, "+", startLine, startColumn};

            case '-':
                if(match('-')) {
                    return {TokenType::DECREMENT, "--", startLine, startColumn};
                }
                if(match('=')) {
                    return {TokenType::MINUS_EQUALS, "-=", startLine, startColumn};
                }
                return {TokenType::MINUS, "-", startLine, startColumn};

            default:
                if(c == ' ' || c == '\n' || c == '\t' || c == '\r') break;
                return {TokenType::ERROR, "Unrecognized character: " + std::string(1, c), startLine, startColumn};
        }
    }
    return {TokenType::END_OF_FILE, "EOF", line_, column_};
}
#pragma clang diagnostic pop
