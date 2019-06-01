#ifndef WASM_INTERPRETER_NUMERIC_H
#define WASM_INTERPRETER_NUMERIC_H

#include "type.h"
#include "instruction.h"

bool is_numeric_instr(opcode_t opcode);


i32 add_i32(i32 op1, i32 op2);

i64 add_i64(i64 op1, i64 op2);

f32 add_f32(f32 op1, f32 op2);

f64 add_f64(f64 op1, f64 op2);

i32 sub_i32(i32 op1, i32 op2);

i64 sub_i64(i64 op1, i64 op2);

f32 sub_f32(f32 op1, f32 op2);

f64 sub_f64(f64 op1, f64 op2);

i32 mul_i32(i32 op1, i32 op2);

i64 mul_i64(i64 op1, i64 op2);

f32 mul_f32(f32 op1, f32 op2);

f64 mul_f64(f64 op1, f64 op2);

f32 min_f32(f32 op1, f32 op2);

f64 min_f64(f64 op1, f64 op2);

f32 max_f32(f32 op1, f32 op2);

f64 max_f64(f64 op1, f64 op2);

f32 sqrt_f32(f32 op);

f64 sqrt_f64(f64 op);

f32 abs_f32(f32 op);

f64 abs_f64(f64 op);

f32 neg_f32(f32 op);

f64 neg_f64(f64 op);

f32 ceil_f32(f32 op);

f64 ceil_f64(f64 op);

f32 floor_f32(f32 op);

f64 floor_f64(f64 op);

i32 xor_i32(i32 op1, i32 op2);

i64 xor_i64(i64 op1, i64 op2);

i32 or_i32(i32 op1, i32 op2);

i64 or_i64(i64 op1, i64 op2);

i32 and_i32(i32 op1, i32 op2);

i64 and_i64(i64 op1, i64 op2);

#endif //WASM_INTERPRETER_NUMERIC_H
