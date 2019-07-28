#include <stdlib.h>

#include "memory.h"
#include "interpreter.h"
#include "util.h"
#include "stack.h"

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
        interpreter_error(eval_state, "this module needs memory");
    }

    if (mem.lim.min > memory->size) {
        interpreter_error(eval_state, "memory size is too small for this import");
    }

    if (mem.lim.has_max && mem.lim.max > memory->max_size) {
        interpreter_error(eval_state, "maximum memory size is too small for this import");
    }
}

bool is_memory_instr(const opcode_t *opcode) {
    switch (*opcode) {
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

i32 calculate_address(eval_state_t *eval_state, instruction_t instr, i32 bit_width) {
    i32 a = pop_i32(eval_state->opd_stack);
    i32 ea = a + instr.memarg.offset;
    if ((ea + bit_width / 8) > (memory->size * PAGE_SIZE)) {
        interpreter_error(eval_state, "memory access is out of bounds\n");
    }
    return ea;
}

void eval_memory_instr(eval_state_t *eval_state, instruction_t instr) {
    if (memory == NULL) {
        interpreter_error(eval_state, "no memory found\n");
    }

#define LOAD_INSN(eval_state, srctype, targettype) \
    i32 ea = calculate_address(eval_state, instr, sizeof(srctype) * 8); \
    srctype value = *((srctype *) (memory->data + ea)); \
    CAT(push_, targettype)(eval_state->opd_stack, (targettype) value)

#define FLOAT_LOAD_INSN(eval_state, type) \
    i32 ea = calculate_address(eval_state, instr, sizeof(type) * 8); \
    type value = *((type *) (memory->data + ea)); \
    CAT(push_, type)(eval_state->opd_stack, value)

#define STORE_INSN(eval_state, srctype, intermediatetype, targettype) \
    srctype value = CAT(pop_, srctype)(eval_state->opd_stack); \
    intermediatetype v = (intermediatetype) value; \
    i32 ea = calculate_address(eval_state, instr, sizeof(targettype) * 8); \
    *((targettype *) (memory->data + ea)) = (targettype) (v & BIT_MASK(intermediatetype, sizeof(targettype) * 8))

#define FLOAT_STORE_INSN(eval_state, type) \
    type value = CAT(pop_, type)(eval_state->opd_stack); \
    i32 ea = calculate_address(eval_state, instr, sizeof(type) * 8); \
    *((type *) (memory->data + ea)) = value

    opcode_t opcode = instr.opcode;
    if (opcode == OP_I32_LOAD) {
        LOAD_INSN(eval_state, i32, i32);
    } else if (opcode == OP_I64_LOAD) {
        LOAD_INSN(eval_state, i64, i64);
    } else if (opcode == OP_F32_LOAD) {
        FLOAT_LOAD_INSN(eval_state, f32);
    } else if (opcode == OP_F64_LOAD) {
        FLOAT_LOAD_INSN(eval_state, f64);
    } else if (opcode == OP_I32_LOAD8_S) {
        LOAD_INSN(eval_state, s8, i32);
    } else if (opcode == OP_I32_LOAD8_U) {
        LOAD_INSN(eval_state, u8, i32);
    } else if (opcode == OP_I32_LOAD16_S) {
        LOAD_INSN(eval_state, s16, i32);
    } else if (opcode == OP_I32_LOAD16_U) {
        LOAD_INSN(eval_state, u16, i32);
    } else if (opcode == OP_I64_LOAD8_S) {
        LOAD_INSN(eval_state, s8, i64);
    } else if (opcode == OP_I64_LOAD8_U) {
        LOAD_INSN(eval_state, u8, i64);
    } else if (opcode == OP_I64_LOAD16_S) {
        LOAD_INSN(eval_state, s16, i64);
    } else if (opcode == OP_I64_LOAD16_U) {
        LOAD_INSN(eval_state, u16, i64);
    } else if (opcode == OP_I64_LOAD32_S) {
        LOAD_INSN(eval_state, s32, i64);
    } else if (opcode == OP_I64_LOAD32_U) {
        LOAD_INSN(eval_state, u32, i64);
    } else if (opcode == OP_I32_STORE) {
        STORE_INSN(eval_state, i32, u32, i32);
    } else if (opcode == OP_I64_STORE) {
        STORE_INSN(eval_state, i64, u64, i64);
    } else if (opcode == OP_F32_STORE) {
        FLOAT_STORE_INSN(eval_state, f32);
    } else if (opcode == OP_F64_STORE) {
        FLOAT_STORE_INSN(eval_state, f64);
    } else if (opcode == OP_I32_STORE8) {
        STORE_INSN(eval_state, i32, u32, u8);
    } else if (opcode == OP_I32_STORE16) {
        STORE_INSN(eval_state, i32, u32, u16);
    } else if (opcode == OP_I64_STORE8) {
        STORE_INSN(eval_state, i64, u64, u8);
    } else if (opcode == OP_I64_STORE16) {
        STORE_INSN(eval_state, i64, u64, u16);
    } else if (opcode == OP_I64_STORE32) {
        STORE_INSN(eval_state, i64, u64, u32);
    } else if (opcode == OP_MEMORY_SIZE) {
        push_i32(eval_state->opd_stack, memory->size);
    } else if (opcode == OP_MEMORY_GROW) {
        interpreter_error(eval_state, "memory opcode not implemented\n");
    } else {
        interpreter_error(eval_state, "unknown memory opcode\n");
    }
}