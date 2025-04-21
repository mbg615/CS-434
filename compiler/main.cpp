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
    Lexer lexer("test.c");
    Parser parser(lexer);

//    auto program = parser.parseProgram();
//    for (const auto& stmt : program) {
//        std::cout << "Internal:\n";
//        stmt->emit();
//    }
//
//    std::streambuf* originalCoutBuffer = std::cout.rdbuf(); // Save the original buffer
//    std::ostringstream oss; // Redirect cout to a stringstream
//    std::cout.rdbuf(oss.rdbuf());
//
//    for (const auto& stmt : program) {
//        std::cout << std::endl;
//        stmt->emitStackCode();
//        std::cout << "print \"The answer is: \"\n";
//        std::cout << "print\n";
//        std::cout << std::endl;
//    }
//
//    std::cout << "push 0\n";
//    std::cout << "end\n";
//
//    std::cout.rdbuf(originalCoutBuffer); // Restore the original cout
//    std::ofstream outfile("test.vsm"); // Write the captured string to a file
//    outfile << oss.str();
//
//    StackMachine stackMachine;
//    stackMachine.loadProgramFromFile("test.vsm");
//    std::cout << "Instructions:\n";
//    stackMachine.printInstructionQueue();
//    std::cout << std::endl;
//    std::cout << "Labels:\n";
//    stackMachine.printLabelMap();
//    std::cout << std::endl;
//
//    stackMachine.runProgram();



    try {
        ASTPtr ast = parser.parseStmt();

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
