cmake_minimum_required(VERSION 3.14)

project(wasm_interpreter C)

add_library(wasm_parser
        src/type.h
        src/instruction.h
        src/module.h
        src/value.h
        src/strings.h
        src/parser.h
        src/parser.c
        )
set_property(TARGET wasm_parser PROPERTY C_STANDARD 99)

add_executable(wasm_interpreter
        src/parser.h
        src/main.c
        src/interpreter.c
        src/interpreter.h
        src/stack.c
        src/stack.h
        src/numeric.c
        src/numeric.h
        src/list.c
        src/list.h
        src/memory.c
        src/memory.h
        src/variable.c
        src/variable.h src/control.c src/control.h)
set_property(TARGET wasm_interpreter PROPERTY C_STANDARD 99)

target_link_libraries(wasm_interpreter wasm_parser)
