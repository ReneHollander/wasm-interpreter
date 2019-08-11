#ifndef WASM_INTERPRETER_HANDLER_H
#define WASM_INTERPRETER_HANDLER_H

#define OP_HANDLER(op) static inline void CAT(op, _HANDLER)(eval_state_t *eval_state, instruction_t *instr)

#endif // WASM_INTERPRETER_HANDLER_H