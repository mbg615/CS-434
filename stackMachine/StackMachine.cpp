#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#define MAX_INSTRUCTION_COUNT 1024

class StackMachine {
private:
    int generalPurposeRegister = 0; // General Purpose Register

    // Instruction model
    std::string instructionQueue[MAX_INSTRUCTION_COUNT];
    int instructionCounter = 0; // Next instruction to execute

    // Stack model
    int memoryStack[4096]{};
    int stackTop = 0; // Next open slot in memory stack
    int stackPointer = -1; // Current frame in memory

public:




















    void run() {
        for(auto & i: instructionQueue) {
            if(i.empty()) continue; // Skip empty lines
            parseLine(i);
        }
    }


    std::vector<std::string> parseLine(std::string &instruction) {
        // Strip any comments from the instruction line.
        if(size_t position = instruction.find(';'); position != std::string::npos) {
            instruction.erase(position);
        }

        std::vector<std::string> tokens;
        std::istringstream stream(instruction);
        std::string token;

        while(stream >> token) {
            if(token == "print") {
                tokens.push_back(token);

                std::string remaining;
                std::getline(stream, remaining);


                if (size_t position = remaining.find(" \t"); position != std::string::npos) {
                    remaining = remaining.substr(position);
                }

                tokens.push_back(remaining);
                break;

            } else {
                tokens.push_back(token);
            }
        }

    }

    bool loadProgramFromFile(const std::string &filename) {
        std::ifstream programFile(filename);
        if(!programFile.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return false;
        }

        int index = 0;
        std::string instruction;

        while(std::getline(programFile, instruction) && index < MAX_INSTRUCTION_COUNT) {
            instructionQueue[index++] = instruction;
        }

        programFile.close();

        if(index >= MAX_INSTRUCTION_COUNT) {
            std::cerr << "Warning: Program truncated to fit instruction memory" << std::endl;
        }

        return true;
    }
};

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program_file>" << std::endl;
        return 1;
    }

    std::cout << "C++ based Stack Machine evaluating " << argv[0] << "\n" << std::endl;

    StackMachine stackMachine;

    if(!stackMachine.loadProgramFromFile(argv[1])){
        std::cerr << "Error: " << argv[1] << " does not exist" << std::endl;
        return 1;
    }

    stackMachine.run();

    return 0;
}