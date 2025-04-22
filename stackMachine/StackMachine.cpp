#include "StackMachine.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

int StackMachine::validAddress(const int addr) {
    if (addr >= 4096) {
        std::cerr << "Stack overflow" << std::endl;
        return 0;
    } if (addr < 0) {
        std::cerr << "Stack underflow" << std::endl;
        return 0;
    }
    return 1;
}

StackMachine::StackMachine() {
    // Memory state function initializations
    instructionImplementationMap["push"] = [this](const std::string &arg) {push(arg);};
    instructionImplementationMap["pop"] = [this](const std::string &arg) {pop(arg);};
    instructionImplementationMap["pop"] = [this]() {pop();};
    instructionImplementationMap["dup"] = [this]() {dup();};
    instructionImplementationMap["load"] = [this](const std::string &arg) {load(arg);};
    instructionImplementationMap["save"] = [this](const std::string &arg) {save(arg);};
    instructionImplementationMap["store"] = [this](const std::string &arg) {store(arg);};

    // Control of execution function initializations
    instructionImplementationMap["call"] = [this](const std::string &arg) {call(arg);};
    instructionImplementationMap["ret"] = [this]() {ret();};
    instructionImplementationMap["retv"] = [this]() {retv();};
    instructionImplementationMap["brt"] = [this](const std::string &arg) {brt(arg);};
    instructionImplementationMap["brz"] = [this](const std::string &arg) {brz(arg);};
    instructionImplementationMap["jump"] = [this](const std::string &arg) {jump(arg);};

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
void StackMachine::push(const std::string &arg) {
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
    if(DEBUG) std::cout << "Pushed " << generalPurposeRegister << " onto the stack" << std::endl;
}

void StackMachine::pop(const std::string &arg) {
    if (!validAddress(stackTop)) return;

    if(arg == "top") {
        stackTop = memoryStack[--stackTop];
        if(DEBUG) std::cout << "Popped " << stackTop << " from the stack" << std::endl;
    } else if(arg == "bp") {
        basePointer = memoryStack[--stackTop];
        if(DEBUG) std::cout << "Popped " << basePointer << " from the stack" << std::endl;
    } else {
        generalPurposeRegister = memoryStack[--stackTop];
        if(DEBUG) std::cout << "Popped " << generalPurposeRegister << " from the stack" << std::endl;
    }

    memoryStack[stackTop] = 0;
}

void StackMachine::pop() {
    if (!validAddress(stackTop)) return;
    generalPurposeRegister = memoryStack[--stackTop];
    if(DEBUG) std::cout << "Popped " << generalPurposeRegister << " from the stack" << std::endl;
    memoryStack[stackTop] = 0;
}

void StackMachine::dup() {
    if (!validAddress(stackTop)) return;
    memoryStack[stackTop] = generalPurposeRegister = memoryStack[stackTop - 1];
    stackTop++;
    std::cout << "Duplicated " << generalPurposeRegister << " onto the stack" << std::endl;
}

void StackMachine::load(const std::string &arg) {
    pop();
    int addr = generalPurposeRegister;
    if (arg == "bp") {
        addr += basePointer;
    } else if (arg == "top") {
        addr += stackTop - 1;
    }

    if (!validAddress(addr)) return;
    push(std::to_string(memoryStack[addr]));
}

void StackMachine::save(const std::string &arg) {
    pop();
    int addr = generalPurposeRegister;
    if (arg == "bp") {
        addr += basePointer;
    } else if (arg == "top") {
        addr += stackTop - 1;
    }

    if (!validAddress(addr)) return;
    memoryStack[addr] = generalPurposeRegister = memoryStack[stackTop - 1];
}

void StackMachine::store(const std::string &arg) {
    pop();
    int addr = generalPurposeRegister;
    if (arg == "bp") {
        addr += basePointer;
    } else if (arg == "top") {
        addr += stackTop - 1;
    }

    if (!validAddress(addr)) return;
    memoryStack[addr] = generalPurposeRegister = memoryStack[stackTop - 1];
    //pop();
}

// Control flow functions
void StackMachine::call(const std::string &arg) {
    if (arg.empty()) {
        std::cerr << "Error: call requires an argument" << std::endl;
        return;
    }

    // Print current stack before modifying it
    std::cerr << "CALL: Stack before function call:\n";

    int argNum = generalPurposeRegister;
    std::cerr << "CALL: Number of arguments = " << argNum << std::endl;

    if (argNum < 0 || argNum > 4096) {
        std::cerr << "Error: Invalid number of arguments: " << argNum << std::endl;
        return;
    }

    // Store function arguments
    std::vector<int> functionArgs(argNum);
    for (int i = 0; i < argNum; i++) {
        pop();
        functionArgs[i] = generalPurposeRegister;
        std::cerr << "CALL: Popped argument " << i << " = " << functionArgs[i] << std::endl;
    }

    // Push the current base pointer (saved frame pointer)
    std::cerr << "CALL: Pushing base pointer = " << basePointer << std::endl;
    push(std::to_string(basePointer));

    // Push the return address
    std::cerr << "CALL: Pushing return address = " << instructionCounter + 1 << std::endl;
    push(std::to_string(instructionCounter + 1));

    // Update base pointer to the new stack frame
    basePointer = stackTop - 1; // Point to saved BP
    std::cerr << "CALL: Updated base pointer to " << basePointer << std::endl;

    // Push function arguments back in order
    for (int i = argNum - 1; i >= 0; i--) {
        push(std::to_string(functionArgs[i]));
        std::cerr << "CALL: Pushed argument " << i << " = " << functionArgs[i] << std::endl;
    }

    // Print stack after pushing everything
    std::cerr << "CALL: Stack after setting up function call:\n";

    // Jump to function
    std::cerr << "CALL: Jumping to function " << arg << std::endl;
    jump(arg);
}

void StackMachine::ret() {
    if(basePointer == 0) {
        exit(generalPurposeRegister);
    }
    instructionCounter = memoryStack[basePointer];
    basePointer = memoryStack[basePointer - 1];
}

void StackMachine::retv() {
    pop();
    if(basePointer == 0) {
        push(std::to_string(generalPurposeRegister));
        exit(generalPurposeRegister);
    }
    instructionCounter = memoryStack[basePointer];
    basePointer = memoryStack[basePointer - 1];
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::brt(const std::string &arg) {
    if (arg.empty()) {
        std::cerr << "Error: brt requires an argument" << std::endl;
        return;
    }

    pop();
    if(generalPurposeRegister == 1) {
        jump(arg);
    }
}

void StackMachine::brz(const std::string &arg) {
    if (arg.empty()) {
        std::cerr << "Error: brz requires an argument" << std::endl;
        return;
    }

    pop();
    if(generalPurposeRegister == 0) {
        jump(arg);
    }
}

void StackMachine::jump(const std::string &arg) {
    if (arg.empty()) {
        std::cerr << "Error: jump requires an argument" << std::endl;
        return;
    }

    auto it = labelMap.find(arg);
    if(it != labelMap.end()) {
        instructionCounter = it->second;
    } else {
        std::cerr << "Error: label '" << arg << "' not found" << std::endl;
    }
}

// Arithmetic functions
void StackMachine::add() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister += temp;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::sub() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister -= temp;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::mul() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister *= temp;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::div() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister /= temp;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::mod() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister %= temp;
    push(std::to_string(generalPurposeRegister));
}

// Relational operator functions
void StackMachine::eq() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = temp == generalPurposeRegister;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::neq() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = temp != generalPurposeRegister;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::lt() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = generalPurposeRegister < temp;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::lte() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = generalPurposeRegister <= temp;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::gt() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = generalPurposeRegister > temp;
    push(std::to_string(generalPurposeRegister));
}

void StackMachine::gte() {
    if (!validAddress(stackTop)) return;
    pop();
    const int temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = generalPurposeRegister >= temp;
    push(std::to_string(generalPurposeRegister));
}

// Special functions
void StackMachine::print(const std::string &arg) {
    if (arg.empty()) {
        if (stackTop <= 0) {
            std::cerr << "Error: Stack is empty. Nothing to print." << std::endl;
            return;
        }
        std::cout << memoryStack[stackTop - 1] << std::endl;
    } else {
        std::string formattedArg;
        for (size_t i = 0; i < arg.length(); ++i) {
            if (arg[i] == '\\' && i + 1 < arg.length()) {
                if (arg[i + 1] == 'n') {
                    formattedArg += '\n';
                    ++i; // Skip next character
                } else if (arg[i + 1] == 't') {
                    formattedArg += '\t';
                    ++i; // Skip next character
                } else {
                    formattedArg += arg[i];
                }
            } else {
                formattedArg += arg[i];
            }
        }
        std::cout << formattedArg << std::endl;
    }
}

void StackMachine::read() {
    std::cin >> memoryStack[stackTop];
    generalPurposeRegister = memoryStack[stackTop++];
}

void StackMachine::end(const std::string &arg) {
    if(arg.empty()) {
        push(std::to_string(generalPurposeRegister));
        exit(generalPurposeRegister);
    } else if(arg == "bp") {
        push(std::to_string(basePointer));
        exit(basePointer);
    } else if(arg == "top") {
        push(std::to_string(stackTop));
        exit(stackTop);
    } else {
        push(arg);
        exit(memoryStack[stackTop - 1]);
    }
}

// Program execution functions
bool StackMachine::runProgram() {
    while(true) {
        std::string key = instructionQueue[instructionCounter][0];
        std::string value = instructionQueue[instructionCounter][1];

        // Try and call the correct variant
        if(instructionImplementationMap.contains(key)) {
            auto& funcVariant = instructionImplementationMap[key];

            if(std::holds_alternative<std::function<void(std::string)>>(funcVariant)) {
                std::get<std::function<void(std::string)>>(funcVariant)(value);
            } else if(std::holds_alternative<std::function<void()>>(funcVariant)) {
                std::get<std::function<void()>>(funcVariant)();
            }
        } else {
            std::cerr << "Error: Instruction " << key << " not found!" <<std::endl;
        }

        if(key == "end") {
            exit(stackTop - 1);
        }

        instructionCounter++;
    }
}

bool StackMachine::loadProgramFromFile(const std::string &filename) {
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
        if (instructionStream >> std::ws && instructionStream.peek() == '"') {  // Handle quoted string argument
            instructionStream.get(); // Remove the opening quote
            std::getline(instructionStream, argument, '"'); // Read until closing quote
        } else if(!(instructionStream >> argument)) {
            argument.clear();
        }

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

void StackMachine::printInstructionQueue() const {
    for (int i = 0; i < MAX_INSTRUCTION_COUNT; ++i) {
        if(instructionQueue[i][0].empty() && instructionQueue[i][1].empty()) continue;
        std::cout << "Instruction " << i << ": " << instructionQueue[i][0] << " " << instructionQueue[i][1] << std::endl;
    }
}

void StackMachine::printLabelMap() const {
    for (const auto& entry : labelMap) {
        std::cout << "Location: " << entry.second << ", Label: " << entry.first << std::endl;
    }
}
