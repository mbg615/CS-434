#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

#define MAX_INSTRUCTION_COUNT 1024

class StackMachine {
private:
    std::unordered_map<std::string, std::function<void(std::string)>> instructionImplementationMap;
    int generalPurposeRegister = 0; // General Purpose Register

    // Instruction model
    std::string instructionQueue[MAX_INSTRUCTION_COUNT][2];
    int instructionCounter = 0; // (pc) Next instruction to execute

    // Stack model
    int memoryStack[4096]{};
    int stackTop = 0; // (top) Next open slot in memory stack
    int stackPointer = -1; // (sp) Current frame in memory

public:
    StackMachine() {
        // instructionImplementationMap["push"] = std::bind(&StackMachine::push, this, std::placeholders::_1);
        instructionImplementationMap["push"] = [this](const std::string &arg) {push(arg);};
        instructionImplementationMap["pop"] = [this](const std::string &arg) {pop(arg);};
        instructionImplementationMap["dup"] = [this](const std::string &arg) {dup(arg);};
        instructionImplementationMap["load"] = [this](const std::string &arg) {load(arg);};


        
    }
    
    void push(const std::string &arg) {
        if (arg.empty()) {
            std::cerr << "Error: push requires an argument" << std::endl;
            return;
        }
        if (stackTop >= MAX_INSTRUCTION_COUNT) {
            std::cerr << "Error: stack overflow" << std::endl;
            return;
        }
        memoryStack[stackTop++] = generalPurposeRegister = std::stoi(arg);
        stackPointer++;
        std::cout << "Pushed " << arg << " onto the stack" << std::endl;
    }

    void pop(const std::string &arg) {
        if (stackTop <= 0) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        if (!arg.empty()) {
            // ToDO: Implement register pop?
        }
        generalPurposeRegister = memoryStack[--stackTop];
        stackPointer--;
        std::cout << "Popped " << generalPurposeRegister << " from the stack" << std::endl;
    }

    void dup(const std::string &arg) {
        if (stackTop <= 0) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        if (stackTop >= MAX_INSTRUCTION_COUNT) {
            std::cerr << "Error: stack overflow" << std::endl;
            return;
        }
        memoryStack[stackTop++] = generalPurposeRegister = memoryStack[stackTop - 1];
        stackPointer++;
        std::cout << "Duplicated " << generalPurposeRegister << " onto the stack" << std::endl;
    }

    void load(const std::string &arg) {
        if (stackTop <= 0) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        if (stackTop >= MAX_INSTRUCTION_COUNT) {
            std::cerr << "Error: stack overflow" << std::endl;
            return;
        }
        if (arg.empty()) {
            // Assuming (0) global frame
            memoryStack[stackTop++] = generalPurposeRegister = memoryStack[0];
        }
        if (arg == "sp" || arg == "SP" || arg == "top" || arg == "TOP") {
            if (stackPointer < 0 || stackTop < 0) {
                std::cerr << "Error: stack underflow" << std::endl;
                return;
            }
            if (stackPointer >= MAX_INSTRUCTION_COUNT || stackTop >= MAX_INSTRUCTION_COUNT) {
                std::cerr << "Error: stack overflow" << std::endl;
                return;
            }
            memoryStack[stackTop++] = generalPurposeRegister = memoryStack[stackPointer];
            stackPointer++;
        }
    }

    void save(const std::string &arg) {
        if (stackTop <= 0) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        if (stackTop >= MAX_INSTRUCTION_COUNT) {
            std::cerr << "Error: stack overflow" << std::endl;
            return;
        }
        if (arg.empty()) {
            // Assuming (0) global frame
            generalPurposeRegister = memoryStack[stackTop - 1];
        }
        if (arg == "sp" || arg == "SP" || arg == "top" || arg == "TOP") {
            if (stackPointer < 0 || stackTop < 0) {
                std::cerr << "Error: stack underflow" << std::endl;
                return;
            }
            if (stackPointer >= MAX_INSTRUCTION_COUNT || stackTop >= MAX_INSTRUCTION_COUNT) {
                std::cerr << "Error: stack overflow" << std::endl;
                return;
            }
            memoryStack[stackTop++] = generalPurposeRegister = memoryStack[stackPointer];
            stackPointer++;
        }
    }














    static std::vector<std::string> parseLine(std::string &instruction) {
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

        return tokens;
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
            parseLine(instruction);
            index++;
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


    return 0;
}