#include <iostream>

#include "Lexer.hpp"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
        return 1;
    }

    Lexer lexer(argv[1]);
    auto token = lexer.lex();
    while (token.getToken() != TokenType::END_OF_FILE) {
        std::cout << token << "\n";
        token = lexer.lex();
    }
    std::cout << token << "\n";
    return 0;
}
