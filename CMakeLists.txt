cmake_minimum_required(VERSION 3.14)

project(wasm_interpreter C)

add_library(wasm_parser
        src/type.h
        src/instruction.h
        src/module.h
        src/value.h
        src/parser.h
        src/parser.c
        )
set_property(TARGET wasm_parser PROPERTY C_STANDARD 99)

add_executable(wasm_interpreter
        src/parser.h
        src/main.c
        )
set_property(TARGET wasm_interpreter PROPERTY C_STANDARD 99)

target_link_libraries(wasm_interpreter wasm_parser)
