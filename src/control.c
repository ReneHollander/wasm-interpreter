#include <stdio.h>

#include "control.h"
#include "variable.h"
#include "stack.h"
#include "interpreter.h"
#include "stack.h"

static void eval_return(eval_state_t *eval_state);

static void eval_br(eval_state_t *eval_state, instruction_t *instr);

static void eval_if(eval_state_t *eval_state, instruction_t *instr);

static void eval_br_if(eval_state_t *eval_state, instruction_t *instr);

static void eval_loop(eval_state_t *eval_state, instruction_t *instr);

static void eval_block(eval_state_t *eval_state, instruction_t *instr);

static void eval_br_table(eval_state_t *eval_state, instruction_t *instr);

static void eval_call_indirect(eval_state_t *eval_state, instruction_t *instr);

void eval_control_instr(eval_state_t *eval_state, instruction_t *instr) {
    opcode_t opcode = instr->opcode;

    if (opcode == OP_CALL) {
        eval_call(eval_state, vec_func_getp(eval_state->module->funcs, instr->funcidx));
    } else if (opcode == OP_NOP) {
        //do nothing
    } else if (opcode == OP_UNREACHABLE) {
        interpreter_exit(eval_state);
    } else if (opcode == OP_BLOCK) {
        eval_block(eval_state, instr);
    } else if (opcode == OP_BR) {
        eval_br(eval_state, instr);
    } else if (opcode == OP_BR_IF) {
        eval_br_if(eval_state, instr);
    } else if (opcode == OP_IF) {
        eval_if(eval_state, instr);
    } else if (opcode == OP_LOOP) {
        eval_loop(eval_state, instr);
    } else if (opcode == OP_RETURN) {
        eval_return(eval_state);
    } else if (opcode == OP_BR_TABLE) {
        eval_br_table(eval_state, instr);
    } else if (opcode == OP_CALL_INDIRECT) {
        eval_call_indirect(eval_state, instr);
    } else {
        fprintf(stderr, "not yet implemented control instruction (opcode %x)\n", opcode);
        interpreter_exit(eval_state);
    }
}

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
        for (int i = 0; i < locals.n; i++) {
            vec_local_entry_add(current->locals, (local_entry_t) {
                    .val = {0},
                    .valtype = locals.t,
            });
        }
    }

    push_marker_func(eval_state->opd_stack);
}

static void eval_call_indirect(eval_state_t *eval_state, instruction_t *instr) {
    i32 offset = pop_i32(eval_state->opd_stack);
    table_entry_t *table_entry = vec_table_entry_getp(eval_state->table, offset);

    if (!table_entry->initialized) {
        interpreter_error(eval_state, "call_indirect referencing uninitialized table entry\n");
    }

    func_t *func = vec_func_getp(eval_state->module->funcs, table_entry->funcidx);
    eval_call(eval_state, func);
}

static void eval_return(eval_state_t *eval_state) {
    clean_to_func_marker(eval_state);

    while (!vec_frame_empty(eval_state->frames)) {
        frame_t frame = vec_frame_pop(eval_state->frames);
        context_t context = frame.context;
        if (context == FUNCTION_CONTEXT) {
            break;
        }
    }
}

static void eval_br_if(eval_state_t *eval_state, instruction_t *instr) {
    i32 val = pop_i32(eval_state->opd_stack);

    if (val != 0) {
        eval_br(eval_state, instr);
    }
}

static void eval_br_table(eval_state_t *eval_state, instruction_t *instr) {
    i32 index = pop_i32(eval_state->opd_stack);
    vec_labelidx_t *labels = instr->table.labels;

    if (index < vec_labelidx_length(labels)) {
        instr->labelidx = vec_labelidx_get(labels, index);
        eval_br(eval_state, instr);
    } else {
        instr->labelidx = instr->table.default_label;
        eval_br(eval_state, instr);
    }
}

static void eval_block(eval_state_t *eval_state, instruction_t *instr) {
    u32 arity = instr->block.resulttype.empty ? 0 : 1;
    push_marker_label(eval_state->opd_stack);
    vec_frame_push(eval_state->frames, (frame_t) {
            .instrs = instr->block.instructions,
            .arity = arity,
            .context = CONTROL_CONTEXT,
            .result_type = instr->block.resulttype.type,
    });
}

static void eval_loop(eval_state_t *eval_state, instruction_t *instr) {
    u32 arity = instr->block.resulttype.empty ? 0 : 1;
    push_marker_label(eval_state->opd_stack);
    vec_frame_push(eval_state->frames, (frame_t) {
            .instrs = instr->block.instructions,
            .arity = arity,
            .context = LOOP_CONTEXT,
            .result_type = instr->block.resulttype.type,
    });
}

static void eval_if(eval_state_t *eval_state, instruction_t *instr) {
    u32 arity = instr->if_block.resulttype.empty ? 0 : 1;
    i32 val = pop_i32(eval_state->opd_stack);
    push_marker_label(eval_state->opd_stack);

    if (val != 0) {
        vec_frame_push(eval_state->frames, (frame_t) {
                .instrs = instr->if_block.ifpath,
                .arity = arity,
                .context = CONTROL_CONTEXT,
                .result_type = instr->if_block.resulttype.type,
        });
    } else if (instr->if_block.elsepath != NULL) {
        vec_frame_push(eval_state->frames, (frame_t) {
                .instrs = instr->if_block.elsepath,
                .arity = arity,
                .context = CONTROL_CONTEXT,
                .result_type = instr->if_block.resulttype.type,
        });
    }
}

static void eval_br(eval_state_t *eval_state, instruction_t *instr) {
    labelidx labelidx = instr->labelidx;
    frame_t *frame = vec_frame_getp(eval_state->frames, vec_frame_length(eval_state->frames) - labelidx - 1);
    bool has_result = frame->arity > 0 ? true : false;
    val_t val;
    valtype_t result_type = frame->result_type;

    if (has_result) {
        pop_generic_assert_type(eval_state->opd_stack, frame->result_type, &val);
    }

    for (int32_t lbl_idx = labelidx; lbl_idx >= 0; lbl_idx--) {
        while (!pop_marker_label_or_func(eval_state->opd_stack));
        frame_t *current = vec_frame_peekp(eval_state->frames);

        /* if we are in a loop and this is the outer level (lbl_idx = 0),
           we want to jump back to the start (ip=0) and push the label again for
            the next iteration, but we do not want to remove the frame */
        if (lbl_idx == 0 && current->context == LOOP_CONTEXT) {
            push_marker_label(eval_state->opd_stack);
            current->ip = 0;
        } else {
            vec_frame_pop(eval_state->frames);
        }
    }

    if (has_result) {
        push_generic(eval_state->opd_stack, result_type, val);
    }
}

bool is_control_instr(const opcode_t *opcode) {
    switch (*opcode) {
        case OP_NOP:
        case OP_UNREACHABLE:
        case OP_BLOCK:
        case OP_LOOP:
        case OP_IF:
        case OP_BR:
        case OP_BR_IF:
        case OP_BR_TABLE:
        case OP_RETURN:
        case OP_CALL:
        case OP_CALL_INDIRECT:
            return true;
        default:
            return false;
    }
}

void clean_to_func_marker(eval_state_t *eval_state) {
    frame_t *frame = peek_func_frame(eval_state->frames);
    bool has_result = frame->arity > 0 ? true : false;
    val_t val;

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