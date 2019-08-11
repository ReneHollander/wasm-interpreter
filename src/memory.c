#include <stdlib.h>

#include "memory.h"

static memory_t *memory = NULL;

memory_t *create_memory(i32 size) {
    memory_t *m = malloc(sizeof(memory_t));
    m->size = size;
    m->data = malloc(sizeof(byte) * size * PAGE_SIZE);
    return m;
}

void use_memory(memory_t *m) {
    memory = m;
}

memory_t *get_current_memory() {
    return memory;
}

void free_memory(memory_t *m) {
    if (m->data != NULL) {
        free(m->data);
    }
    free(m);
}

void init_memory(eval_state_t *eval_state, memtype_t mem) {
    if (memory == NULL) {
        THROW_EXCEPTION_WITH_MSG(EXCEPTION_INTERPRETER_NO_MEMORY, "this module needs memory");
    }

    if (mem.lim.min > memory->size) {
        THROW_EXCEPTION_WITH_MSG(EXCEPTION_INTERPRETER_INVALID_MEMORY, "memory size is too small for this import");
    }

    if (mem.lim.has_max && mem.lim.max > memory->max_size) {
        THROW_EXCEPTION_WITH_MSG(EXCEPTION_INTERPRETER_INVALID_MEMORY,
                                 "maximum memory size is too small for this import");
    }
}
