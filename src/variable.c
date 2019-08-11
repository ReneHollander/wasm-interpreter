#include <stdlib.h>
#include <stdio.h>

#include "variable.h"
#include "interpreter.h"
#include "stack.h"
#include "instruction.h"
#include "strings.h"

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
