#ifndef WASM_INTERPRETER_INTERPRETER_H
#define WASM_INTERPRETER_INTERPRETER_H

#include "module.h"
#include "variable.h"
#include "exception.h"

eval_state_t *create_interpreter();

void init_interpreter(eval_state_t *eval_state);

void free_interpreter(eval_state_t *eval_state);

exception_t
interpret_function(eval_state_t *eval_state, char *func_name, vec_parameter_value_t *parameters, return_value_t *ret);

void interpreter_error(eval_state_t *eval_state, char *err_message);

void interpreter_exit(eval_state_t *eval_state) __attribute__ ((__noreturn__));

void eval_instrs(eval_state_t *eval_state);

void eval_instr(eval_state_t *eval_state, instruction_t *instr);

#endif //WASM_INTERPRETER_INTERPRETER_H
