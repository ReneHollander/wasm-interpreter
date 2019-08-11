#ifndef WASM_INTERPRETER_CONTROL_OPCODE_HANDLERS_H
#define WASM_INTERPRETER_CONTROL_OPCODE_HANDLERS_H

#include "handler.h"
#include "stack.h"

static inline void eval_br(eval_state_t *eval_state, instruction_t *instr) {
    labelidx labelidx = instr->labelidx;
    frame_t *frame = vec_frame_getp(eval_state->frames, vec_frame_length(eval_state->frames) - labelidx - 1);
    bool has_result = frame->arity > 0 ? true : false;
    val_t val = {0};
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

OP_HANDLER(OP_CALL) {
    eval_call(eval_state, vec_func_getp(eval_state->module->funcs, instr->funcidx));
}

OP_HANDLER(OP_NOP) {
    // Do nothing.
}

OP_HANDLER(OP_UNREACHABLE) {
    THROW_EXCEPTION(EXCEPTION_INTERPRETER_REACHED_OP_UNREACHABLE);
}

OP_HANDLER(OP_BLOCK) {
    u32 arity = instr->block.resulttype.empty ? 0 : 1;
    push_marker_label(eval_state->opd_stack);
    vec_frame_push(eval_state->frames, (frame_t) {
            .instrs = instr->block.instructions,
            .arity = arity,
            .context = CONTROL_CONTEXT,
            .result_type = instr->block.resulttype.type,
    });
}

OP_HANDLER(OP_BR) {
    eval_br(eval_state, instr);
}

OP_HANDLER(OP_BR_IF) {
    i32 val = pop_i32(eval_state->opd_stack);

    if (val != 0) {
        eval_br(eval_state, instr);
    }
}

OP_HANDLER(OP_IF) {
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

OP_HANDLER(OP_LOOP) {
    u32 arity = instr->block.resulttype.empty ? 0 : 1;
    push_marker_label(eval_state->opd_stack);
    vec_frame_push(eval_state->frames, (frame_t) {
            .instrs = instr->block.instructions,
            .arity = arity,
            .context = LOOP_CONTEXT,
            .result_type = instr->block.resulttype.type,
    });
}

OP_HANDLER(OP_RETURN) {
    clean_to_func_marker(eval_state);

    while (!vec_frame_empty(eval_state->frames)) {
        frame_t frame = vec_frame_pop(eval_state->frames);
        context_t context = frame.context;
        if (context == FUNCTION_CONTEXT) {
            break;
        }
    }
}

OP_HANDLER(OP_BR_TABLE) {
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

OP_HANDLER(OP_CALL_INDIRECT) {
    i32 offset = pop_i32(eval_state->opd_stack);
    table_entry_t *table_entry = vec_table_entry_getp(eval_state->table, offset);

    if (!table_entry->initialized) {
        THROW_EXCEPTION_WITH_MSG(EXCEPTION_INTERPRETER_UNINITIALIZED,
                                 "call_indirect referencing uninitialized table entry");
    }

    func_t *func = vec_func_getp(eval_state->module->funcs, table_entry->funcidx);
    eval_call(eval_state, func);
}

#endif // WASM_INTERPRETER_CONTROL_OPCODE_HANDLERS_H