#ifndef WASM_INTERPRETER_CONTROL_H
#define WASM_INTERPRETER_CONTROL_H

#include "type.h"
#include "module.h"

bool is_control_instr(opcode_t opcode);

void clean_to_func_marker();

void clean_to_label();

void eval_control_instr(instruction_t instr);

void eval_call(func_t func);

#endif //WASM_INTERPRETER_CONTROL_H
