#ifndef WASM_INTERPRETER_VARIABLE_H
#define WASM_INTERPRETER_VARIABLE_H

#include "eval_types.h"
#include "instruction.h"
#include "module.h"

void init_globals(eval_state_t *eval_state, vec_global_t *_globals);

#endif //WASM_INTERPRETER_VARIABLE_H
