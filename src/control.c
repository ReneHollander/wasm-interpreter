/*
 * Contains everything related to
 * control instructions
 */

#include "stdio.h"
#include "control.h"
#include "variable.h"
#include "stack.h"
#include "interpreter.h"
#include "opd_stack.h"

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
        eval_call(eval_state, &eval_state->module->funcs->values[instr->funcidx]);
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
    vec_valtype_t *fun_output = eval_state->module->types->values[func->type].t2;
    push_frame(eval_state, func->expression.instructions, fun_output->length, fun_output->values[0], FUNCTION_CONTEXT);

    //first we init the locals and afterwards the params because we want the params to come first in the list
    init_locals(eval_state, func->locals);
    init_params(eval_state, eval_state->module->types->values[func->type].t1);
    push_func_marker(eval_state->opd_stack);
    eval_instrs(eval_state);
}

static void eval_call_indirect(eval_state_t *eval_state, instruction_t *instr) {
    i32 offset = pop_i32(eval_state->opd_stack);
    table_entry_t *table_entry = get_at(&eval_state->table, offset);

    if (!table_entry->initialized) {
        interpreter_error(eval_state, "call_indirect referencing uninitialized table entry\n");
    }

    func_t *func = &eval_state->module->funcs->values[table_entry->funcidx];
    eval_call(eval_state, func);
}

static void eval_return(eval_state_t *eval_state) {
    clean_to_func_marker(eval_state);

    for (frame_t *frame; (frame = peek_frame(eval_state)) != NULL;) {
        context_t context = frame->context;
        pop_frame(eval_state);
        if (context == FUNCTION_CONTEXT) {
            break;
        }
    }
}

static void eval_br_if(eval_state_t *eval_state, instruction_t *instr) {
    i32 val = pop_opd_i32(eval_state);

    if (val != 0) {
        eval_br(eval_state, instr);
    }
}

static void eval_br_table(eval_state_t *eval_state, instruction_t *instr) {
    i32 index = pop_opd_i32(eval_state);
    vec_labelidx_t *labels = instr->table.labels;

    if (index < labels->length) {
        instr->labelidx = labels->values[index];
        eval_br(eval_state, instr);
    } else {
        instr->labelidx = instr->table.default_label;
        eval_br(eval_state, instr);
    }
}

static void eval_block(eval_state_t *eval_state, instruction_t *instr) {
    u32 arity = instr->block.resulttype.empty ? 0 : 1;
    push_label(eval_state->opd_stack);
    push_frame(eval_state, instr->block.instructions, arity, instr->block.resulttype.type, CONTROL_CONTEXT);
}

static void eval_loop(eval_state_t *eval_state, instruction_t *instr) {
    u32 arity = instr->block.resulttype.empty ? 0 : 1;
    push_label(eval_state->opd_stack);
    push_frame(eval_state, instr->block.instructions, arity, instr->block.resulttype.type, LOOP_CONTEXT);
}

static void eval_if(eval_state_t *eval_state, instruction_t *instr) {
    u32 arity = instr->if_block.resulttype.empty ? 0 : 1;
    i32 val = pop_opd_i32(eval_state);
    push_label(eval_state->opd_stack);

    if (val != 0) {
        push_frame(eval_state, instr->if_block.ifpath, arity, instr->if_block.resulttype.type, CONTROL_CONTEXT);
    } else if (instr->if_block.elsepath != NULL) {
        push_frame(eval_state, instr->if_block.elsepath, arity, instr->if_block.resulttype.type, CONTROL_CONTEXT);
    }
}

static void eval_br(eval_state_t *eval_state, instruction_t *instr) {
    frame_t *frame = peek_frame(eval_state);
    bool has_result = frame->arity > 0 ? true : false;
    val_t val;
    labelidx labelidx = instr->labelidx;
    context_t context = frame->context;
    valtype_t result_type = frame->result_type;

    //loop jumps do not save result values
    if (has_result && (frame->context != LOOP_CONTEXT || labelidx > 0)) {
        pop_generic(eval_state, frame->result_type, &val);
    }

    for (int32_t lbl_idx = labelidx; lbl_idx >= 0; lbl_idx--) {
        while (!pop_label_or_func_marker(eval_state->opd_stack));
        frame_t *current = peek_frame(eval_state);

        /* if we are in a loop and this is the outer level (lbl_idx = 0),
           we want to jump back to the start (ip=0) and push the label again for
            the next iteration, but we do not want to remove the frame */
        if (lbl_idx == 0 && current->context == LOOP_CONTEXT) {
            push_label(eval_state->opd_stack);
            current->ip = 0;
        } else {
            pop_frame(eval_state);
        }
    }

    if (has_result && (context != LOOP_CONTEXT || labelidx > 0)) {
        push_generic(eval_state, result_type, &val);
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
    frame_t *frame = peek_func_frame(eval_state);
    bool has_result = frame->arity > 0 ? true : false;
    val_t val;

    if (has_result) {
        pop_generic(eval_state, frame->result_type, &val);
    }

    while (!pop_func_marker(eval_state->opd_stack));

    if (has_result) {
        push_generic(eval_state, frame->result_type, &val);
    }
}

void clean_to_label(eval_state_t *eval_state) {
    frame_t *frame = peek_frame(eval_state);
    bool has_result = frame->arity > 0 ? true : false;
    val_t val;

    if (has_result) {
        pop_generic(eval_state, frame->result_type, &val);
    }

    while (!pop_label(eval_state->opd_stack));

    if (has_result) {
        push_generic(eval_state, frame->result_type, &val);
    }
}