#ifndef WASM_INTERPRETER_HANDLER_H
#define WASM_INTERPRETER_HANDLER_H

#define OP_HANDLER(op) static void CAT(op, _HANDLER)(eval_state_t SILENCE_UNUSED *eval_state, SILENCE_UNUSED instruction_t *instr)

#endif // WASM_INTERPRETER_HANDLER_H
