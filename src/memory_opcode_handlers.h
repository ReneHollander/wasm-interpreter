#ifndef WASM_INTERPRETER_MEMORY_OPCODE_HANDLERS_H
#define WASM_INTERPRETER_MEMORY_OPCODE_HANDLERS_H

#include "type.h"
#include "instruction.h"
#include "interpreter.h"
#include "math.h"
#include "stack.h"
#include "handler.h"
#include "memory.h"
#include "util.h"
#include "strings.h"

static i32 calculate_address(eval_state_t *eval_state, instruction_t *instr, i32 bit_width) {
    i32 a = pop_i32(eval_state->opd_stack);
    i32 ea = a + instr->memarg.offset;
    if ((ea + bit_width / 8) > (get_current_memory()->size * PAGE_SIZE)) {
        THROW_EXCEPTION(EXCEPTION_INTERPRETER_MEMORY_ACCESS_OUT_OF_BOUNDS);
    }
    return ea;
}

#define LOAD_INSN(eval_state, srctype, targettype) \
    if (get_current_memory() == NULL) { \
        THROW_EXCEPTION(EXCEPTION_INTERPRETER_NO_MEMORY); \
    } \
    i32 ea = calculate_address(eval_state, instr, sizeof(srctype) * 8); \
    srctype value = *((srctype *) (get_current_memory()->data + ea)); \
    CAT(push_, targettype)(eval_state->opd_stack, (targettype) value)

#define FLOAT_LOAD_INSN(eval_state, type) \
    if (get_current_memory() == NULL) { \
        THROW_EXCEPTION(EXCEPTION_INTERPRETER_NO_MEMORY); \
    } \
    i32 ea = calculate_address(eval_state, instr, sizeof(type) * 8); \
    type value = *((type *) (get_current_memory()->data + ea)); \
    CAT(push_, type)(eval_state->opd_stack, value)

#define STORE_INSN(eval_state, srctype, intermediatetype, targettype) \
    if (get_current_memory() == NULL) { \
        THROW_EXCEPTION(EXCEPTION_INTERPRETER_NO_MEMORY); \
    } \
    srctype value = CAT(pop_, srctype)(eval_state->opd_stack); \
    intermediatetype v = (intermediatetype) value; \
    i32 ea = calculate_address(eval_state, instr, sizeof(targettype) * 8); \
    *((targettype *) (get_current_memory()->data + ea)) = (targettype) (v & BIT_MASK(intermediatetype, sizeof(targettype) * 8))

#define FLOAT_STORE_INSN(eval_state, type) \
    if (get_current_memory() == NULL) { \
        THROW_EXCEPTION(EXCEPTION_INTERPRETER_NO_MEMORY); \
    } \
    type value = CAT(pop_, type)(eval_state->opd_stack); \
    i32 ea = calculate_address(eval_state, instr, sizeof(type) * 8); \
    *((type *) (get_current_memory()->data + ea)) = value

OP_HANDLER(OP_I32_LOAD) {
    LOAD_INSN(eval_state, i32, i32);
}

OP_HANDLER(OP_I64_LOAD) {
    LOAD_INSN(eval_state, i64, i64);
}

OP_HANDLER(OP_F32_LOAD) {
    FLOAT_LOAD_INSN(eval_state, f32);
}

OP_HANDLER(OP_F64_LOAD) {
    FLOAT_LOAD_INSN(eval_state, f64);
}

OP_HANDLER(OP_I32_LOAD8_S) {
    LOAD_INSN(eval_state, s8, i32);
}

OP_HANDLER(OP_I32_LOAD8_U) {
    LOAD_INSN(eval_state, u8, i32);
}

OP_HANDLER(OP_I32_LOAD16_S) {
    LOAD_INSN(eval_state, s16, i32);
}

OP_HANDLER(OP_I32_LOAD16_U) {
    LOAD_INSN(eval_state, u16, i32);
}

OP_HANDLER(OP_I64_LOAD8_S) {
    LOAD_INSN(eval_state, s8, i64);
}

OP_HANDLER(OP_I64_LOAD8_U) {
    LOAD_INSN(eval_state, u8, i64);
}

OP_HANDLER(OP_I64_LOAD16_S) {
    LOAD_INSN(eval_state, s16, i64);
}

OP_HANDLER(OP_I64_LOAD16_U) {
    LOAD_INSN(eval_state, u16, i64);
}

OP_HANDLER(OP_I64_LOAD32_S) {
    LOAD_INSN(eval_state, s32, i64);
}

OP_HANDLER(OP_I64_LOAD32_U) {
    LOAD_INSN(eval_state, u32, i64);
}

OP_HANDLER(OP_I32_STORE) {
    STORE_INSN(eval_state, i32, u32, i32);
}

OP_HANDLER(OP_I64_STORE) {
    STORE_INSN(eval_state, i64, u64, i64);
}

OP_HANDLER(OP_F32_STORE) {
    FLOAT_STORE_INSN(eval_state, f32);
}

OP_HANDLER(OP_F64_STORE) {
    FLOAT_STORE_INSN(eval_state, f64);
}

OP_HANDLER(OP_I32_STORE8) {
    STORE_INSN(eval_state, i32, u32, u8);
}

OP_HANDLER(OP_I32_STORE16) {
    STORE_INSN(eval_state, i32, u32, u16);
}

OP_HANDLER(OP_I64_STORE8) {
    STORE_INSN(eval_state, i64, u64, u8);
}

OP_HANDLER(OP_I64_STORE16) {
    STORE_INSN(eval_state, i64, u64, u16);
}

OP_HANDLER(OP_I64_STORE32) {
    STORE_INSN(eval_state, i64, u64, u32);
}

OP_HANDLER(OP_MEMORY_SIZE) {
    push_i32(eval_state->opd_stack, get_current_memory()->size);
}

OP_HANDLER(OP_MEMORY_GROW) {
    THROW_EXCEPTION_WITH_MSG(EXCEPTION_INTERPRETER_INVALID_INSTRUCTION, "memory grow opcode not implemented");
}

#undef LOAD_INSN
#undef FLOAT_LOAD_INSN
#undef STORE_INSN
#undef FLOAT_STORE_INSN

#endif // WASM_INTERPRETER_MEMORY_OPCODE_HANDLERS_H
