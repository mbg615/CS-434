# CS-434 Compiler Construction

This repository contains most of the code I wrote during my Computer Science 434 class at the University of Alabama. The project is a toy compiler for a C-like language, featuring a hand-written lexical analyzer, recursive descent parser, and a stack-based virtual machine.

## Overview

The compiler consists of several components:

- **Lexical Analyzer**: A custom lexer written without Flex. 
It tokenizes the source code and handles basic error reporting 
(e.g., unterminated strings, invalid escapes, unrecognized characters).


- **Parser**: A recursive descent parser that builds an abstract syntax tree (AST) from the tokens. 
It handles precedence rules, supports both expressions and statements, and produces useful error messages.


- **AST**: The AST classes are defined using a `std::unique_ptr<AST>` model. 
Each node includes virtual methods like `emit()` and `emitStackCode()` to produce code for the VM.


- **Stack Machine**: A custom stack-based VM with:
    - Support for `int` and `float` using `std::variant`
    - Basic stack operations (`push`, `pop`, `load`, `store`)
    - Arithmetic expressions with proper type handling at runtime
    - Function call mechanism (currently **WIP** and not functioning properly)


- **Error Reporting**: Line and column tracking are implemented to give clear diagnostics during lexing and parsing.

## Planned / In Progress
- Proper function call support and argument handling
- Type enforcement during variable declaration and assignment
- Error recovery during parsing
- Additional control flow constructs (e.g., `for`)

  ## Building

This project uses CMake.
``` bash
mkdir build
cd build
cmake ..
```

Then build a specific component with:
``` bash
make compiler           # Full pipeline (lexer + parser + VM)
make lexicalAnalyzer    # Lexer only
make parser             # Parser only
make stackMachine       # VM only
```

> [!NOTE]
> Each component includes its own `main.cpp` for standalone testing. These are excluded from the full `compiler` build to avoid conflicts.

## Status

This is a work-in-progress compiler intended for learning and experimentation. Some features may be incomplete, and others may be added in the future as time allows.
