/*
 * Contains everything related to
 * numeric instructions
 */

#include "stdio.h"
#include "numeric.h"
#include "type.h"
#include "instruction.h"
#include "math.h"
#include "opd_stack.h"
#include "interpreter.h"

typedef void (*numeric_func)(instruction_t *instr);

#define OP_HANDLER(op) \
    static void CAT(op, _HANDLER)(instruction_t *instr) \

#define ADD_OP_HANDLER(op) \
    [op] = op ## _HANDLER

OP_HANDLER(OP_I32_CONST) {
    push_opd_i32(instr->const_i32);
}

OP_HANDLER(OP_I64_CONST) {
    push_opd_i64(instr->const_i64);
}

OP_HANDLER(OP_F32_CONST) {
    push_opd_f32(instr->const_f32);
}

OP_HANDLER(OP_F64_CONST) {
    push_opd_f64(instr->const_f64);
}

OP_HANDLER(OP_F32_ADD) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_f32(op1 + op2);
}

OP_HANDLER(OP_F64_ADD) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_f64(op1 + op2);
}

OP_HANDLER(OP_I32_ADD) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 + op2);
}

OP_HANDLER(OP_I64_ADD) {
    i64 op2 = pop_opd_i64();
    i64 op1 = pop_opd_i64();
    push_opd_i64(op1 + op2);
}

OP_HANDLER(OP_F32_SUB) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_f32(op1 - op2);
}

OP_HANDLER(OP_F64_SUB) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_f64(op1 - op2);
}

OP_HANDLER(OP_I32_SUB) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 - op2);
}

OP_HANDLER(OP_I64_SUB) {
    i64 op2 = pop_opd_i64();
    i64 op1 = pop_opd_i64();
    push_opd_i64(op1 - op2);
}

OP_HANDLER(OP_F32_MUL) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_f32(op1 * op2);
}

OP_HANDLER(OP_F64_MUL) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_f64(op1 * op2);
}

OP_HANDLER(OP_I32_MUL) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 * op2);
}

OP_HANDLER(OP_I64_MUL) {
    i64 op2 = pop_opd_i64();
    i64 op1 = pop_opd_i64();
    push_opd_i64(op1 * op2);
}

OP_HANDLER(OP_F32_MIN) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();

    if (isnan(op1)) {
        push_opd_f32(op1);
    } else if (isnan(op2)) {
        push_opd_f32(op2);
    } else {
        push_opd_f32(op1 <= op2 ? op1 : op2);
    }
}

OP_HANDLER(OP_F64_MIN) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();

    if (isnan(op1)) {
        push_opd_f64(op1);
    } else if (isnan(op2)) {
        push_opd_f64(op2);
    } else {
        push_opd_f64(op1 <= op2 ? op1 : op2);
    }
}

OP_HANDLER(OP_F32_MAX) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();

    if (isnan(op1)) {
        push_opd_f32(op1);
    } else if (isnan(op2)) {
        push_opd_f32(op2);
    } else {
        push_opd_f32(op1 >= op2 ? op1 : op2);
    }
}

OP_HANDLER(OP_F64_MAX) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();

    if (isnan(op1)) {
        push_opd_f64(op1);
    } else if (isnan(op2)) {
        push_opd_f64(op2);
    } else {
        push_opd_f64(op1 >= op2 ? op1 : op2);
    }
}

OP_HANDLER(OP_F32_SQRT) {
    f32 op = pop_opd_f32();
    push_opd_f32(sqrtf(op));
}

OP_HANDLER(OP_F64_SQRT) {
    f64 op = pop_opd_f64();
    push_opd_f64(sqrt(op));
}

OP_HANDLER(OP_F32_FLOOR) {
    f32 op = pop_opd_f32();
    push_opd_f32(floorf(op));
}

OP_HANDLER(OP_F64_FLOOR) {
    f64 op = pop_opd_f64();
    push_opd_f64(floor(op));
}

OP_HANDLER(OP_F32_CEIL) {
    f32 op = pop_opd_f32();
    push_opd_f32(ceilf(op));
}

OP_HANDLER(OP_F64_CEIL) {
    f64 op = pop_opd_f64();
    push_opd_f64(ceil(op));
}

OP_HANDLER(OP_F32_ABS) {
    f32 op = pop_opd_f32();
    push_opd_f32(fabsf(op));
}

OP_HANDLER(OP_F64_ABS) {
    f64 op = pop_opd_f64();
    push_opd_f64(fabs(op));
}

OP_HANDLER(OP_F32_NEG) {
    f32 op = pop_opd_f32();
    push_opd_f32(-op);
}

OP_HANDLER(OP_F64_NEG) {
    f64 op = pop_opd_f64();
    push_opd_f64(-op);
}

OP_HANDLER(OP_I32_OR) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 | op2);
}

OP_HANDLER(OP_I64_OR) {
    i64 op2 = pop_opd_i64();
    i64 op1 = pop_opd_i64();
    push_opd_i64(op1 | op2);
}

OP_HANDLER(OP_I32_XOR) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 ^ op2);
}

OP_HANDLER(OP_I64_XOR) {
    i64 op2 = pop_opd_i64();
    i64 op1 = pop_opd_i64();
    push_opd_i64(op1 ^ op2);
}

OP_HANDLER(OP_I32_AND) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 & op2);
}

OP_HANDLER(OP_I64_AND) {
    i64 op2 = pop_opd_i64();
    i64 op1 = pop_opd_i64();
    push_opd_i64(op1 & op2);
}

OP_HANDLER(OP_I32_EQZ) {
    i32 op = pop_opd_i32();
    push_opd_i32(op == 0 ? 1 : 0);
}

OP_HANDLER(OP_I32_EQ) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 == op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_NE) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 != op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_LT_S) {
    s32 op2 = pop_opd_i32();
    s32 op1 = pop_opd_i32();
    push_opd_i32(op1 < op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_LT_U) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    push_opd_i32(op1 < op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_GE_S) {
    s32 op2 = pop_opd_i32();
    s32 op1 = pop_opd_i32();
    push_opd_i32(op1 >= op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_GE_U) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    push_opd_i32(op1 >= op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_GT_S) {
    s32 op2 = pop_opd_i32();
    s32 op1 = pop_opd_i32();
    push_opd_i32(op1 > op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_GT_U) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    push_opd_i32(op1 > op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_LE_S) {
    s32 op2 = pop_opd_i32();
    s32 op1 = pop_opd_i32();
    push_opd_i32(op1 <= op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_LE_U) {
    i32 op2 = pop_opd_i32();
    i32 op1 = pop_opd_i32();
    push_opd_i32(op1 <= op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_EQZ) {
    i64 op = pop_opd_i64();
    push_opd_i32(op == 0 ? 1 : 0);
}

OP_HANDLER(OP_I64_EQ) {
    i64 op2 = pop_opd_i64();
    i64 op1 = pop_opd_i64();
    push_opd_i32(op1 == op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_NE) {
    i64 op2 = pop_opd_i64();
    i64 op1 = pop_opd_i64();
    push_opd_i32(op1 != op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_LT_S) {
    s64 op2 = pop_opd_i64();
    s64 op1 = pop_opd_i64();
    push_opd_i32(op1 < op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_LT_U) {
    u64 op2 = (u64) pop_opd_i64();
    u64 op1 = (u64) pop_opd_i64();
    push_opd_i32(op1 < op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_GE_S) {
    s64 op2 = pop_opd_i64();
    s64 op1 = pop_opd_i64();
    push_opd_i32(op1 >= op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_GE_U) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    push_opd_i32(op1 >= op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_GT_S) {
    s64 op2 = pop_opd_i64();
    s64 op1 = pop_opd_i64();
    push_opd_i32(op1 > op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_GT_U) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    push_opd_i32(op1 > op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_LE_S) {
    s64 op2 = pop_opd_i64();
    s64 op1 = pop_opd_i64();
    push_opd_i32(op1 <= op2 ? 1 : 0);
}

OP_HANDLER(OP_I64_LE_U) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    push_opd_i32(op1 <= op2 ? 1 : 0);
}

OP_HANDLER(OP_F32_EQ) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_i32(op1 == op2 ? 1 : 0);
}

OP_HANDLER(OP_F32_NE) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_i32(op1 != op2 ? 1 : 0);
}

OP_HANDLER(OP_F32_LT) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_i32(op1 < op2 ? 1 : 0);
}

OP_HANDLER(OP_F32_GE) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_i32(op1 >= op2 ? 1 : 0);
}

OP_HANDLER(OP_F32_GT) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_i32(op1 > op2 ? 1 : 0);
}

OP_HANDLER(OP_F32_LE) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_i32(op1 <= op2 ? 1 : 0);
}

OP_HANDLER(OP_F64_EQ) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_i32(op1 == op2 ? 1 : 0);
}

OP_HANDLER(OP_F64_NE) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_i32(op1 != op2 ? 1 : 0);
}

OP_HANDLER(OP_F64_LT) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_i32(op1 < op2 ? 1 : 0);
}

OP_HANDLER(OP_F64_GE) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_i32(op1 >= op2 ? 1 : 0);
}

OP_HANDLER(OP_F64_GT) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_i32(op1 > op2 ? 1 : 0);
}

OP_HANDLER(OP_F64_LE) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_i32(op1 <= op2 ? 1 : 0);
}

OP_HANDLER(OP_I32_CLZ) {
    i32 op = peek_opd_i32();
    push_opd_i32(op == 0 ? 32 : __builtin_clz(op));
}

OP_HANDLER(OP_I32_CTZ) {
    i32 op = peek_opd_i32();
    push_opd_i32(op == 0 ? 32 : __builtin_ctz(op));
}

OP_HANDLER(OP_I32_POPCNT) {
    i32 op = peek_opd_i32();
    push_opd_i32(__builtin_popcount(op));
}

OP_HANDLER(OP_I32_DIV_S) {
    s32 op2 = pop_opd_i32();
    s32 op1 = pop_opd_i32();
    push_opd_i32(op1 / op2);
}

OP_HANDLER(OP_I32_DIV_U) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    push_opd_i32(op1 / op2);
}

OP_HANDLER(OP_I32_REM_S) {
    s32 op2 = pop_opd_i32();
    s32 op1 = pop_opd_i32();
    push_opd_i32(op1 % op2);
}

OP_HANDLER(OP_I32_REM_U) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    push_opd_i32(op1 % op2);
}

OP_HANDLER(OP_I32_SHL) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    push_opd_i32(op1 << op2);
}

OP_HANDLER(OP_I32_SHR_U) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    push_opd_i32(op1 >> op2);
}

OP_HANDLER(OP_I32_SHR_S) {
    s32 op2 = pop_opd_i32();
    s32 op1 = pop_opd_i32();
    push_opd_i32(op1 >> op2);
}

OP_HANDLER(OP_I32_ROTL) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    const unsigned int mask = (32 - 1);
    op2 = op2 % 32;
    op2 &= mask;
    push_opd_i32((op1 << op2) | (op1 >> ((-op2) & mask)));
}

OP_HANDLER(OP_I32_ROTR) {
    u32 op2 = pop_opd_i32();
    u32 op1 = pop_opd_i32();
    const unsigned int mask = (32 - 1);
    op2 = op2 % 32;
    op2 &= mask;
    push_opd_i32((op1 >> op2) | (op1 << ((-op2) & mask)));
}

OP_HANDLER(OP_I64_CLZ) {
    i64 op = peek_opd_i64();
    push_opd_i64(op == 0 ? 64 : __builtin_clzl(op));
}

OP_HANDLER(OP_I64_CTZ) {
    i64 op = peek_opd_i64();
    push_opd_i64(op == 0 ? 64 : __builtin_ctzl(op));
}

OP_HANDLER(OP_I64_POPCNT) {
    i64 op = peek_opd_i64();
    push_opd_i64(__builtin_popcountl(op));
}

OP_HANDLER(OP_I64_DIV_S) {
    s64 op2 = pop_opd_i64();
    s64 op1 = pop_opd_i64();
    push_opd_i64(op1 / op2);
}

OP_HANDLER(OP_I64_DIV_U) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    push_opd_i64(op1 / op2);
}

OP_HANDLER(OP_I64_REM_S) {
    s64 op2 = pop_opd_i64();
    s64 op1 = pop_opd_i64();
    push_opd_i64(op1 % op2);
}

OP_HANDLER(OP_I64_REM_U) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    push_opd_i64(op1 % op2);
}

OP_HANDLER(OP_I64_SHL) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    push_opd_i64(op1 << op2);
}

OP_HANDLER(OP_I64_SHR_U) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    push_opd_i64(op1 >> op2);
}

OP_HANDLER(OP_I64_SHR_S) {
    s64 op2 = pop_opd_i64();
    s64 op1 = pop_opd_i64();
    push_opd_i64(op1 >> op2);
}

OP_HANDLER(OP_I64_ROTL) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    const unsigned int mask = (64 - 1);
    op2 = op2 % 64;
    op2 &= mask;
    push_opd_i64((op1 << op2) | (op1 >> ((-op2) & mask)));
}

OP_HANDLER(OP_I64_ROTR) {
    u64 op2 = pop_opd_i64();
    u64 op1 = pop_opd_i64();
    const unsigned int mask = (32 - 1);
    op2 = op2 % 64;
    op2 &= mask;
    push_opd_i64((op1 >> op2) | (op1 << ((-op2) & mask)));
}

OP_HANDLER(OP_F32_TRUNC) {
    f32 op = pop_opd_f32();
    push_opd_f32(truncf(op));
}

OP_HANDLER(OP_F32_NEAREST) {
    f32 op = pop_opd_f32();
    push_opd_f32(nearbyintf(op));
}

OP_HANDLER(OP_F32_DIV) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_f32(op1 / op2);
}

OP_HANDLER(OP_F32_COPYSIGN) {
    f32 op2 = pop_opd_f32();
    f32 op1 = pop_opd_f32();
    push_opd_f32(copysignf(op1, op2));
}

OP_HANDLER(OP_F64_TRUNC) {
    f64 op = pop_opd_f64();
    push_opd_f64(trunc(op));
}

OP_HANDLER(OP_F64_NEAREST) {
    f64 op = pop_opd_f64();
    push_opd_f64(nearbyint(op));
}

OP_HANDLER(OP_F64_DIV) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_f64(op1 / op2);
}

OP_HANDLER(OP_F64_COPYSIGN) {
    f64 op2 = pop_opd_f64();
    f64 op1 = pop_opd_f64();
    push_opd_f64(copysign(op1, op2));
}

OP_HANDLER(OP_I32_WRAP_I64) {
    i64 op = pop_opd_i64();
    push_opd_i32(op & BIT_MASK(i64, 32));
}

OP_HANDLER(OP_I32_TRUNC_F32_S) {
    f32 op = pop_opd_f32();
    push_opd_i32((s32) op);
}

OP_HANDLER(OP_I32_TRUNC_F32_U) {
    f32 op = pop_opd_f32();
    push_opd_i32((u32) op);
}

OP_HANDLER(OP_I32_TRUNC_F64_S) {
    f64 op = pop_opd_f64();
    push_opd_i32((s32) op);
}

OP_HANDLER(OP_I32_TRUNC_F64_U) {
    f64 op = pop_opd_f64();
    push_opd_i32((u32) op);
}

OP_HANDLER(OP_I64_EXTEND_I32_S) {
    u64 op = pop_opd_i32();
    if (op & 0x80000000) {
        op = op | 0xffffffff00000000;
    }
    push_opd_i64(op);
}

OP_HANDLER(OP_I64_EXTEND_I32_U) {
    i32 op = pop_opd_i32();
    push_opd_i64((u64) op);
}

OP_HANDLER(OP_I64_TRUNC_F32_S) {
    f32 op = pop_opd_f32();
    push_opd_i64((s64) op);
}

OP_HANDLER(OP_I64_TRUNC_F32_U) {
    f32 op = pop_opd_f32();
    push_opd_i64((u64) op);
}

OP_HANDLER(OP_I64_TRUNC_F64_S) {
    f64 op = pop_opd_f64();
    push_opd_i64((s64) op);
}

OP_HANDLER(OP_I64_TRUNC_F64_U) {
    f64 op = pop_opd_f64();
    push_opd_i64((u64) op);
}

OP_HANDLER(OP_F32_CONVERT_I32_S) {
    i32 op = pop_opd_i32();
    push_opd_f32((s32) op);
}

OP_HANDLER(OP_F32_CONVERT_I32_U) {
    i32 op = pop_opd_i32();
    push_opd_f32((u32) op);
}

OP_HANDLER(OP_F32_CONVERT_I64_S) {
    i64 op = pop_opd_i64();
    push_opd_f32((s64) op);
}

OP_HANDLER(OP_F32_CONVERT_I64_U) {
    i64 op = pop_opd_i64();
    push_opd_f32((u64) op);
}

OP_HANDLER(OP_F32_DEMOTE_F64) {
    f64 op = pop_opd_f64();
    push_opd_f32((f32) op);
}

OP_HANDLER(OP_F64_CONVERT_I32_S) {
    i32 op = pop_opd_i32();
    push_opd_f64((s32) op);
}

OP_HANDLER(OP_F64_CONVERT_I32_U) {
    i32 op = pop_opd_i32();
    push_opd_f64((u32) op);
}

OP_HANDLER(OP_F64_CONVERT_I64_S) {
    i64 op = pop_opd_i64();
    push_opd_f64((s64) op);
}

OP_HANDLER(OP_F64_CONVERT_I64_U) {
    i64 op = pop_opd_i64();
    push_opd_f64((u64) op);
}

OP_HANDLER(OP_F64_PROMOTE_F32) {
    f32 op = pop_opd_f32();
    push_opd_f64((f64) op);
}

OP_HANDLER(OP_I32_REINTERPRET_F32) {
    peek(&opd_stack)->valtype = VALTYPE_I32;
}

OP_HANDLER(OP_I64_REINTERPRET_F64) {
    peek(&opd_stack)->valtype = VALTYPE_I64;
}

OP_HANDLER(OP_F32_REINTERPRET_I32) {
    peek(&opd_stack)->valtype = VALTYPE_F32;
}

OP_HANDLER(OP_F64_REINTERPRET_I64) {
    peek(&opd_stack)->valtype = VALTYPE_I64;
}

static numeric_func OP_HANDLERS[255] = {
        ADD_OP_HANDLER(OP_I32_CONST),
        ADD_OP_HANDLER(OP_I64_CONST),
        ADD_OP_HANDLER(OP_F32_CONST),
        ADD_OP_HANDLER(OP_F64_CONST),

        ADD_OP_HANDLER(OP_I32_EQZ),
        ADD_OP_HANDLER(OP_I32_EQ),
        ADD_OP_HANDLER(OP_I32_NE),
        ADD_OP_HANDLER(OP_I32_LT_S),
        ADD_OP_HANDLER(OP_I32_LT_U),
        ADD_OP_HANDLER(OP_I32_GT_S),
        ADD_OP_HANDLER(OP_I32_GT_U),
        ADD_OP_HANDLER(OP_I32_LE_S),
        ADD_OP_HANDLER(OP_I32_LE_U),
        ADD_OP_HANDLER(OP_I32_GE_S),
        ADD_OP_HANDLER(OP_I32_GE_U),

        ADD_OP_HANDLER(OP_I64_EQZ),
        ADD_OP_HANDLER(OP_I64_EQ),
        ADD_OP_HANDLER(OP_I64_NE),
        ADD_OP_HANDLER(OP_I64_LT_S),
        ADD_OP_HANDLER(OP_I64_LT_U),
        ADD_OP_HANDLER(OP_I64_GT_S),
        ADD_OP_HANDLER(OP_I64_GT_U),
        ADD_OP_HANDLER(OP_I64_LE_S),
        ADD_OP_HANDLER(OP_I64_LE_U),
        ADD_OP_HANDLER(OP_I64_GE_S),
        ADD_OP_HANDLER(OP_I64_GE_U),

        ADD_OP_HANDLER(OP_F32_EQ),
        ADD_OP_HANDLER(OP_F32_NE),
        ADD_OP_HANDLER(OP_F32_LT),
        ADD_OP_HANDLER(OP_F32_GT),
        ADD_OP_HANDLER(OP_F32_LE),
        ADD_OP_HANDLER(OP_F32_GE),

        ADD_OP_HANDLER(OP_F64_EQ),
        ADD_OP_HANDLER(OP_F64_NE),
        ADD_OP_HANDLER(OP_F64_LT),
        ADD_OP_HANDLER(OP_F64_GT),
        ADD_OP_HANDLER(OP_F64_LE),
        ADD_OP_HANDLER(OP_F64_GE),

        ADD_OP_HANDLER(OP_I32_CLZ),
        ADD_OP_HANDLER(OP_I32_CTZ),
        ADD_OP_HANDLER(OP_I32_POPCNT),
        ADD_OP_HANDLER(OP_I32_ADD),
        ADD_OP_HANDLER(OP_I32_SUB),
        ADD_OP_HANDLER(OP_I32_MUL),
        ADD_OP_HANDLER(OP_I32_DIV_S),
        ADD_OP_HANDLER(OP_I32_DIV_U),
        ADD_OP_HANDLER(OP_I32_REM_S),
        ADD_OP_HANDLER(OP_I32_REM_U),
        ADD_OP_HANDLER(OP_I32_AND),
        ADD_OP_HANDLER(OP_I32_OR),
        ADD_OP_HANDLER(OP_I32_XOR),
        ADD_OP_HANDLER(OP_I32_SHL),
        ADD_OP_HANDLER(OP_I32_SHR_U),
        ADD_OP_HANDLER(OP_I32_SHR_S),
        ADD_OP_HANDLER(OP_I32_ROTL),
        ADD_OP_HANDLER(OP_I32_ROTR),

        ADD_OP_HANDLER(OP_I64_CLZ),
        ADD_OP_HANDLER(OP_I64_CTZ),
        ADD_OP_HANDLER(OP_I64_POPCNT),
        ADD_OP_HANDLER(OP_I64_ADD),
        ADD_OP_HANDLER(OP_I64_SUB),
        ADD_OP_HANDLER(OP_I64_MUL),
        ADD_OP_HANDLER(OP_I64_DIV_S),
        ADD_OP_HANDLER(OP_I64_DIV_U),
        ADD_OP_HANDLER(OP_I64_REM_S),
        ADD_OP_HANDLER(OP_I64_REM_U),
        ADD_OP_HANDLER(OP_I64_AND),
        ADD_OP_HANDLER(OP_I64_OR),
        ADD_OP_HANDLER(OP_I64_XOR),
        ADD_OP_HANDLER(OP_I64_SHL),
        ADD_OP_HANDLER(OP_I64_SHR_U),
        ADD_OP_HANDLER(OP_I64_SHR_S),
        ADD_OP_HANDLER(OP_I64_ROTL),
        ADD_OP_HANDLER(OP_I64_ROTR),

        ADD_OP_HANDLER(OP_F32_ABS),
        ADD_OP_HANDLER(OP_F32_NEG),
        ADD_OP_HANDLER(OP_F32_CEIL),
        ADD_OP_HANDLER(OP_F32_FLOOR),
        ADD_OP_HANDLER(OP_F32_TRUNC),
        ADD_OP_HANDLER(OP_F32_NEAREST),
        ADD_OP_HANDLER(OP_F32_SQRT),
        ADD_OP_HANDLER(OP_F32_ADD),
        ADD_OP_HANDLER(OP_F32_SUB),
        ADD_OP_HANDLER(OP_F32_MUL),
        ADD_OP_HANDLER(OP_F32_DIV),
        ADD_OP_HANDLER(OP_F32_MIN),
        ADD_OP_HANDLER(OP_F32_MAX),
        ADD_OP_HANDLER(OP_F32_COPYSIGN),

        ADD_OP_HANDLER(OP_F64_ABS),
        ADD_OP_HANDLER(OP_F64_NEG),
        ADD_OP_HANDLER(OP_F64_CEIL),
        ADD_OP_HANDLER(OP_F64_FLOOR),
        ADD_OP_HANDLER(OP_F64_TRUNC),
        ADD_OP_HANDLER(OP_F64_NEAREST),
        ADD_OP_HANDLER(OP_F64_SQRT),
        ADD_OP_HANDLER(OP_F64_ADD),
        ADD_OP_HANDLER(OP_F64_SUB),
        ADD_OP_HANDLER(OP_F64_MUL),
        ADD_OP_HANDLER(OP_F64_DIV),
        ADD_OP_HANDLER(OP_F64_MIN),
        ADD_OP_HANDLER(OP_F64_MAX),
        ADD_OP_HANDLER(OP_F64_COPYSIGN),

        ADD_OP_HANDLER(OP_I32_WRAP_I64),
        ADD_OP_HANDLER(OP_I32_TRUNC_F32_S),
        ADD_OP_HANDLER(OP_I32_TRUNC_F32_U),
        ADD_OP_HANDLER(OP_I32_TRUNC_F64_S),
        ADD_OP_HANDLER(OP_I32_TRUNC_F64_U),
        ADD_OP_HANDLER(OP_I64_EXTEND_I32_S),
        ADD_OP_HANDLER(OP_I64_EXTEND_I32_U),
        ADD_OP_HANDLER(OP_I64_TRUNC_F32_S),
        ADD_OP_HANDLER(OP_I64_TRUNC_F32_U),
        ADD_OP_HANDLER(OP_I64_TRUNC_F64_S),
        ADD_OP_HANDLER(OP_I64_TRUNC_F64_U),
        ADD_OP_HANDLER(OP_F32_CONVERT_I32_S),
        ADD_OP_HANDLER(OP_F32_CONVERT_I32_U),
        ADD_OP_HANDLER(OP_F32_CONVERT_I64_S),
        ADD_OP_HANDLER(OP_F32_CONVERT_I64_U),
        ADD_OP_HANDLER(OP_F32_DEMOTE_F64),
        ADD_OP_HANDLER(OP_F64_CONVERT_I32_S),
        ADD_OP_HANDLER(OP_F64_CONVERT_I32_U),
        ADD_OP_HANDLER(OP_F64_CONVERT_I64_S),
        ADD_OP_HANDLER(OP_F64_CONVERT_I64_U),
        ADD_OP_HANDLER(OP_F64_PROMOTE_F32),
        ADD_OP_HANDLER(OP_I32_REINTERPRET_F32),
        ADD_OP_HANDLER(OP_I64_REINTERPRET_F64),
        ADD_OP_HANDLER(OP_F32_REINTERPRET_I32),
        ADD_OP_HANDLER(OP_F64_REINTERPRET_I64),
};


bool is_numeric_instr(const opcode_t *opcode) {
    return OP_HANDLERS[*opcode] != NULL;
}

void eval_numeric_instr(instruction_t *instr) {
    OP_HANDLERS[instr->opcode](instr);
}
