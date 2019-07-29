#include "table.h"
#include "interpreter.h"
#include "eval_types.h"
#include "stack.h"

void init_tables(eval_state_t *eval_state) {
    if (eval_state->module->tables == NULL || eval_state->module->elem == NULL) {
        return;
    }

    vec_element_t *elements = eval_state->module->elem;
    vec_tabletype_t *tables = eval_state->module->tables;

    if (vec_tabletype_length(tables) > 1) {
        interpreter_error(eval_state, "at most 1 table is supported currently.\n");
    } else if (vec_tabletype_length(tables) == 0) {
        return;
    }

    tabletype_t table = vec_tabletype_get(tables, 0);
    u32 min = table.lim.min;

    for (u32 i = 0; i < min; i++) {
        vec_table_entry_add(eval_state->table, (table_entry_t) {
                .funcidx = -1,
                .initialized = false,
        });
    }

    for (int i = 0; i < vec_element_length(elements); i++) {
        element_t elem = vec_element_get(elements, i);

        if (elem.table != 0) {
            interpreter_error(eval_state, "only table index 0 is supported currently.\n");
        }

        for (int j = 0; i < vec_instruction_length(elem.offset.instructions); i++) {
            eval_instr(eval_state, vec_instruction_getp(elem.offset.instructions, j));
        }

        //the result of the expression should be on the operand stack now, so we can consume it
        u32 offset = pop_i32(eval_state->opd_stack);

        for (int j = 0; j < vec_funcidx_length(elem.init); j++) {
            table_entry_t *table_entry = vec_table_entry_getp(eval_state->table, offset + j);
            table_entry->initialized = true;
            table_entry->funcidx = vec_funcidx_get(elem.init, j);
        }
    }
}