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

#endif //WASM_INTERPRETER_NUMERIC_H
