/*
 * Scanner Header File
 * -------------------
 * This file declares the interface for the lexical analyzer (scanner) for the language,
 * providing the `lex()` function to tokenize input.
 *
 * The scanner reads characters from the input and groups them into meaningful
 * tokens for the Parser to process. It handles keywords, identifiers,
 * literals, operators, and other language constructs.
 */

#include <string>
#include <unordered_map>

#include "../Token.hpp"

#ifndef LEXER_HPP
#define LEXER_HPP

class Lexer {
private:
    std::string source_;
    size_t pos_;
    int line_;
    int column_;

    std::unordered_map<std::string, TokenType> keywords {
            {"int", TokenType::INT},
            {"void", TokenType::VOID},
            {"struct", TokenType::STRUCT},
            {"enum", TokenType::ENUM},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"while", TokenType::WHILE},
            {"for", TokenType::FOR},
            {"switch", TokenType::SWITCH},
            {"case", TokenType::CASE},
            {"default", TokenType::DEFAULT},
            {"break", TokenType::BREAK},
            {"continue", TokenType::CONTINUE},
            {"return", TokenType::RETURN},
    };

    void loadSource(const std::string &filename);
    char advance();
    char peek();
    bool match(char expected);

public:
    explicit Lexer(const std::string &source);
    Token lex();

};

#endif // LEXER_HPP
