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
#include <string>
#include <fstream>
#include "../Token.hpp"

#include "../Lexer/Lexer.hpp"
#include "../Parser/Parser.hpp"
#include "../stackMachine/StackMachine.hpp"

int main(int argc, char **argv) {
    if(argc == 1) {
        std::cerr << "Compile Error: no input files\n";
        exit(1);
    }

    Lexer lexer(argv[1]);
    Parser parser(lexer);

    std::ofstream outputFile("out.vsm");
    AST::setOutputStream(&outputFile);

    try {

        outputFile << "jump _main:\n";

        std::vector<ASTPtr> program = parser.parseProgram();

        for (const auto& func : program) {
            func->emitStackCode();
        }

    } catch (const std::exception& e) {
        std::cerr << "Compile Error: " << e.what() << std::endl;
        return 1;
    }

    outputFile.close();

    StackMachine stackMachine;
    stackMachine.loadProgramFromFile("out.vsm");
    stackMachine.runProgram();

    return 0;
}
