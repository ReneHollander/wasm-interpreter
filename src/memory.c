/*
 * Contains everything related to
 * memory instructions
 */

#include <stdlib.h>
#include "memory.h"
#include "interpreter.h"
#include "opd_stack.h"

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

void free_memory(memory_t *m) {
    if (m->data != NULL) {
        free(m->data);
    }
    free(m);
}

void init_memory(memtype_t mem) {
    if (memory == NULL) {
        interpreter_error("this module needs memory");
    }

    if (mem.lim.min > memory->size) {
        interpreter_error("memory size is too small for this import");
    }

    if (mem.lim.has_max && mem.lim.max > memory->max_size) {
        interpreter_error("maximum memory size is too small for this import");
    }
}

bool is_memory_instr(opcode_t opcode) {
    switch (opcode) {
        case OP_I32_LOAD:
        case OP_I64_LOAD:
        case OP_F32_LOAD:
        case OP_F64_LOAD:
        case OP_I32_LOAD8_S:
        case OP_I32_LOAD8_U:
        case OP_I32_LOAD16_S:
        case OP_I32_LOAD16_U:
        case OP_I64_LOAD8_S:
        case OP_I64_LOAD8_U:
        case OP_I64_LOAD16_S:
        case OP_I64_LOAD16_U:
        case OP_I64_LOAD32_S:
        case OP_I64_LOAD32_U:
        case OP_I32_STORE:
        case OP_I64_STORE:
        case OP_F32_STORE:
        case OP_F64_STORE:
        case OP_I32_STORE8:
        case OP_I32_STORE16:
        case OP_I64_STORE8:
        case OP_I64_STORE16:
        case OP_I64_STORE32:
        case OP_MEMORY_SIZE:
        case OP_MEMORY_GROW:
            return true;
        default:
            return false;
    }
}

i32 caluclate_address(instruction_t instr, i32 bit_width) {
    i32 a = pop_opd_i32();
    i32 ea = a + instr.memarg.offset;
    if ((ea + bit_width / 8) > (memory->size * PAGE_SIZE)) {
        interpreter_error("memory access is out of bounds\n");
    }
    return ea;
}

void eval_memory_instr(instruction_t instr) {
    if (memory == NULL) {
        interpreter_error("no memory found\n");
    }

    opcode_t opcode = instr.opcode;
    if (opcode == OP_I32_LOAD) {
        i32 ea = caluclate_address(instr, 32);
        i32 value = ((i32 *) memory->data)[ea];
        push_opd_i32(value);
    } else if (opcode == OP_I64_LOAD) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_F32_LOAD) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_F64_LOAD) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I32_LOAD8_S) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I32_LOAD8_U) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I32_LOAD16_S) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I32_LOAD16_U) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_LOAD8_S) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_LOAD8_U) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_LOAD16_S) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_LOAD16_U) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_LOAD32_S) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_LOAD32_U) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I32_STORE) {
        i32 value = pop_opd_i32();
        i32 ea = caluclate_address(instr, 32);
        ((i32 *) memory->data)[ea] = value;
    } else if (opcode == OP_I64_STORE) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_F32_STORE) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_F64_STORE) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I32_STORE8) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I32_STORE16) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_STORE8) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_STORE16) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_I64_STORE32) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_MEMORY_SIZE) {
        interpreter_error("memory opcode not implemented\n");
    } else if (opcode == OP_MEMORY_GROW) {
        interpreter_error("memory opcode not implemented\n");
    } else {
        interpreter_error("unknown memory opcode\n");
    }
}