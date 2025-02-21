#include <string>
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
