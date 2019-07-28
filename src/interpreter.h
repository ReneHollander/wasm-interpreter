#ifndef WASM_INTERPRETER_INTERPRETER_H
#define WASM_INTERPRETER_INTERPRETER_H

#include "module.h"
#include "stack.h"
#include "variable.h"

eval_state_t *create_interpreter();

void init_interpreter(eval_state_t *eval_state);

void free_interpreter(eval_state_t *eval_state);

return_value_t interpret_function(eval_state_t *eval_state, char *func_name, node_t *args);

void interpreter_error(eval_state_t *eval_state, char *err_message);

void interpreter_exit(eval_state_t *eval_state);

void eval_instrs(eval_state_t *eval_state);

void eval_instr(eval_state_t *eval_state, instruction_t *instr);

#endif //WASM_INTERPRETER_INTERPRETER_H
