/*
 * Contains everything related to
 * variable instructions
 */

#include <stdlib.h>
#include <stdio.h>
#include "variable.h"
#include "list.h"
#include "interpreter.h"
#include "opd_stack.h"

static inline void eval_local_get(eval_state_t *eval_state, localidx idx) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, idx);
    push_generic(eval_state, local->valtype, &local->val);
}

static inline void eval_local_set(eval_state_t *eval_state, localidx idx) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, idx);
    valtype_t onstack;
    val_t val;
    pop_unknown(eval_state, &onstack, &val);
    if (onstack != local->valtype) {
        interpreter_error(eval_state, "local_set type mismatch");
    }
    local->val = val;
}

static inline void eval_local_tee(eval_state_t *eval_state, localidx idx) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    local_entry_t *local = vec_local_entry_getp(frame->locals, idx);
    valtype_t onstack;
    val_t val;
    peek_unknown(eval_state, &onstack, &val);
    if (onstack != local->valtype) {
        interpreter_error(eval_state, "local_set type mismatch");
    }
    local->val = val;
}

static inline void eval_global_get(eval_state_t *eval_state, globalidx idx) {
    global_entry_t *global = vec_global_entry_getp(eval_state->globals, idx);
    push_generic(eval_state, global->valtype, &global->val);
}

static inline void eval_global_set(eval_state_t *eval_state, globalidx idx) {
    global_entry_t *global = vec_global_entry_getp(eval_state->globals, idx);
    valtype_t onstack;
    val_t val;
    pop_unknown(eval_state, &onstack, &val);
    if (onstack != global->valtype) {
        interpreter_error(eval_state, "global_set type mismatch");
    }
    global->val = val;
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

        valtype_t onstack;
        val_t val;
        pop_unknown(eval_state, &onstack, &val);
        if (onstack != global->gt.t) {
            interpreter_error(eval_state, "evaluated global type and actual type don't match");
        }
        vec_global_entry_add(eval_state->globals, (global_entry_t) {
                .valtype = onstack,
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
