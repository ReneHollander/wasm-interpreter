#ifndef WASM_INTERPRETER_NUMERIC_H
#define WASM_INTERPRETER_NUMERIC_H

#include "type.h"
#include "instruction.h"

bool is_numeric_instr(const opcode_t *opcode);

void eval_numeric_instr(instruction_t *instr);

#endif //WASM_INTERPRETER_NUMERIC_H
