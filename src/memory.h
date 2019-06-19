#ifndef WASM_INTERPRETER_MEMORY_H
#define WASM_INTERPRETER_MEMORY_H

#include "value.h"
#include "instruction.h"

#define PAGE_SIZE (65536)

typedef struct memory {
    i32 max_size;
    i32 size;
    byte *data;
} memory_t;

memory_t *create_memory(i32 max_size);

void use_memory(memory_t *m);

void free_memory(memory_t *m);

bool is_memory_instr(opcode_t opcode);

void eval_memory_instr(instruction_t instr);

#endif //WASM_INTERPRETER_MEMORY_H
