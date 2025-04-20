cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CompilerProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the build target (stackMachine or other)
option(BUILD_STACK_MACHINE "Build stackMachine project" ON)
option(BUILD_LEXICAL_ANALYZER "Build lexicalAnalyzer project" ON)
option(BUILD_PARSER "Build parser project" ON)
option(BUILD_COMPILER "Build compiler project" ON)

# Set the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (BUILD_STACK_MACHINE)
    # Collect all source files in the stackMachine directory
    file(GLOB STACK_MACHINE_SOURCES stackMachine/*.cpp)

    # Define the stackMachine executable
    add_executable(stackMachine ${STACK_MACHINE_SOURCES})

    # Enable warnings for better code safety
    target_compile_options(stackMachine PRIVATE -Wall -Wextra -Wpedantic)

    # Handle Debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(stackMachine PRIVATE DEBUG)
    endif()
endif()

if (BUILD_LEXICAL_ANALYZER)
    # Collect all source files in the Lexer directory
    file(GLOB LEXICAL_ANALYZER_SOURCES Lexer/*.cpp)

    # Define the Lexer executable
    add_executable(lexicalAnalyzer ${LEXICAL_ANALYZER_SOURCES})

    # Enable warnings for better code safety
    target_compile_options(lexicalAnalyzer PRIVATE -Wall -Wextra -Wpedantic)

    # Handle Debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(lexicalAnalyzer PRIVATE DEBUG)
    endif()
endif()

if (BUILD_PARSER)
    # Collect all source files in the Parser directory
    file(GLOB PARSER_SOURCES Parser/*.cpp)

    # Define the Parser executable
    add_executable(parser ${PARSER_SOURCES}
            Parser/Parser.cpp
            Parser/ast.cpp)

    # Enable warnings for better code safety
    target_compile_options(parser PRIVATE -Wall -Wextra -Wpedantic)

    # Handle Debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(parser PRIVATE DEBUG)
    endif()
endif()

if (BUILD_COMPILER)
    # Collect all source files in the compiler directory
    file(GLOB COMPILER_SOURCES compiler/*.cpp)
    file(GLOB LEXER_SOURCES Lexer/*.cpp)
    file(GLOB PARSER_SOURCES Parser/*.cpp)
    file(GLOB STACK_MACHINE_SOURCES stackMachine/*.cpp)

    # Define the compiler executable
    add_executable(compiler
            ${COMPILER_SOURCES}
            ${LEXER_SOURCES}
            ${PARSER_SOURCES}
            ${STACK_MACHINE_SOURCES}
    )

    # Enable warnings for better code safety
    target_compile_options(compiler PRIVATE -Wall -Wextra -Wpedantic)

    # Handle Debug mode
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(compiler PRIVATE DEBUG)
    endif()
endif()
