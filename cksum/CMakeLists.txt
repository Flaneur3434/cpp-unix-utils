cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")


# Export the compilation commands to a file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp-unix-utils)

# Create the build directory
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
if(NOT EXISTS ${BUILD_DIR})
    file(MAKE_DIRECTORY ${BUILD_DIR})
endif()

# Create the executable
add_executable(cksum cksum.cpp)

# Set the output directory for the executable
set_target_properties(cksum PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

# Compiler flags for this target
target_compile_options(cksum PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnon-virtual-dtor -Wnull-dereference -Wuninitialized -Wuninitialized -Wstrict-aliasing -Wpessimizing-move -Wredundant-move)
