/*
 * main.cpp
 *
 * Entry point for the compiler.
 *
 * This file ties together all major components of the compiler, including:
 *  - Lexical analysis (lexer)
 *  - Syntax analysis (Parser)
 *  - Abstract Syntax Tree (AST) construction
 *  - Code generation
 *
 * Responsibilities:
 *  - Coordinate the flow from source input to final output
 *  - Initialize and invoke each compiler stage
 *  - Handle errors and report diagnostics
 *
 * Author: Maddox Guthrie
 * Date: April 7th 2025
 */

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "../Token.hpp"
#include "../Lexer/Lexer.hpp"
#include "../Parser/Parser.hpp"
#include "../stackMachine/StackMachine.hpp"

int main() {
    try {
        Lexer lexerRead("math.c");

        std::vector<Token> tokens;
        Token token = lexerRead.lex();
        while(token.getToken() != TokenType::END_OF_FILE) {
            tokens.push_back(token);
            std::cout << token << std::endl;
            token = lexerRead.lex();
        }
        tokens.push_back(token);
        std::cout << token << std::endl;

        Lexer lexer("math.c");


        Parser parser(lexer);
        ASTPtr ast = parser.parseExpression();

        std::cout << "\nPretty printed expression:\n";
        ast->emit();
        std::cout << "\n\n";

        std::cout << "Stack machine code:\n";



        std::streambuf* originalCoutBuffer = std::cout.rdbuf(); // Save the original buffer
        std::ostringstream oss; // Redirect cout to a stringstream
        std::cout.rdbuf(oss.rdbuf());

        ast->emitStackCode();
        std::cout << "print \"The answer is: \"\n";
        std::cout << "print\n";
        std::cout << "push 0\n";
        std::cout << "end\n";

        std::cout.rdbuf(originalCoutBuffer); // Restore the original cout
        std::ofstream outfile("test.vsm"); // Write the captured string to a file
        outfile << oss.str();

        std::cout << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Compiler Error: " << e.what() << std::endl;
        return 1;
    }

    StackMachine stackMachine;

    stackMachine.loadProgramFromFile("test.vsm");

    stackMachine.printInstructionQueue();
    std::cout << std::endl;
    stackMachine.printLabelMap();
    std::cout << std::endl;

    stackMachine.runProgram();



    return 0;
}
