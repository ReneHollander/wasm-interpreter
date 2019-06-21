#ifndef WASM_INTERPRETER_VARIABLE_H
#define WASM_INTERPRETER_VARIABLE_H

#include "list.h"
#include "eval_types.h"
#include "instruction.h"
#include "module.h"

bool is_variable_instr(const opcode_t *opcode);

void eval_variable_instr(eval_state_t *eval_state, instruction_t *instr);

void init_globals(eval_state_t *eval_state, vec_global_t *_globals);

void init_locals(eval_state_t *eval_state, vec_locals_t *locals);

void init_params(eval_state_t *eval_state, vec_valtype_t *params);

/* frame operations */
void push_frame(eval_state_t *eval_state, vec_instruction_t *instrs,
                uint32_t arity, valtype_t result_type, context_t context);

void pop_frame(eval_state_t *eval_state);

frame_t *peek_frame(eval_state_t *eval_state);

frame_t *peek_func_frame(eval_state_t *eval_state);

/* **************** */

#endif //WASM_INTERPRETER_VARIABLE_H
