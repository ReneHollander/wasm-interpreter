#ifndef WASM_INTERPRETER_VARIABLE_H
#define WASM_INTERPRETER_VARIABLE_H

#include "list.h"
#include "eval_types.h"
#include "instruction.h"
#include "module.h"

bool is_variable_instr(const opcode_t *opcode);

void eval_variable_instr(eval_state_t *eval_state, instruction_t *instr);

void init_globals(eval_state_t *eval_state, vec_global_t *_globals);

/* **************** */

#endif //WASM_INTERPRETER_VARIABLE_H
