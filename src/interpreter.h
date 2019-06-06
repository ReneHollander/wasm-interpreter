#ifndef WASM_INTERPRETER_INTERPRETER_H
#define WASM_INTERPRETER_INTERPRETER_H

#include "module.h"
#include "stack.h"

void interpret(module_t *module);

void interpret_function(module_t *module, char *func_name);

void interpreter_error(char *err_message);

void interpreter_exit(void);

void eval_instrs(void);

void eval_instr(instruction_t instr);

module_t *module_global;

#endif //WASM_INTERPRETER_INTERPRETER_H
