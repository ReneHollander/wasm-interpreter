#ifndef WASM_INTERPRETER_OPD_STACK_H
#define WASM_INTERPRETER_OPD_STACK_H

#include "instruction.h"
#include "stack.h"

i32 pop_opd_i32(void);

i64 pop_opd_i64(void);

f32 pop_opd_f32(void);

f64 pop_opd_f64(void);

void push_opd_i32(i32 val);

void push_opd_i64(i64 val);

void push_opd_f32(f32 val);

void push_opd_f64(f64 val);

i32 peek_opd_i32();

i64 peek_opd_i64();

f32 peek_opd_f32();

f64 peek_opd_f64();

stack opd_stack;

#endif //WASM_INTERPRETER_OPD_STACK_H
