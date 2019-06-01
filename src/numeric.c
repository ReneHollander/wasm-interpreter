/*
 * Contains everything related to
 * numeric instructions
 */

#include "numeric.h"
#include "type.h"
#include "instruction.h"


static bool is_add(opcode_t opcode);

static bool is_sub(opcode_t opcode);

static bool is_mul(opcode_t opcode);

static bool is_div(opcode_t opcode);

static bool is_const(opcode_t opcode);


bool is_numeric_instr(opcode_t opcode) {
    return is_add(opcode) || is_sub(opcode) || is_mul(opcode) || is_div(opcode) || is_const(opcode);
}

static bool is_const(opcode_t opcode) {
    switch (opcode) {
        case OP_I32_CONST:
        case OP_I64_CONST:
        case OP_F32_CONST:
        case OP_F64_CONST:
            return true;
        default:
            return false;
    }
}

static bool is_add(opcode_t opcode) {
    switch (opcode) {
        case OP_I32_ADD:
        case OP_F32_ADD:
        case OP_I64_ADD:
        case OP_F64_ADD:
            return true;
        default:
            return false;
    }
}

static bool is_sub(opcode_t opcode) {
    switch (opcode) {
        case OP_I32_SUB:
        case OP_F32_SUB:
        case OP_I64_SUB:
        case OP_F64_SUB:
            return true;
        default:
            return false;
    }
}

static bool is_mul(opcode_t opcode) {
    switch (opcode) {
        case OP_I32_MUL:
        case OP_F32_MUL:
        case OP_I64_MUL:
        case OP_F64_MUL:
            return true;
        default:
            return false;
    }
}

static bool is_div(opcode_t opcode) {
    switch (opcode) {
        case OP_I32_DIV_S:
        case OP_I32_DIV_U:
        case OP_F32_DIV:
        case OP_I64_DIV_S:
        case OP_I64_DIV_U:
        case OP_F64_DIV:
            return true;
        default:
            return false;
    }
}

/* Actual numeric functions */

i32 add_i32(i32 op1, i32 op2) {
    return op1 + op2;
}

i64 add_i64(i64 op1, i64 op2) {
    return op1 + op2;
}

f32 add_f32(f32 op1, f32 op2) {
    return op1 + op2;
}

f64 add_f64(f64 op1, f64 op2) {
    return op1 + op2;
}

i32 sub_i32(i32 op1, i32 op2) {
    return op1 - op2;
}

i64 sub_i64(i64 op1, i64 op2) {
    return op1 - op2;
}

f32 sub_f32(f32 op1, f32 op2) {
    return op1 - op2;
}

f64 sub_f64(f64 op1, f64 op2) {
    return op1 - op2;
}

i32 mul_i32(i32 op1, i32 op2) {
    return op1 * op2;
}

i64 mul_i64(i64 op1, i64 op2) {
    return op1 * op2;
}

f32 mul_f32(f32 op1, f32 op2) {
    return op1 * op2;
}

f64 mul_f64(f64 op1, f64 op2) {
    return op1 * op2;
}
