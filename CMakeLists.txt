#cmake_minimum_required(VERSION 3.10)
#set(CMAKE_CXX_STANDARD 23)
#
#project(compiler434)
#
#add_executable(stackMachine stackMachine/StackMachine.cpp)
#

cmake_minimum_required(VERSION 3.10)

# Set the project name
project(StackProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the build target (stackMachine or other)
option(BUILD_STACK_MACHINE "Build stackMachine project" ON)

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