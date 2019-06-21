#ifndef WASM_INTERPRETER_NUMERIC_H
#define WASM_INTERPRETER_NUMERIC_H

#include "type.h"
#include "instruction.h"
#include "interpreter.h"

bool is_numeric_instr(const opcode_t *opcode);

void eval_numeric_instr(eval_state_t *eval_state, instruction_t *instr);

#endif //WASM_INTERPRETER_NUMERIC_H
