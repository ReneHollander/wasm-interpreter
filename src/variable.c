#include <stdlib.h>
#include <stdio.h>

#include "variable.h"
#include "interpreter.h"
#include "stack.h"

static inline void eval_local_get(eval_state_t *eval_state, localidx idx) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, idx);
    push_generic(eval_state->opd_stack, local->valtype, local->val);
}

static inline void eval_local_set(eval_state_t *eval_state, localidx idx) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, idx);
    pop_generic_assert_type(eval_state->opd_stack, local->valtype, &local->val);
}

static inline void eval_local_tee(eval_state_t *eval_state, localidx idx) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, idx);
    peek_generic_assert_type(eval_state->opd_stack, local->valtype, &local->val);
}

static inline void eval_global_get(eval_state_t *eval_state, globalidx idx) {
    global_entry_t *global = vec_global_entry_getp(eval_state->globals, idx);
    push_generic(eval_state->opd_stack, global->valtype, global->val);
}

static inline void eval_global_set(eval_state_t *eval_state, globalidx idx) {
    global_entry_t *global = vec_global_entry_getp(eval_state->globals, idx);
    pop_generic_assert_type(eval_state->opd_stack, global->valtype, &global->val);
}

void eval_variable_instr(eval_state_t *eval_state, instruction_t *instr) {
    opcode_t opcode = (*instr).opcode;

    switch (opcode) {
        case OP_LOCAL_GET:
            eval_local_get(eval_state, instr->localidx);
            break;
        case OP_LOCAL_SET:
            eval_local_set(eval_state, instr->localidx);
            break;
        case OP_LOCAL_TEE:
            eval_local_tee(eval_state, instr->localidx);
            break;
        case OP_GLOBAL_GET:
            eval_global_get(eval_state, instr->globalidx);
            break;
        case OP_GLOBAL_SET:
            eval_global_set(eval_state, instr->globalidx);
            break;
        default:
            fprintf(stderr, "variable instruction with opcode %X currently not supported\n", opcode);
            interpreter_exit(eval_state);
    }
}

void init_globals(eval_state_t *eval_state, vec_global_t *_globals) {
    eval_state->globals = vec_global_entry_create();
    if (_globals == NULL) {
        return;
    }

    vec_global_iterator_t it = vec_global_iterator(_globals, IT_FORWARDS);
    while (vec_global_has_next(&it)) {
        global_t *global = vec_global_nextp(&it);
        for (int i = 0; i < vec_instruction_length(global->e.instructions); i++) {
            eval_instr(eval_state, vec_instruction_getp(global->e.instructions, i));
        }

        val_t val;
        pop_generic_assert_type(eval_state->opd_stack, global->gt.t, &val);
        vec_global_entry_add(eval_state->globals, (global_entry_t) {
                .valtype = global->gt.t,
                .val = val,
        });
    }
}

bool is_variable_instr(const opcode_t *opcode) {
    switch (*opcode) {
        case OP_LOCAL_GET:
        case OP_LOCAL_SET:
        case OP_LOCAL_TEE:
        case OP_GLOBAL_GET:
        case OP_GLOBAL_SET:
            return true;
        default:
            return false;
    }
}
