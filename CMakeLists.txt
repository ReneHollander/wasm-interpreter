cmake_minimum_required(VERSION 3.14)

project(wasm_interpreter C)

add_library(parser
        src/type.h
        src/instruction.h
        src/module.h
        src/value.h
        src/strings.h
        src/parser.h
        src/parser.c
        )
set_property(TARGET parser PROPERTY C_STANDARD 99)

add_library(interpreter
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
        src/variable.h
        src/control.c
        src/control.h
        src/opd_stack.c
        src/opd_stack.h
        src/eval_types.h
        src/table.c
        src/table.h
        src/import.c
        src/import.h)
set_property(TARGET interpreter PROPERTY C_STANDARD 99)
target_link_libraries(interpreter m)

add_executable(wasm_interpreter
        src/main.c
        )
set_property(TARGET wasm_interpreter PROPERTY C_STANDARD 99)
target_link_libraries(wasm_interpreter parser)
target_link_libraries(wasm_interpreter interpreter)
