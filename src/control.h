#ifndef WASM_INTERPRETER_CONTROL_H
#define WASM_INTERPRETER_CONTROL_H

#include "eval_types.h"
#include "type.h"
#include "module.h"

void clean_to_func_marker(eval_state_t *eval_state);

void clean_to_label(eval_state_t *eval_state);

void eval_call(eval_state_t *eval_state, func_t *func);

#endif //WASM_INTERPRETER_CONTROL_H
