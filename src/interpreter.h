#ifndef WASM_INTERPRETER_INTERPRETER_H
#define WASM_INTERPRETER_INTERPRETER_H

#include "module.h"
#include "stack.h"
#include "variable.h"

typedef struct parameter_value {
    val_t val;
    valtype_t type;
} parameter_value_t;

typedef struct return_value {
    val_t val;
    valtype_t type;
} return_value_t;

void interpret(module_t *module);

return_value_t interpret_function(module_t *module, char *func_name, node_t *args);

void interpreter_error(char *err_message);

void interpreter_exit(void);

void eval_instrs(void);

void eval_instr(instruction_t instr);

module_t *module_global;

#endif //WASM_INTERPRETER_INTERPRETER_H
