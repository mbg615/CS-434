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
#include <fstream>

#include "../Token.hpp"
#include "../Lexer/Lexer.hpp"
#include "../Parser/Parser.hpp"
#include "../stackMachine/StackMachine.hpp"

int main() {
    Lexer lexer("test.c");
    Parser parser(lexer);

    std::ofstream outputFile("test.vsm");
    AST::setOutputStream(&outputFile);

    try {
        ASTPtr ast = parser.parseStmt();

//        std::cout << "\nPretty printed expression:\n";
//        ast->emit();
//        std::cout << "\n\n";


        ast->emitStackCode();

    } catch (const std::exception& e) {
        std::cerr << "Compiler Error: " << e.what() << std::endl;
        return 1;
    }

    outputFile.close();

    StackMachine stackMachine;
    stackMachine.loadProgramFromFile("test.vsm");

    stackMachine.printInstructionQueue();
    std::cout << std::endl;
    stackMachine.printLabelMap();
    std::cout << std::endl;

    stackMachine.runProgram();

    return 0;
}
