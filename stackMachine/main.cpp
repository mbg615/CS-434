#include <iostream>

#include "StackMachine.hpp"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program_file>" << std::endl;
        return 1;
    }

    std::cout << "C++ based Stack Machine evaluating " << argv[1] << "\n" << std::endl;

    StackMachine stackMachine;

    if(!stackMachine.loadProgramFromFile(argv[1])) {
        std::cerr << "Error: " << argv[1] << " does not exist" << std::endl;
        return 1;
    }

    stackMachine.printInstructionQueue();
    std::cout << std::endl;
    stackMachine.printLabelMap();
    std::cout << std::endl;

    stackMachine.runProgram();

    return 0;
}
