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

static void eval_rel(instruction_t instr);

static bool is_add(opcode_t opcode);

static bool is_sub(opcode_t opcode);

static bool is_mul(opcode_t opcode);

static bool is_div(opcode_t opcode);

static bool is_const(opcode_t opcode);

static bool is_rel(opcode_t opcode);

static i32 add_i32(i32 op1, i32 op2);

static i64 add_i64(i64 op1, i64 op2);

static f32 add_f32(f32 op1, f32 op2);

static f64 add_f64(f64 op1, f64 op2);

static i32 sub_i32(i32 op1, i32 op2);

static i64 sub_i64(i64 op1, i64 op2);

static f32 sub_f32(f32 op1, f32 op2);

static f64 sub_f64(f64 op1, f64 op2);

static i32 mul_i32(i32 op1, i32 op2);

static i64 mul_i64(i64 op1, i64 op2);

static f32 mul_f32(f32 op1, f32 op2);

static f64 mul_f64(f64 op1, f64 op2);

static f32 min_f32(f32 op1, f32 op2);

static f64 min_f64(f64 op1, f64 op2);

static f32 max_f32(f32 op1, f32 op2);

static f64 max_f64(f64 op1, f64 op2);

static f32 sqrt_f32(f32 op);

static f64 sqrt_f64(f64 op);

static f32 abs_f32(f32 op);

static f64 abs_f64(f64 op);

static f32 neg_f32(f32 op);

static f64 neg_f64(f64 op);

static f32 ceil_f32(f32 op);

static f64 ceil_f64(f64 op);

static f32 floor_f32(f32 op);

static f64 floor_f64(f64 op);

static i32 xor_i32(i32 op1, i32 op2);

static i64 xor_i64(i64 op1, i64 op2);

static i32 or_i32(i32 op1, i32 op2);

static i64 or_i64(i64 op1, i64 op2);

static i32 and_i32(i32 op1, i32 op2);

static i64 and_i64(i64 op1, i64 op2);

i32 eqz_i32(i32 op);

i32 eq_i32(i32 op1, i32 op2);

i32 ne_i32(i32 op1, i32 op2);

i32 lt_i32_s(i32 op1, i32 op2);

i32 lt_i32_u(u32 op1, u32 op2);

i32 gt_i32_s(i32 op1, i32 op2);

i32 gt_i32_u(u32 op1, u32 op2);

i32 le_i32_s(i32 op1, i32 op2);

i32 le_i32_u(u32 op1, u32 op2);

i32 ge_i32_s(i32 op1, i32 op2);

i32 ge_i32_u(u32 op1, u32 op2);

i32 eqz_i64(i64 op);

i32 eq_i64(i64 op1, i64 op2);

i32 ne_i64(i64 op1, i64 op2);

i32 lt_i64_s(i64 op1, i64 op2);

i32 lt_i64_u(i64 op1, i64 op2);

i32 gt_i64_s(i64 op1, i64 op2);

i32 gt_i64_u(i64 op1, i64 op2);

i32 le_i64_s(i64 op1, i64 op2);

i32 le_i64_u(i64 op1, i64 op2);

i32 ge_i64_s(i64 op1, i64 op2);

i32 ge_i64_u(i64 op1, i64 op2);

i32 eq_f32(f32 op1, f32 op2);

i32 ne_f32(f32 op1, f32 op2);

i32 lt_f32(f32 op1, f32 op2);

i32 gt_f32(f32 op1, f32 op2);

i32 le_f32(f32 op1, f32 op2);

i32 ge_f32(f32 op1, f32 op2);

i32 eq_f64(i64 op1, f64 op2);

i32 ne_f64(f64 op1, f64 op2);

i32 lt_f64(f64 op1, f64 op2);

i32 gt_f64(f64 op1, f64 op2);

i32 le_f64(f64 op1, f64 op2);

i32 ge_f64(f64 op1, f64 op2);

void eval_numeric_instr(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    if (opcode == OP_I32_CONST) {
        push_opd_i32(instr.const_i32);
    } else if (opcode == OP_I64_CONST) {
        push_opd_i64(instr.const_i64);
    } else if (opcode == OP_F32_CONST) {
        push_opd_f32(instr.const_f32);
    } else if (opcode == OP_F64_CONST) {
        push_opd_f64(instr.const_f64);
    } else if (opcode == OP_F32_ADD) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_f32(add_f32(op1, op2));
    } else if (opcode == OP_F64_ADD) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_f64(add_f64(op1, op2));
    } else if (opcode == OP_I32_ADD) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(add_i32(op1, op2));
    } else if (opcode == OP_I64_ADD) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(add_i64(op1, op2));
    } else if (opcode == OP_F32_SUB) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_f32(sub_f32(op1, op2));
    } else if (opcode == OP_F64_SUB) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_f64(sub_f64(op1, op2));
    } else if (opcode == OP_I32_SUB) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(sub_i32(op1, op2));
    } else if (opcode == OP_I64_SUB) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(sub_i64(op1, op2));
    } else if (opcode == OP_F32_MUL) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_f32(mul_f32(op1, op2));
    } else if (opcode == OP_F64_MUL) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_f64(mul_f64(op1, op2));
    } else if (opcode == OP_I32_MUL) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(mul_i32(op1, op2));
    } else if (opcode == OP_I64_MUL) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(mul_i64(op1, op2));
    } else if (opcode == OP_F32_MIN) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_f32(min_f32(op1, op2));
    } else if (opcode == OP_F64_MIN) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_f64(min_f64(op1, op2));
    } else if (opcode == OP_F32_MAX) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_f32(max_f32(op1, op2));
    } else if (opcode == OP_F64_MAX) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_f64(max_f64(op1, op2));
    } else if (opcode == OP_F32_SQRT) {
        f32 op = pop_opd_f32();
        push_opd_f32(sqrt_f32(op));
    } else if (opcode == OP_F64_SQRT) {
        f64 op = pop_opd_f64();
        push_opd_f64(sqrt_f64(op));
    } else if (opcode == OP_F32_FLOOR) {
        f32 op = pop_opd_f32();
        push_opd_f32(floor_f32(op));
    } else if (opcode == OP_F64_FLOOR) {
        f64 op = pop_opd_f64();
        push_opd_f64(floor_f64(op));
    } else if (opcode == OP_F32_CEIL) {
        f32 op = pop_opd_f32();
        push_opd_f32(ceil_f32(op));
    } else if (opcode == OP_F64_CEIL) {
        f64 op = pop_opd_f64();
        push_opd_f64(ceil_f64(op));
    } else if (opcode == OP_F32_ABS) {
        f32 op = pop_opd_f32();
        push_opd_f32(abs_f32(op));
    } else if (opcode == OP_F64_ABS) {
        f64 op = pop_opd_f64();
        push_opd_f64(abs_f64(op));
    } else if (opcode == OP_F32_NEG) {
        f32 op = pop_opd_f32();
        push_opd_f32(neg_f32(op));
    } else if (opcode == OP_F64_NEG) {
        f64 op = pop_opd_f64();
        push_opd_f64(neg_f64(op));
    } else if (opcode == OP_I32_OR) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(or_i32(op1, op2));
    } else if (opcode == OP_I64_OR) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(or_i64(op1, op2));
    } else if (opcode == OP_I32_XOR) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(xor_i32(op1, op2));
    } else if (opcode == OP_I64_XOR) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(xor_i64(op1, op2));
    } else if (opcode == OP_I32_AND) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(and_i32(op1, op2));
    } else if (opcode == OP_I64_AND) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(and_i64(op1, op2));
    } else if (is_rel(opcode)) {
        eval_rel(instr);
    } else {
        fprintf(stderr, "numeric instruction with opcode %X currently not supported\n", opcode);
        interpreter_exit();
    }
}

static void eval_rel(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    if (opcode == OP_I32_EQZ) {
        i32 op = pop_opd_i32();
        push_opd_i32(eqz_i32(op));
    } else if (opcode == OP_I32_EQ) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(eq_i32(op1, op2));
    } else if (opcode == OP_I32_NE) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(ne_i32(op1, op2));
    } else if (opcode == OP_I32_LT_S) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(lt_i32_s(op1, op2));
    } else if (opcode == OP_I32_LT_U) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(lt_i32_u(op1, op2));
    } else if (opcode == OP_I32_GE_S) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(ge_i32_s(op1, op2));
    } else if (opcode == OP_I32_GE_U) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(ge_i32_u(op1, op2));
    } else if (opcode == OP_I32_GT_S) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(gt_i32_s(op1, op2));
    } else if (opcode == OP_I32_GT_U) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(gt_i32_u(op1, op2));
    } else if (opcode == OP_I32_LE_S) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(le_i32_s(op1, op2));
    } else if (opcode == OP_I32_LE_U) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(le_i32_u(op1, op2));
    } else if (opcode == OP_I64_EQZ) {
        i64 op = pop_opd_i64();
        push_opd_i32(eqz_i64(op));
    } else if (opcode == OP_I64_EQ) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(eq_i64(op1, op2));
    } else if (opcode == OP_I64_NE) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(ne_i64(op1, op2));
    } else if (opcode == OP_I64_LT_S) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(lt_i64_s(op1, op2));
    } else if (opcode == OP_I64_LT_U) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(lt_i64_u(op1, op2));
    } else if (opcode == OP_I64_GE_S) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(ge_i64_s(op1, op2));
    } else if (opcode == OP_I64_GE_U) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(ge_i64_u(op1, op2));
    } else if (opcode == OP_I64_GT_S) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(gt_i64_s(op1, op2));
    } else if (opcode == OP_I64_GT_U) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(gt_i64_u(op1, op2));
    } else if (opcode == OP_I64_LE_S) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(le_i64_s(op1, op2));
    } else if (opcode == OP_I64_LE_U) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i32(le_i64_u(op1, op2));
    } else if (opcode == OP_F32_EQ) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_i32(eq_f32(op1, op2));
    } else if (opcode == OP_F32_NE) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_i32(ne_f32(op1, op2));
    } else if (opcode == OP_F32_LT) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_i32(lt_f32(op1, op2));
    } else if (opcode == OP_F32_GE) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_i32(ge_f32(op1, op2));
    } else if (opcode == OP_F32_GT) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_i32(gt_f32(op1, op2));
    } else if (opcode == OP_F32_LE) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_i32(le_f32(op1, op2));
    } else if (opcode == OP_F64_EQ) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_i32(eq_f64(op1, op2));
    } else if (opcode == OP_F64_NE) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_i32(ne_f64(op1, op2));
    } else if (opcode == OP_F64_LT) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_i32(lt_f64(op1, op2));
    } else if (opcode == OP_F64_GE) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_i32(ge_f64(op1, op2));
    } else if (opcode == OP_F64_GT) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_i32(gt_f64(op1, op2));
    } else if (opcode == OP_F64_LE) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_i32(le_f64(op1, op2));
    } else {
        interpreter_error("unknown rel opcode\n");
    }
}

bool is_numeric_instr(opcode_t opcode) {
    return is_add(opcode) || is_sub(opcode) || is_mul(opcode) ||
           is_div(opcode) || is_const(opcode) || is_rel(opcode);
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

static bool is_rel(opcode_t opcode) {
    switch (opcode) {
        case OP_I32_LT_S:
        case OP_I32_LT_U:
        case OP_I32_GE_S:
        case OP_I32_GE_U:
        case OP_I32_GT_S:
        case OP_I32_GT_U:
        case OP_I32_LE_S:
        case OP_I32_LE_U:
        case OP_I32_EQ:
        case OP_I32_NE:
        case OP_I64_LT_S:
        case OP_I64_LT_U:
        case OP_I64_GE_S:
        case OP_I64_GE_U:
        case OP_I64_GT_S:
        case OP_I64_GT_U:
        case OP_I64_LE_S:
        case OP_I64_LE_U:
        case OP_I64_EQ:
        case OP_I64_NE:
        case OP_F32_LT:
        case OP_F32_GE:
        case OP_F32_GT:
        case OP_F32_LE:
        case OP_F32_EQ:
        case OP_F32_NE:
        case OP_F64_LT:
        case OP_F64_GE:
        case OP_F64_GT:
        case OP_F64_LE:
        case OP_F64_EQ:
        case OP_F64_NE:
        case OP_I32_EQZ:
        case OP_I64_EQZ:
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

f32 min_f32(f32 op1, f32 op2) {
    return op1 <= op2 ? op1 : op2;
}

f64 min_f64(f64 op1, f64 op2) {
    return op1 <= op2 ? op1 : op2;
}

f32 max_f32(f32 op1, f32 op2) {
    return op1 >= op2 ? op1 : op2;
}

f64 max_f64(f64 op1, f64 op2) {
    return op1 >= op2 ? op1 : op2;
}

f32 sqrt_f32(f32 op) {
    return sqrtf(op);
}

f64 sqrt_f64(f64 op) {
    return sqrt(op);
}

f32 abs_f32(f32 op) {
    return fabsf(op);
}

f64 abs_f64(f64 op) {
    return fabs(op);
}

f32 neg_f32(f32 op) {
    return -op;
}

f64 neg_f64(f64 op) {
    return -op;
}

f32 ceil_f32(f32 op) {
    return ceilf(op);
}

f64 ceil_f64(f64 op) {
    return ceil(op);
}

f32 floor_f32(f32 op) {
    return floorf(op);
}

f64 floor_f64(f64 op) {
    return floor(op);
}

i32 xor_i32(i32 op1, i32 op2) {
    return op1 ^ op2;
}

i64 xor_i64(i64 op1, i64 op2) {
    return op1 ^ op2;
}

i32 or_i32(i32 op1, i32 op2) {
    return op1 | op2;
}

i64 or_i64(i64 op1, i64 op2) {
    return op1 | op2;
}

i32 and_i32(i32 op1, i32 op2) {
    return op1 & op2;
}

i64 and_i64(i64 op1, i64 op2) {
    return op1 & op2;
}

i32 eqz_i32(i32 op) {
    return op == 0 ? 1 : 0;
}

i32 eq_i32(i32 op1, i32 op2) {
    return op1 == op2 ? 1 : 0;
}

i32 ne_i32(i32 op1, i32 op2) {
    return op1 != op2 ? 1 : 0;
}

i32 lt_i32_s(i32 op1, i32 op2) {
    return op1 < op2 ? 1 : 0;
}

i32 lt_i32_u(u32 op1, u32 op2) {
    return op1 < op2 ? 1 : 0;
}

i32 gt_i32_s(i32 op1, i32 op2) {
    return op1 > op2 ? 1 : 0;
}

i32 gt_i32_u(u32 op1, u32 op2) {
    return op1 > op2 ? 1 : 0;
}

i32 le_i32_s(i32 op1, i32 op2) {
    return op1 <= op2 ? 1 : 0;
}

i32 le_i32_u(u32 op1, u32 op2) {
    return op1 <= op2 ? 1 : 0;
}

i32 ge_i32_s(i32 op1, i32 op2) {
    return op1 >= op2 ? 1 : 0;
}

i32 ge_i32_u(u32 op1, u32 op2) {
    return op1 >= op2 ? 1 : 0;
}

i32 eqz_i64(i64 op) {
    return op == 0 ? 1 : 0;
}

i32 eq_i64(i64 op1, i64 op2) {
    return op1 == op2 ? 1 : 0;
}

i32 ne_i64(i64 op1, i64 op2) {
    return op1 != op2 ? 1 : 0;
}

i32 lt_i64_s(i64 op1, i64 op2) {
    return op1 < op2 ? 1 : 0;
}

i32 lt_i64_u(i64 op1, i64 op2) {
    return op1 < op2 ? 1 : 0;
}

i32 gt_i64_s(i64 op1, i64 op2) {
    return op1 > op2 ? 1 : 0;
}

i32 gt_i64_u(i64 op1, i64 op2) {
    return op1 > op2 ? 1 : 0;
}

i32 le_i64_s(i64 op1, i64 op2) {
    return op1 <= op2 ? 1 : 0;
}

i32 le_i64_u(i64 op1, i64 op2) {
    return op1 <= op2 ? 1 : 0;
}

i32 ge_i64_s(i64 op1, i64 op2) {
    return op1 >= op2 ? 1 : 0;
}

i32 ge_i64_u(i64 op1, i64 op2) {
    return op1 >= op2 ? 1 : 0;
}

i32 eq_f32(f32 op1, f32 op2) {
    return op1 == op2 ? 1 : 0;
}

i32 ne_f32(f32 op1, f32 op2) {
    return op1 != op2 ? 1 : 0;
}

i32 lt_f32(f32 op1, f32 op2) {
    return op1 < op2 ? 1 : 0;
}

i32 gt_f32(f32 op1, f32 op2) {
    return op1 > op2 ? 1 : 0;
}

i32 le_f32(f32 op1, f32 op2) {
    return op1 <= op2 ? 1 : 0;
}

i32 ge_f32(f32 op1, f32 op2) {
    return op1 >= op2 ? 1 : 0;
}

i32 eq_f64(i64 op1, f64 op2) {
    return op1 == op2 ? 1 : 0;
}

i32 ne_f64(f64 op1, f64 op2) {
    return op1 != op2 ? 1 : 0;
}

i32 lt_f64(f64 op1, f64 op2) {
    return op1 < op2 ? 1 : 0;
}

i32 gt_f64(f64 op1, f64 op2) {
    return op1 > op2 ? 1 : 0;
}

i32 le_f64(f64 op1, f64 op2) {
    return op1 <= op2 ? 1 : 0;
}

i32 ge_f64(f64 op1, f64 op2) {
    return op1 >= op2 ? 1 : 0;
}