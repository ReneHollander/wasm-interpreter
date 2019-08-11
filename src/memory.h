#ifndef WASM_INTERPRETER_MEMORY_H
#define WASM_INTERPRETER_MEMORY_H

#include "value.h"
#include "instruction.h"
#include "interpreter.h"

#define PAGE_SIZE (65536)

typedef struct memory {
    i32 max_size;
    i32 size;
    byte *data;
} memory_t;

memory_t *create_memory(i32 max_size);

void use_memory(memory_t *m);

memory_t *get_current_memory();

void free_memory(memory_t *m);

void init_memory(eval_state_t *eval_state, memtype_t mem);

#endif //WASM_INTERPRETER_MEMORY_H
