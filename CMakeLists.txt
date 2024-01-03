cmake_minimum_required(VERSION 3.13)  # CMake version check
project(raytracing_weekend)               # Create project with name
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of the project root directory as a source file
set(SOURCE_FILES main.cc)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(raytracing_weekend ${SOURCE_FILES})