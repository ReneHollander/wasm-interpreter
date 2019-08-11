cmake_minimum_required(VERSION 3.14)

project(wasm_interpreter C)

set(CMAKE_C_FLAGS_RELEASE "-O3 -flto")

set(CMAKE_C_FLAGS_ASAN "-g -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_EXE_LINKER_FLAGS_ASAN "-fsanitize=address")

add_library(exception
        src/exception.h
        src/exception.c
        )
set_property(TARGET exception PROPERTY C_STANDARD 11)

add_library(parser
        src/type.h
        src/instruction.h
        src/module.h
        src/value.h
        src/strings.h
        src/parser.h
        src/parser.c
        src/exception.h
        )
set_property(TARGET parser PROPERTY C_STANDARD 11)
target_link_libraries(parser exception)

add_library(interpreter
        src/interpreter.c
        src/interpreter.h
        src/stack.h
        src/handler.h
        src/numeric_opcode_handlers.h
        src/variable_opcode_handlers.h
        src/parametric_opcode_handlers.h
        src/memory_opcode_handlers.h
        src/control_opcode_handlers.h
        src/memory.c
        src/memory.h
        src/opcode.h
        src/variable.c
        src/variable.h
        src/control.c
        src/control.h
        src/eval_types.h
        src/table.c
        src/table.h
        src/import.c
        src/import.h
        src/vec.h
        )
set_property(TARGET interpreter PROPERTY C_STANDARD 11)
target_link_libraries(interpreter m)
target_link_libraries(interpreter exception)

add_executable(wasm_interpreter
        src/main.c
        )
set_property(TARGET wasm_interpreter PROPERTY C_STANDARD 11)
target_link_libraries(wasm_interpreter parser)
target_link_libraries(wasm_interpreter interpreter)
target_link_libraries(wasm_interpreter exception)

add_executable(test_vec
        src/util.h
        src/value.h
        src/vec.h
        src/test_vec.c
        )
set_property(TARGET test_vec PROPERTY C_STANDARD 11)
target_link_libraries(test_vec m)
target_link_libraries(test_vec exception)

add_executable(test_exception
        src/exception.h
        src/test_exception.c
        )
set_property(TARGET test_exception PROPERTY C_STANDARD 11)
target_link_libraries(test_exception exception)