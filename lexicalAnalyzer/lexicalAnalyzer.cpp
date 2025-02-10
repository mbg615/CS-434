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
#include <utility>

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, SEMICOLON, COLON, HASH, SINGLE_QUOTE, DOUBLE_QUOTE,
    BACKSLASH, FORWARD_SLASH, ASTERISK, PLUS, MINUS, PERCENT, NOT, AMPERSAND,

    // Multi-character operators
    ASSIGN, EQUALS, NOT_EQUALS, GREATER, GREATER_EQUALS, LESS, LESS_EQUALS,
    INCREMENT, DECREMENT, PLUS_EQUALS, MINUS_EQUALS, MULT_EQUALS, DIV_EQUALS,
    OR, AND, TERNARY,

    // Comments
    LINE_COMMENT, BLOCK_COMMENT_START, BLOCK_COMMENT_END,

    // Whitespace
    SPACE, NEWLINE, TAB, CARRIAGE_RETURN, FORM_FEED,

    // Keywords
    INT, VOID, STRUCT, ENUM, IF, ELSE, WHILE, FOR, SWITCH, CASE, DEFAULT,
    BREAK, CONTINUE, RETURN, END_OF_FILE,

    // Literals and Identifiers
    LITERAL, IDENTIFIER
};

class Token {
private:
    TokenType type_;
    std::string lexeme_;
    int line_;
    int column_;

public:
    Token(TokenType type, std::string lexeme, int line, int column) : type_(type), lexeme_(std::move(lexeme)), line_(line), column_(column) {}

    [[nodiscard]] TokenType getToken() const { return type_; }
    [[nodiscard]] const std::string& getLexeme() const { return lexeme_; }
    [[nodiscard]] int getLine() const { return line_; }
    [[nodiscard]] int getColumn() const { return column_; }

};

class Lexer {
private:
    std::string source_;
    size_t pos_;
    int line_;
    int column_;

    std::unordered_map<std::string, TokenType> keywords_ = {
            // Keywords
            {"enum", TokenType::ENUM},
            {"struct", TokenType::STRUCT},
            {"int", TokenType::INT},
            {"void", TokenType::VOID},
            {"if", TokenType::IF},
            {"else", TokenType::ELSE},
            {"switch", TokenType::SWITCH},
            {"case", TokenType::CASE},
            {"while", TokenType::WHILE},
            {"for", TokenType::FOR},
            {"break", TokenType::BREAK},
            {"continue", TokenType::CONTINUE},
            {"default", TokenType::DEFAULT},
            {"return", TokenType::RETURN},
            {"EOF", TokenType::END_OF_FILE},

            // Single Character symbols
            {"(", TokenType::LEFT_PAREN},
            {")", TokenType::RIGHT_PAREN},
            {"[", TokenType::LEFT_BRACKET},
            {"]", TokenType::RIGHT_BRACKET},
            {"{", TokenType::LEFT_BRACE},
            {"}", TokenType::RIGHT_BRACE},
            {"<", TokenType::LESS},
            {">", TokenType::GREATER},
            {"+", TokenType::PLUS},
            {"-", TokenType::MINUS},
            {"*", TokenType::ASTERISK},
            {"/", TokenType::FORWARD_SLASH},
            {"%", TokenType::PERCENT},
            {"&", TokenType::AMPERSAND},
            {",", TokenType::COMMA},
            {".", TokenType::DOT},
            {";", TokenType::SEMICOLON},
            {":", TokenType::COLON},
            {"\n", TokenType::NEWLINE},
            {"\t", TokenType::TAB},
            {"\r", TokenType::CARRIAGE_RETURN},
            {"\f", TokenType::FORM_FEED},
            {"\\", TokenType::BACKSLASH},
            {"\"", TokenType::DOUBLE_QUOTE},
            {"'", TokenType::SINGLE_QUOTE},
            {" ", TokenType::SPACE},
            {"#", TokenType::HASH},
            {"?", TokenType::TERNARY},
            {"!", TokenType::NOT},
            {"=", TokenType::ASSIGN},
            {"==", TokenType::EQUALS},
            {"!=", TokenType::NOT_EQUALS},
            {"//", TokenType::LINE_COMMENT},
            {"/*", TokenType::BLOCK_COMMENT_START},
            {"*/", TokenType::BLOCK_COMMENT_END},

            // Other Symbols
            {"||", TokenType::OR},
            {"&&", TokenType::AND},
            {"++", TokenType::INCREMENT},
            {"--", TokenType::DECREMENT},
            {"+=", TokenType::PLUS_EQUALS},
            {"-=", TokenType::MINUS_EQUALS},
            {"*=", TokenType::MULT_EQUALS},
            {"/=", TokenType::DIV_EQUALS},
            {">=", TokenType::GREATER_EQUALS},
            {"<=", TokenType::LESS_EQUALS},
    };

public:
    explicit Lexer(std::string source) : source_(std::move(source)), pos_(0), line_(1), column_(1) {}
    
};
