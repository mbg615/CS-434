#include "StackMachine.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

int StackMachine::validAddress(const int addr) {
    /* ToDo:
     * Inline this in the functions that need it. Due to memoryStack being a vector it is possible
     * that memoryStack.size() == memoryStack.capacity() before the .push_back() which would grow
     * the vector. So this is no longer a safe or functional function.
     */
    // if (addr >= memoryStack.size()) {
    if (addr > memoryStack.size()) {
        std::cout << "Addr:" << addr << std::endl;
        std::cout << "Size:" << memoryStack.size() << std::endl;
        return 1;
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
    instructionImplementationMap["neg"] = [this]() {neg();};
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
        memoryStack.push_back(memoryStack[basePointer]);
    } else if(arg == "top") {
        memoryStack.push_back(memoryStack[stackTop - 1]);
    } else {
        // ToDo: Fix this mess
        try {
            if(arg.find('.') != std::string::npos) {
                memoryStack.emplace_back(std::stof(arg));
            } else{
                memoryStack.emplace_back(std::stoi(arg));
            }
        } catch(const std::invalid_argument &e) {
            std::cerr << "Invalid push argument: " << arg << std::endl;
            return;
        }
    }

    if(DEBUG) {
        std::visit([](auto v) {
            std::cout << "Pushed " << v << " onto the stack" << std::endl;
        }, generalPurposeRegister);
    }

    generalPurposeRegister = memoryStack.back();
    stackTop++;
}

void StackMachine::pop(const std::string &arg) {
    if (!validAddress(stackTop)) return;

    if(arg == "top") {
        stackTop = std::visit([](auto v) -> int {
            if constexpr (std::is_same_v<decltype(v), float>) {
                std::cerr << "Warning: float value " << v << " converted to int for stackTop\n";
                return static_cast<int>(v); // convert float to int
            }
            return v;
        }, memoryStack.back());
        if(DEBUG) std::cout << "Popped " << stackTop << " from the stack" << std::endl;
    } else if(arg == "bp") {
        basePointer = std::visit([](auto v) -> int {
            if constexpr (std::is_same_v<decltype(v), float>) {
                std::cerr << "Warning: float value " << v << " converted to int for basePointer\n";
                return static_cast<int>(v); // convert float to int
            }
            return v;
        }, memoryStack.back());
        if(DEBUG) std::cout << "Popped " << basePointer << " from the stack" << std::endl;
    } else {
        generalPurposeRegister = memoryStack.back();
        if(DEBUG) {
            std::visit([](auto v) {
                std::cout << "Popped " << v << " from the stack" << std::endl;
            }, generalPurposeRegister);
        }
    }

    memoryStack.pop_back();
    stackTop--;
}

void StackMachine::pop() {
    if (!validAddress(stackTop)) return;
    generalPurposeRegister = memoryStack.back();
    if(DEBUG) {
        std::visit([](auto v) {
            std::cout << "Popped " << v << " from the stack" << std::endl;
        }, generalPurposeRegister);
    }
    memoryStack.pop_back();
    stackTop--;
}

void StackMachine::dup() {
    if (!validAddress(stackTop)) return;
    memoryStack.push_back(memoryStack.back());
    generalPurposeRegister = memoryStack.back();
    stackTop++;
    if(DEBUG) {
        std::visit([](auto v) {
            std::cout << "Duplicated " << v << " onto the stack" << std::endl;
        }, generalPurposeRegister);
    }
}

void StackMachine::load(const std::string &arg) {
    pop();
    int addr = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int for memory access in load()\n";
            return static_cast<int>(v);
        }
        return v;
    }, generalPurposeRegister);

    if (arg == "bp") {
        addr += basePointer;
    } else if (arg == "top") {
        addr += stackTop - 1;
    }

    if (!validAddress(addr)) return;
    push(std::visit([](auto v) { return std::to_string(v); }, memoryStack[addr]));
}

void StackMachine::save(const std::string &arg) {
    pop();
    int addr = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int for memory access in save()\n";
            return static_cast<int>(v);
        }
        return v;
    }, generalPurposeRegister);

    if (arg == "bp") {
        addr += basePointer;
    } else if (arg == "top") {
        addr += stackTop - 1;
    }

    if (!validAddress(addr)) return;
    memoryStack[addr] = generalPurposeRegister = memoryStack.back();
}

void StackMachine::store(const std::string &arg) {
    pop();
    int addr = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int for memory access in store()\n";
            return static_cast<int>(v);
        }
        return v;
    }, generalPurposeRegister);

    if (arg == "bp") {
        addr += basePointer;
    } else if (arg == "top") {
        addr += stackTop - 1;
    }

    if (!validAddress(addr)) return;
    memoryStack[addr] = generalPurposeRegister = memoryStack.back();
}

// Control flow functions
void StackMachine::call(const std::string &arg) {
    if (arg.empty()) {
        std::cerr << "Error: call requires a function label as argument" << std::endl;
        return;
    }

    if (stackTop <= 0) {
        std::cerr << "Error: Stack is empty. Cannot read argument count." << std::endl;
        return;
    }

    pop();
    int argNum = std::visit([](auto v) {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int in call()\n";
            return static_cast<int>(v);
        }
        return static_cast<int>(v);
    }, generalPurposeRegister);

    if(argNum < 0 || stackTop < argNum) {
        std::cerr << "Error: Invalid argument count or stack underflow." << std::endl;
        return;
    }

    push(std::to_string(basePointer)); // Old base pointer
    push(std::to_string(instructionCounter + 1)); // Return address

    basePointer = stackTop - argNum - 1;

    if(DEBUG) {
        std::cerr << "CALL: argNum = " << argNum << std::endl;
        std::cerr << "CALL: New base pointer = " << basePointer << std::endl;
    }

    jump(arg);
}

void StackMachine::ret() {
    if(basePointer == 0) {
        std::visit([](auto v) {
            if constexpr (std::is_same_v<decltype(v), float>) {
                std::cerr << "Warning: float " << v << " converted to int in ret()\n";
                exit(static_cast<int>(v));
            }
            exit(v);
        }, generalPurposeRegister);
    }

    instructionCounter = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int for instructionCounter in ret()\n";
            return static_cast<int>(v);
        }
        return v;
    }, memoryStack[basePointer]);

    basePointer = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int for basePointer in ret()\n";
            return static_cast<int>(v);
        }
        return v;
    }, memoryStack[basePointer - 1]);
}

void StackMachine::retv() {
    pop();
    if(basePointer == 0) {
        push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
        std::visit([](auto v) {
            if constexpr (std::is_same_v<decltype(v), float>) {
                std::cerr << "Warning: float " << v << " converted to int in retv()\n";
                exit(static_cast<int>(v));
            }
            exit(v);
        }, generalPurposeRegister);
    }

    instructionCounter = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int for instructionCounter in retv()\n";
            return static_cast<int>(v);
        }
        return v;
    }, memoryStack[basePointer]);

    basePointer = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int for basePointer in retv()\n";
            return static_cast<int>(v);
        }
        return v;
    }, memoryStack[basePointer - 1]);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::brt(const std::string &arg) {
    if (arg.empty()) {
        std::cerr << "Error: brt requires an argument" << std::endl;
        return;
    }

    pop();

    int val = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int in brt()\n";
            return static_cast<int>(v);
        }
        return v;
    }, generalPurposeRegister);

    if(val == 1) {
        jump(arg);
    }
}

void StackMachine::brz(const std::string &arg) {
    if (arg.empty()) {
        std::cerr << "Error: brz requires an argument" << std::endl;
        return;
    }

    pop();
    int val = std::visit([](auto v) -> int {
        if constexpr (std::is_same_v<decltype(v), float>) {
            std::cerr << "Warning: float " << v << " converted to int in brt()\n";
            return static_cast<int>(v);
        }
        return v;
    }, generalPurposeRegister);

    if(val == 0) {
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

    if(DEBUG) std::cout << "Jump to " << arg << std::endl;
}

// Arithmetic functions
void StackMachine::neg() {
    if(stackTop <= 0) {
        std::cerr << "Error: Stack underflow in neg()\n";
        return;
    }
    auto &top = memoryStack.back();
    std::visit([](auto &v) { v = -v; }, top);
}

void StackMachine::add() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();

    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return a + b;
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::sub() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();

    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return a - b;
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::mul() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();

    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return a * b;
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::div() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();

    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return a / b;
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::mod() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();

    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        if constexpr (std::is_same_v<decltype(a), int> && std::is_same_v<decltype(b), int>) {
            return a % b;
        } else {
            std::cerr << "Error: cannot perform modulus on a float" << std::endl;
            return 0;
        }
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

// Relational operator functions
void StackMachine::eq() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return static_cast<int>(a == b);
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::neq() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return static_cast<int>(a != b);
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::lt() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return static_cast<int>(a < b);
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::lte() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return static_cast<int>(a <= b);
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::gt() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return static_cast<int>(a > b);
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

void StackMachine::gte() {
    if (!validAddress(stackTop)) return;
    pop();
    const Value temp = generalPurposeRegister;
    pop();
    generalPurposeRegister = std::visit([](auto a, auto b) -> Value {
        return static_cast<int>(a >= b);
    }, generalPurposeRegister, temp);

    push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
}

// Special functions
void StackMachine::print(const std::string &arg) {
    if (arg.empty()) {
        if (stackTop <= 0) {
            std::cerr << "Error: Stack is empty. Nothing to print." << std::endl;
            return;
        }

        std::visit([](auto v) {
            std::cout << v << std::endl;
        }, memoryStack.back());

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
    std::string input;
    std::cin >> input;

    try {
        push(input);
    } catch(...) {
        std::cerr << "Error: Invalid input for read()\n";
    }
}

void StackMachine::end(const std::string &arg) {
    if(arg.empty()) {
        push(std::visit([](auto v) { return std::to_string(v); }, generalPurposeRegister));
        std::visit([](auto v) {
            if constexpr (std::is_same_v<decltype(v), float>) {
                std::cerr << "Warning: float " << v << " converted to int in end()\n";
                exit(static_cast<int>(v));
            }
            exit(v);
        }, generalPurposeRegister);
    } else if(arg == "bp") {
        push(std::to_string(basePointer));
        exit(basePointer);
    } else if(arg == "top") {
        push(std::to_string(stackTop));
        exit(stackTop);
    } else {
        push(arg);
        std::visit([](auto v) {
            if constexpr (std::is_same_v<decltype(v), float>) {
                std::cerr << "Warning: float " << v << " converted to int in end()\n";
                exit(static_cast<int>(v));
            }
            exit(v);
        }, memoryStack.back());
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
            auto it = labelMap.find(key);
            if(it != labelMap.end()) {
                instructionCounter++;
                continue;
            }
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

        instructionQueue.push_back({token, argument});
        index++;
    }

//    if (index >= MAX_INSTRUCTION_COUNT) {
//        std::cerr << "Warning: Program truncated to fit instruction memory" << std::endl;
//    }

    return true;
}

void StackMachine::printInstructionQueue() const {
    int index = 0;
    for (auto &instruction : instructionQueue) {
        if(instruction[0].empty() && instruction[1].empty()) continue;
        std::cout << "Instruction " << index++ << ": " << instruction[0] << " " << instruction[1] << "\n";
    }
}

void StackMachine::printLabelMap() const {
    for (const auto& entry : labelMap) {
        std::cout << "Location: " << entry.second << ", Label: " << entry.first << std::endl;
    }
}
