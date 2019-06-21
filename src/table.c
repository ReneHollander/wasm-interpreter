#include "table.h"
#include "interpreter.h"
#include "eval_types.h"

void init_tables(eval_state_t *eval_state) {
    if (eval_state->module->tables == NULL || eval_state->module->elem == NULL) {
        return;
    }

    vec_element_t *elements = eval_state->module->elem;
    vec_tabletype_t *tables = eval_state->module->tables;

    if (tables->length > 1) {
        interpreter_error(eval_state, "at most 1 table is supported currently.\n");
    } else if (tables->length == 0) {
        return;
    }

    tabletype_t table = tables->values[0];
    u32 min = table.lim.min;

    for (u32 i = 0; i < min; i++) {
        table_entry_t table_entry;
        table_entry.funcidx = -1;
        table_entry.initialized = false;

        insert_last(&eval_state->table, &table_entry, sizeof(table_entry_t));
    }

    for (int i = 0; i < elements->length; i++) {
        element_t elem = elements->values[i];

        if (elem.table != 0) {
            interpreter_error(eval_state, "only table index 0 is supported currently.\n");
        }

        for (int j = 0; i < elem.offset.instructions->length; i++) {
            eval_instr(eval_state, &elem.offset.instructions->values[j]);
        }

        //the result of the expression should be on the operand stack now, so we can consume it
        u32 offset = pop_i32(eval_state->opd_stack);

        for (int j = 0; j < elem.init->length; j++) {
            table_entry_t *table_entry = get_at(&eval_state->table, offset + j);
            table_entry->initialized = true;
            table_entry->funcidx = elem.init->values[j];
        }
    }
}