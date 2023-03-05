cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

# Export the compilation commands to a file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp-unix-utils)

# Create the executable
add_executable(cksum cksum.cpp)

# Compiler flags for this target
add_compile_options(-std=c++20)
target_compile_options(cksum PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnon-virtual-dtor -Wnull-dereference -Wuninitialized -Wuninitialized -Wstrict-aliasing -Wpessimizing-move -Wredundant-move)
