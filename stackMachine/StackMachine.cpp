#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <sstream>
#include <unordered_map>
#include <variant>

#define MAX_INSTRUCTION_COUNT 1024

class StackMachine {
private:
    std::unordered_map<std::string, std::variant<std::function<void(std::string)>, std::function<void()>>> instructionImplementationMap;
    int generalPurposeRegister = 0; // General Purpose Register

    // Instruction model
    std::string instructionQueue[MAX_INSTRUCTION_COUNT][2];
    std::unordered_map<std::string, int> labelMap;
    int instructionCounter = 0; // (pc) Next instruction to execute

    // Stack model
    int memoryStack[4096]{};
    int stackTop = 0; // (top) Next open slot in memory stack
    int basePointer = 0; // (bp) Base frame of current function

    static int validAddress(const int addr) {
        if (addr >= 4096) {
            std::cerr << "Stack overflow" << std::endl;
            return 0;
        } if (addr <= 0) {
            std::cerr << "Stack underflow" << std::endl;
            return 0;
        }
        return 1;
    }

public:
    StackMachine() {
        // Memory state function initializations
        instructionImplementationMap["push"] = [this](const std::string &arg) {push(arg);};
        instructionImplementationMap["pop"] = [this](const std::string &arg) {pop(arg);};
        instructionImplementationMap["dup"] = [this]() {dup();};
        instructionImplementationMap["load"] = [this](const std::string &arg) {load(arg);};
        instructionImplementationMap["save"] = [this](const std::string &arg) {save(arg);};

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
        if (!validAddress(stackTop)) return;

        if(arg == "bp") {
            if (!validAddress(basePointer)) return;
            memoryStack[stackTop++] = generalPurposeRegister = memoryStack[basePointer];
        } else if(arg == "top") {
            memoryStack[stackTop++] = generalPurposeRegister = memoryStack[stackTop - 1];
        } else {
            memoryStack[stackTop++] = generalPurposeRegister = std::stoi(arg);
        }
        std::cout << "Pushed " << generalPurposeRegister << " onto the stack" << std::endl;
    }

    void pop(const std::string &arg) {
        if (!validAddress(stackTop)) return;

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
        if (!validAddress(stackTop)) return;
        memoryStack[stackTop++] = generalPurposeRegister = memoryStack[stackTop - 1];
        std::cout << "Duplicated " << generalPurposeRegister << " onto the stack" << std::endl;
    }

    void load(const std::string &arg) {
        pop(std::string());
        int addr = generalPurposeRegister;
        if (arg == "bp") {
            addr += basePointer;
        } else if (arg == "top") {
            addr += stackTop - 1;
        }

        if (!validAddress(addr)) return;
        push(std::to_string(memoryStack[addr]));
    }

    void save(const std::string &arg) {
        pop(std::string());
        int addr = generalPurposeRegister;
        if (arg == "bp") {
            addr += basePointer;
        } else if (arg == "top") {
            addr += stackTop - 1;
        }

        if (!validAddress(addr)) return;
        memoryStack[addr] = generalPurposeRegister = memoryStack[stackTop - 1];
    }

    void store(const std::string &arg) {
        pop(std::string());
        int addr = generalPurposeRegister;
        if (arg == "bp") {
            addr += basePointer;
        } else if (arg == "top") {
            addr += stackTop - 1;
        }

        if (!validAddress(addr)) return;
        memoryStack[addr] = generalPurposeRegister = memoryStack[stackTop - 1];
        pop(std::string());
    }

    // Control of execution functions
    void call(const std::string &arg) {
        if (arg.empty()) {
            std::cerr << "Error: push requires an argument" << std::endl;
            return;
        }

    }

    // ret and retv are both handled in this method
    void ret(const std::string &arg) {

    }

    void brt(const std::string &arg) {

    }

    void brz(const std::string &arg) {

    }

    void jump(const std::string &arg) {

    }

    // Arithmetic functions
    void add() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister += temp;
        push(std::to_string(generalPurposeRegister));
    }

    void sub() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister -= temp;
        push(std::to_string(generalPurposeRegister));
    }

    void mul() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister *= temp;
        push(std::to_string(generalPurposeRegister));
    }

    void div() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister /= temp;
        push(std::to_string(generalPurposeRegister));
    }

    void mod() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister %= temp;
        push(std::to_string(generalPurposeRegister));
    }

    // Relational operator functions
    void eq() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = temp == generalPurposeRegister;
        push(std::to_string(generalPurposeRegister));
    }

    void neq() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = temp != generalPurposeRegister;
        push(std::to_string(generalPurposeRegister));
    }

    void lt() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = generalPurposeRegister < temp;
        push(std::to_string(generalPurposeRegister));
    }

    void lte() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = generalPurposeRegister <= temp;
        push(std::to_string(generalPurposeRegister));
    }

    void gt() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = generalPurposeRegister > temp;
        push(std::to_string(generalPurposeRegister));
    }

    void gte() {
        if (!validAddress(stackTop)) return;
        pop(std::string());
        const int temp = generalPurposeRegister;
        pop(std::string());
        generalPurposeRegister = generalPurposeRegister >= temp;
        push(std::to_string(generalPurposeRegister));
    }

    // Special functions
    void print(const std::string &arg) {
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

    bool loadProgramFromFile(const std::string &filename) {
        std::ifstream programFile(filename);
        if(!programFile.is_open()) {
            std::cerr << "Unable to open file " << filename << std::endl;
            return false;
        }

        int index = 0;
        std::string instruction;

        while(std::getline(programFile, instruction)) {
            if(instruction.empty()) continue;

            // Remove ; -->
            if(const size_t pos = instruction.find(';'); pos != std::string::npos) {
                instruction.erase(pos);
            }

            std::istringstream instructionStream(instruction);
            std::string token;
            if(!(instructionStream >> token)) continue;

            // Add labels to the label map as they are found
            if(!instructionImplementationMap.contains(token)) {
                labelMap[token] = index;
            }

            std::string argument;
            if(!(instructionStream >> argument)) argument.clear();

            if(index >= MAX_INSTRUCTION_COUNT) continue;

            instructionQueue[index][0] = token;
            instructionQueue[index][1] = argument;

            index++;
        }

        if (index >= MAX_INSTRUCTION_COUNT) {
            std::cerr << "Warning: Program truncated to fit instruction memory" << std::endl;
        }

        return true;
    }

    void printInstructionQueue() const {
        for (int i = 0; i < MAX_INSTRUCTION_COUNT; ++i) {
            if(instructionQueue[i][0] == "" && instructionQueue[i][1] == "") continue;
            std::cout << "Instruction " << i << ": " << instructionQueue[i][0] << " " << instructionQueue[i][1] << std::endl;
        }
    }

    void printLabelMap() const {
        for (const auto& entry : labelMap) {
            std::cout << "Label: " << entry.first << ", Value: " << entry.second << std::endl;
        }
    }

};

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


    return 0;
}