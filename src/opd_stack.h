#ifndef WASM_INTERPRETER_OPD_STACK_H
#define WASM_INTERPRETER_OPD_STACK_H

#include "instruction.h"
#include "stack.h"
#include "variable.h"

void pop_unknown(eval_state_t *eval_state, valtype_t *result_type, val_t *val);

void pop_generic(eval_state_t *eval_state, valtype_t result_type, val_t *val);

void push_generic(eval_state_t *eval_state, valtype_t result_type, val_t *val);

i32 pop_opd_i32(eval_state_t *eval_state);

i64 pop_opd_i64(eval_state_t *eval_state);

f32 pop_opd_f32(eval_state_t *eval_state);

f64 pop_opd_f64(eval_state_t *eval_state);

void push_opd_i32(eval_state_t *eval_state, i32 val);

void push_opd_i64(eval_state_t *eval_state, i64 val);

void push_opd_f32(eval_state_t *eval_state, f32 val);

void push_opd_f64(eval_state_t *eval_state, f64 val);

i32 peek_opd_i32(eval_state_t *eval_state);

i64 peek_opd_i64(eval_state_t *eval_state);

f32 peek_opd_f32(eval_state_t *eval_state);

f64 peek_opd_f64(eval_state_t *eval_state);

#endif //WASM_INTERPRETER_OPD_STACK_H
