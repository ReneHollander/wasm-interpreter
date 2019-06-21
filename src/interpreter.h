#ifndef WASM_INTERPRETER_INTERPRETER_H
#define WASM_INTERPRETER_INTERPRETER_H

#include "module.h"
#include "stack.h"
#include "variable.h"

void interpret(module_t *module);

return_value_t interpret_function(module_t *module, char *func_name, node_t *args);

void interpreter_error(eval_state_t *eval_state, char *err_message);

void interpreter_exit(eval_state_t *eval_state);

void eval_instrs(eval_state_t *eval_state);

void eval_instr(eval_state_t *eval_state, instruction_t *instr);

#endif //WASM_INTERPRETER_INTERPRETER_H
