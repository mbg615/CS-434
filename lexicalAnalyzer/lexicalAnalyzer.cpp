/*
 * Scanner Source File
 * -------------------
 * This file implements the lexical analyzer (scanner) for the language,
 * providing the `lex()` function to tokenize input.
 *
 * The scanner reads characters from the input and groups them into meaningful
 * tokens for the parser to process. It handles keywords, identifiers,
 * literals, operators, and other language constructs.
 */

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <iomanip>

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, SEMICOLON, COLON, FORWARD_SLASH, ASTERISK, PLUS, MINUS, PERCENT, NOT,

    // Multi-character operators
    ASSIGN, EQUALS, NOT_EQUALS, GREATER, GREATER_EQUALS, LESS, LESS_EQUALS,
    INCREMENT, DECREMENT, PLUS_EQUALS, MINUS_EQUALS, MULT_EQUALS, DIV_EQUALS, MOD_EQUALS,
    OR, AND, TERNARY,

    // Comments
    LINE_COMMENT, BLOCK_COMMENT, END_OF_FILE,

    // Keywords
    INT, VOID, STRUCT, ENUM, IF, ELSE, WHILE, FOR, SWITCH, CASE, DEFAULT,
    BREAK, CONTINUE, RETURN,

    // Literals and Identifiers
    STRING_LITERAL, CHAR_LITERAL, INT_LITERAL, IDENTIFIER, ERROR
};

class Token {
private:
    TokenType type_;
    std::string lexeme_;
    int line_;
    int column_;

    [[nodiscard]] std::string toString() const {
        std::string string[] = {
                "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE", "LEFT_BRACKET", "RIGHT_BRACKET",
                "COMMA", "DOT", "SEMICOLON", "COLON", "FORWARD_SLASH", "ASTERISK", "PLUS", "MINUS", "PERCENT", "NOT",
                "ASSIGN", "EQUALS", "NOT_EQUALS", "GREATER", "GREATER_EQUALS", "LESS", "LESS_EQUALS",
                "INCREMENT", "DECREMENT", "PLUS_EQUALS", "MINUS_EQUALS", "MULT_EQUALS", "DIV_EQUALS", "MOD_EQUALS",
                "OR", "AND", "TERNARY",
                "LINE_COMMENT", "BLOCK_COMMENT", "END_OF_FILE",
                "INT", "VOID", "STRUCT", "ENUM", "IF", "ELSE", "WHILE", "FOR", "SWITCH", "CASE", "DEFAULT",
                "BREAK", "CONTINUE", "RETURN",
                "STRING_LITERAL", "CHAR_LITERAL", "INT_LITERAL", "IDENTIFIER", "ERROR"};
        return string[static_cast<int>(type_)];
    }

public:
    Token(TokenType type, std::string lexeme, int line, int column) : type_(type), lexeme_(std::move(lexeme)), line_(line), column_(column) {}
    Token(TokenType type, char lexeme, int line, int column) : type_(type), lexeme_(1, lexeme), line_(line), column_(column) {}

    [[nodiscard]] TokenType getToken() const { return type_; }
    [[nodiscard]] const std::string& getLexeme() const { return lexeme_; }
    [[nodiscard]] int getLine() const { return line_; }
    [[nodiscard]] int getColumn() const { return column_; }

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
            constexpr int TOKEN_WIDTH = 15;

            os << std::left << std::setw(TOKEN_WIDTH) << token.toString() << " "
               << std::right << std::setw(3) << token.getLine() << ":"
               << std::setw(2) << token.getColumn() << " "
               << "\"" << token.getLexeme() << "\"";
            return os;
    }
};

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

    void loadSource(const std::string &filename) {
        std::ifstream source(filename);
        if(!source) throw std::runtime_error("Could not open file: " + filename);

        source_ = std::string((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
    }

    char advance() {
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

    char peek() { return (pos_ < source_.size()) ? source_[pos_] : '\0'; }

    bool match(char expected) {
        if(pos_ < source_.size() && peek() == expected) {
            pos_++;
            column_++;
            return true;
        }
        return false;
    }

public:
    explicit Lexer(const std::string &source) : pos_(0), line_(1), column_(1) {
        loadSource(source);
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-case-range"
    Token lex() {
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
                case '.':
                    return {TokenType::DOT, c, line_, column_};
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
                    // ToDo: Does this work?
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
                    // ToDo: Does this work?
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
                    return {TokenType::INT_LITERAL, lexeme, line_, column_};
                }

                case 'a' ... 'z':
                case 'A' ... 'Z':
                case '_': {
                    std::string lexeme(1, c);
                    while(pos_ < source_.size() && (std::isalnum(source_[pos_]) || source_[pos_] == '_')) {
                        lexeme += advance();
                    }

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

};

int main() {
    Lexer myLexer("test.c");

    Token token = myLexer.lex();
    while(token.getToken() != TokenType::END_OF_FILE) {
        std::cout << token << std::endl;
        token = myLexer.lex();
    }
    return 0;
}
