cmake_minimum_required(VERSION 3.13)  # CMake version check
project(HelloWorld)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 17)            # Enable c++17 standard

# Add main.cpp file of the project root directory as a source file
set(SOURCE_FILES HelloWorld.c)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(HelloWorld.exe ${SOURCE_FILES})