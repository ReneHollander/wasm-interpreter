#include "control.h"
#include "variable.h"
#include "stack.h"
#include "interpreter.h"
#include "stack.h"
#include "strings.h"

void eval_call(eval_state_t *eval_state, func_t *func) {
    functype_t *ft = vec_functype_getp(eval_state->module->types, func->type);

    frame_t *current = vec_frame_push(eval_state->frames, (frame_t) {
            .instrs = func->expression.instructions,
            .arity = vec_valtype_length(ft->t2),
            .context = FUNCTION_CONTEXT,
            .result_type = vec_valtype_get_or(ft->t2, 0, VALTYPE_UNKNOWN),
    });

    current->locals = vec_local_entry_create();

    // Prepare locals
    vec_valtype_iterator_t param_it = vec_valtype_iterator(ft->t1, IT_BACKWARDS);
    while (vec_valtype_has_next(&param_it)) {
        valtype_t type = vec_valtype_next(&param_it);
        val_t val;
        pop_generic_assert_type(eval_state->opd_stack, type, &val);
        vec_local_entry_set(current->locals, vec_valtype_get_iterator_index(&param_it), (local_entry_t) {
                .val = val,
                .valtype = type,
        });
    }
    vec_locals_iterator_t locals_it = vec_locals_iterator(func->locals, IT_FORWARDS);
    while (vec_locals_has_next(&locals_it)) {
        locals_t locals = vec_locals_next(&locals_it);
        for (u32 i = 0; i < locals.n; i++) {
            vec_local_entry_add(current->locals, (local_entry_t) {
                    .val = {0},
                    .valtype = locals.t,
            });
        }
    }

    push_marker_func(eval_state->opd_stack);
}

void clean_to_func_marker(eval_state_t *eval_state) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    bool has_result = frame->arity > 0 ? true : false;
    val_t val = {0};

    if (has_result) {
        pop_generic_assert_type(eval_state->opd_stack, frame->result_type, &val);
    }

    while (!pop_marker_func(eval_state->opd_stack));

    if (has_result) {
        push_generic(eval_state->opd_stack, frame->result_type, val);
    }
}

void clean_to_label(eval_state_t *eval_state) {
    frame_t *frame = vec_frame_peekp(eval_state->frames);
    bool has_result = frame->arity > 0 ? true : false;
    val_t val = {0};

    if (has_result) {
        pop_generic_assert_type(eval_state->opd_stack, frame->result_type, &val);
    }

    while (!pop_marker_label(eval_state->opd_stack));

    if (has_result) {
        push_generic(eval_state->opd_stack, frame->result_type, val);
    }
}
