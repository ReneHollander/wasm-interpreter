cmake_minimum_required(VERSION 3.14)
project(wasm_interpreter)

set(CMAKE_CXX_STANDARD 17)

add_executable(wasm_interpreter src/main.cpp)
