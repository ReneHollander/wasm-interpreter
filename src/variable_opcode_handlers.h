#ifndef WASM_INTERPRETER_VARIABLE_OPCODE_HANDLER_H
#define WASM_INTERPRETER_VARIABLE_OPCODE_HANDLER_H

#include "handler.h"

OP_HANDLER(OP_LOCAL_GET) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, instr->localidx);
    push_generic(eval_state->opd_stack, local->valtype, local->val);
}

OP_HANDLER(OP_LOCAL_SET) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, instr->localidx);
    pop_generic_assert_type(eval_state->opd_stack, local->valtype, &local->val);
}

OP_HANDLER(OP_LOCAL_TEE) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, instr->localidx);
    peek_generic_assert_type(eval_state->opd_stack, local->valtype, &local->val);
}

OP_HANDLER(OP_GLOBAL_GET) {
    global_entry_t *global = vec_global_entry_getp(eval_state->globals, instr->localidx);
    push_generic(eval_state->opd_stack, global->valtype, global->val);
}

OP_HANDLER(OP_GLOBAL_SET) {
    global_entry_t *global = vec_global_entry_getp(eval_state->globals, instr->localidx);
    pop_generic_assert_type(eval_state->opd_stack, global->valtype, &global->val);
}

#endif // WASM_INTERPRETER_VARIABLE_OPCODE_HANDLER_H
