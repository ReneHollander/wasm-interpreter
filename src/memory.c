/*
 * Contains everything related to
 * memory instructions
 */

#include <stdlib.h>
#include "memory.h"
#include "interpreter.h"
#include "opd_stack.h"
#include "util.h"

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

#define LOAD_INSN(srctype, targettype) \
    i32 ea = caluclate_address(instr, sizeof(srctype) * 8); \
    srctype value = *((srctype *) (memory->data + ea)); \
    CAT(push_opd_, targettype)((targettype) value)

#define FLOAT_LOAD_INSN(type) \
    i32 ea = caluclate_address(instr, sizeof(type) * 8); \
    type value = *((type *) (memory->data + ea)); \
    CAT(push_opd_, type)(value)

#define STORE_INSN(srctype, intermediatetype, targettype) \
    srctype value = CAT(pop_opd_, srctype)(); \
    intermediatetype v = (intermediatetype) value; \
    i32 ea = caluclate_address(instr, sizeof(targettype) * 8); \
    *((targettype *) (memory->data + ea)) = (targettype) (v & BIT_MASK(intermediatetype, sizeof(targettype) * 8))

#define FLOAT_STORE_INSN(type) \
    type value = CAT(pop_opd_, type)(); \
    i32 ea = caluclate_address(instr, sizeof(type) * 8); \
    *((type *) (memory->data + ea)) = value

    opcode_t opcode = instr.opcode;
    if (opcode == OP_I32_LOAD) {
        LOAD_INSN(i32, i32);
    } else if (opcode == OP_I64_LOAD) {
        LOAD_INSN(i64, i64);
    } else if (opcode == OP_F32_LOAD) {
        FLOAT_LOAD_INSN(f32);
    } else if (opcode == OP_F64_LOAD) {
        FLOAT_LOAD_INSN(f64);
    } else if (opcode == OP_I32_LOAD8_S) {
        LOAD_INSN(s8, i32);
    } else if (opcode == OP_I32_LOAD8_U) {
        LOAD_INSN(u8, i32);
    } else if (opcode == OP_I32_LOAD16_S) {
        LOAD_INSN(s16, i32);
    } else if (opcode == OP_I32_LOAD16_U) {
        LOAD_INSN(u16, i32);
    } else if (opcode == OP_I64_LOAD8_S) {
        LOAD_INSN(s8, i64);
    } else if (opcode == OP_I64_LOAD8_U) {
        LOAD_INSN(u8, i64);
    } else if (opcode == OP_I64_LOAD16_S) {
        LOAD_INSN(s16, i64);
    } else if (opcode == OP_I64_LOAD16_U) {
        LOAD_INSN(u16, i64);
    } else if (opcode == OP_I64_LOAD32_S) {
        LOAD_INSN(s32, i64);
    } else if (opcode == OP_I64_LOAD32_U) {
        LOAD_INSN(u32, i64);
    } else if (opcode == OP_I32_STORE) {
        STORE_INSN(i32, u32, i32);
    } else if (opcode == OP_I64_STORE) {
        STORE_INSN(i64, u64, i64);
    } else if (opcode == OP_F32_STORE) {
        FLOAT_STORE_INSN(f32);
    } else if (opcode == OP_F64_STORE) {
        FLOAT_STORE_INSN(f64);
    } else if (opcode == OP_I32_STORE8) {
        STORE_INSN(i32, u32, u8);
    } else if (opcode == OP_I32_STORE16) {
        STORE_INSN(i32, u32, u16);
    } else if (opcode == OP_I64_STORE8) {
        STORE_INSN(i64, u64, u8);
    } else if (opcode == OP_I64_STORE16) {
        STORE_INSN(i64, u64, u16);
    } else if (opcode == OP_I64_STORE32) {
        STORE_INSN(i64, u64, u32);
    } else if (opcode == OP_MEMORY_SIZE) {
        push_opd_i32(memory->size);
    } else if (opcode == OP_MEMORY_GROW) {
        interpreter_error("memory opcode not implemented\n");
    } else {
        interpreter_error("unknown memory opcode\n");
    }
}