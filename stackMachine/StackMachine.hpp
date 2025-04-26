#ifndef STACKMACHINE_HPP
#define STACKMACHINE_HPP

#define MAX_INSTRUCTION_COUNT 1024
#define DEBUG 0

#include <unordered_map>
#include <variant>
#include <functional>
#include <string>

using Value = std::variant<int, float>;

class StackMachine {
private:
    std::unordered_map<std::string, std::variant<std::function<void(std::string)>, std::function<void()>>> instructionImplementationMap;
    Value generalPurposeRegister = 0; // General Purpose Register

    // Instruction model
    std::string instructionQueue[MAX_INSTRUCTION_COUNT][2];
    std::unordered_map<std::string, int> labelMap;
    int instructionCounter = 0; // (pc) Next instruction to execute

    // Stack model
    Value memoryStack[4096]{};
    int stackTop = 0; // (top) Next open slot in memory stack
    int basePointer = 0; // (bp) Base frame of current function
    std::vector<int> returnAddressStack;

    static int validAddress(const int addr);

public:
    StackMachine();

    void push(const std::string &arg);
    void pop(const std::string &arg);
    void pop();
    void dup();
    void load(const std::string &arg);
    void save(const std::string &arg);
    void store(const std::string &arg);
    void call(const std::string &arg);
    void ret();
    void retv();
    void brt(const std::string &arg);
    void brz(const std::string &arg);
    void jump(const std::string &arg);

    void neg();
    void add();
    void sub();
    void mul();
    void div();
    void mod();

    void eq();
    void neq();
    void lt();
    void lte();
    void gt();
    void gte();

    void print(const std::string &arg);
    void read();
    void end(const std::string &arg);
    bool runProgram();
    bool loadProgramFromFile(const std::string &filename);
    void printInstructionQueue() const;
    void printLabelMap() const;
};

#endif //STACKMACHINE_HPP
