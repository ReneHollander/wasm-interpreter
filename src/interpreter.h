#ifndef WASM_INTERPRETER_INTERPRETER_H
#define WASM_INTERPRETER_INTERPRETER_H

#include "module.h"

void interpret(module_t *module);

void interpret_function(module_t *module, char *func_name);

#endif //WASM_INTERPRETER_INTERPRETER_H
