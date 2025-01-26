#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <bits/fs_path.h>

#define MAX_INSTRUCTION_COUNT 1024

class StackMachine {
private:
    //std::unordered_map<std::string, std::function<void(std::string)>> instructionImplementationMap;
    std::unordered_map<std::string, std::variant<std::function<void(std::string)>, std::function<void()>>> instructionImplementationMap;
    int generalPurposeRegister = 0; // General Purpose Register

    // Instruction model
    std::string instructionQueue[MAX_INSTRUCTION_COUNT][2];
    int instructionCounter = 0; // (pc) Next instruction to execute

    // Stack model
    int memoryStack[4096]{};
    int stackTop = 0; // (top) Next open slot in memory stack
    int basePointer = 0; // (bp) Base frame of current function

public:
    StackMachine() {
        // Memory state function initializations
        instructionImplementationMap["push"] = [this](const std::string &arg) {push(arg);};
        instructionImplementationMap["pop"] = [this](const std::string &arg) {pop(arg);};
        instructionImplementationMap["dup"] = [this]() {dup();};




        // Control of execution function initializations







        // Arithmetic function initializations
        instructionImplementationMap["add"] = [this]() {add();};
        instructionImplementationMap["sub"] = [this]() {sub();};
        instructionImplementationMap["mul"] = [this]() {mul();};
        instructionImplementationMap["div"] = [this]() {div();};
        instructionImplementationMap["mod"] = [this]() {mod();};

        // Relational function initializations
        instructionImplementationMap["eq"] = [this]() {eq();};
        instructionImplementationMap["neq"] = [this]() {neq();};
        instructionImplementationMap["lt"] = [this]() {lt();};
        instructionImplementationMap["lte"] = [this]() {lte();};
        instructionImplementationMap["gt"] = [this]() {gt();};
        instructionImplementationMap["gte"] = [this]() {gte();};

        // // Special function initializations
        instructionImplementationMap["print"] = [this](const std::string &arg) {print(arg);};
        instructionImplementationMap["read"] = [this]() {read();};
        instructionImplementationMap["end"] = [this](const std::string &arg) {end(arg);};



        
    }

    // Memory state functions
    void push(const std::string &arg) {
        if (arg.empty()) {
            std::cerr << "Error: push requires an argument" << std::endl;
            return;
        }
        if (stackTop >= MAX_INSTRUCTION_COUNT) {
            std::cerr << "Error: stack overflow" << std::endl;
            return;
        }

        if(arg == "bp") {
            if(basePointer <= 0 || basePointer >= MAX_INSTRUCTION_COUNT) {
                return;
            }
            memoryStack[stackTop++] = generalPurposeRegister = memoryStack[basePointer];
        } else if(arg == "top") {
            memoryStack[stackTop++] = generalPurposeRegister = memoryStack[stackTop - 1];
        } else {
            memoryStack[stackTop++] = generalPurposeRegister = std::stoi(arg);
        }
        std::cout << "Pushed " << generalPurposeRegister << " onto the stack" << std::endl;
    }

    void pop(const std::string &arg) {
        if (stackTop <= 0) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        if(arg == "top") {
            stackTop = memoryStack[--stackTop];
            std::cout << "Popped " << stackTop << " from the stack" << std::endl;
        } else if(arg == "bp") {
            basePointer = memoryStack[--stackTop];
            std::cout << "Popped " << basePointer << " from the stack" << std::endl;

        } else {
            generalPurposeRegister = memoryStack[--stackTop];
            std::cout << "Popped " << generalPurposeRegister << " from the stack" << std::endl;
        }
    }

    void dup() {
        if (stackTop <= 0) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        if (stackTop >= MAX_INSTRUCTION_COUNT) {
            std::cerr << "Error: stack overflow" << std::endl;
            return;
        }
        memoryStack[stackTop++] = generalPurposeRegister = memoryStack[stackTop - 1];
        std::cout << "Duplicated " << generalPurposeRegister << " onto the stack" << std::endl;
    }

    void load(const std::string &arg) {

    }

    void save(const std::string &arg) {

    }

    void store(const std::string &arg) {

    }

    // Control of execution functions
    void call(const std::string &arg) {

    }

    void ret(const std::string &arg) {

    }

    void retv(const std::string &arg) {

    }

    void brt(const std::string &arg) {

    }

    void brz(const std::string &arg) {

    }

    void jump(const std::string &arg) {

    }

    // Arithmetic functions
    void add() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister += temp;
        push(std::to_string(generalPurposeRegister));
    }

    void sub() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister -= temp;
        push(std::to_string(generalPurposeRegister));
    }

    void mul() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister *= temp;
        push(std::to_string(generalPurposeRegister));
    }

    void div() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister /= temp;
        push(std::to_string(generalPurposeRegister));
    }

    void mod() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister %= temp;
        push(std::to_string(generalPurposeRegister));
    }

    // Relational operator functions
    void eq() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = temp == generalPurposeRegister;
        push(std::to_string(generalPurposeRegister));
    }

    void neq() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = temp != generalPurposeRegister;
        push(std::to_string(generalPurposeRegister));
    }

    void lt() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = generalPurposeRegister < temp;
        push(std::to_string(generalPurposeRegister));
    }

    void lte() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = generalPurposeRegister <= temp;
        push(std::to_string(generalPurposeRegister));
    }

    void gt() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = generalPurposeRegister > temp;
        push(std::to_string(generalPurposeRegister));
    }

    void gte() {
        if (stackTop <= 2) {
            std::cerr << "Error: stack underflow" << std::endl;
            return;
        }
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = generalPurposeRegister >= temp;
        push(std::to_string(generalPurposeRegister));
    }

    // Special functions
    void print(const std::string &arg) const {
        // ToDo: Does const cause a compile error?
        // ToDo: Does print pop
        if(arg.empty()) {
            if(stackTop <= 0) {
                std::cerr << "Error: Stack is empty. Nothing to print." << std::endl;
                return;
            }
            std::cout << stackTop << std::endl;
        } else {
            std::cout << arg << std::endl;
        }
    }

    void read() {
        std::cin >> memoryStack[stackTop];
        generalPurposeRegister = memoryStack[stackTop];
        stackTop++;
    }

    void end(const std::string &arg) {
        if(arg.empty()) {
            generalPurposeRegister = std::stoi(arg);
        } else if(arg == "bp") {
            basePointer = std::stoi(arg);
        } else if(arg == "top") {
            stackTop = std::stoi(arg);
        } else {
            memoryStack[stackTop++] = std::stoi(arg);
        }
        exit(memoryStack[stackTop - 1]);
    }

    void label(const std::string &arg) {

    }

    // Instruction stack functions
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