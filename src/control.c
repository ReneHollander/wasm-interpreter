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

static void eval_return(void);

static void eval_br(instruction_t instr);

static void eval_if(instruction_t instr);

static void eval_br_if(instruction_t instr);

static void eval_loop(instruction_t instr);

static void eval_block(instruction_t instr);

void eval_control_instr(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    if (opcode == OP_CALL) {
        call_func(module_global->funcs->values[instr.funcidx]);
    } else if (opcode == OP_NOP) {
        //do nothing
    } else if (opcode == OP_UNREACHABLE) {
        interpreter_exit();
    } else if (opcode == OP_BLOCK) {
        eval_block(instr);
    } else if (opcode == OP_BR) {
        eval_br(instr);
    } else if (opcode == OP_BR_IF) {
        eval_br_if(instr);
    } else if (opcode == OP_IF) {
        eval_if(instr);
    } else if (opcode == OP_LOOP) {
        eval_loop(instr);
    } else if (opcode == OP_RETURN) {
        eval_return();
    } else {
        fprintf(stderr, "not yet implemented control instruction (opcode %x)\n", opcode);
        interpreter_exit();
    }
}

void call_func(func_t func) {
    vec_valtype_t *fun_output = module_global->types->values[func.type].t2;
    push_frame(func.expression.instructions, fun_output->length, fun_output->values[0], FUNCTION_CONTEXT);

    //first we init the locals and afterwards the params because we want the params to come first in the list
    init_locals(func.locals);
    init_params(module_global->types->values[func.type].t1);
    push_func_marker(&opd_stack);
    eval_instrs();
}

static void eval_return(void) {
    clean_to_func_marker();

    frame_t *frame;
    while ((frame = peek_frame()) != NULL) {
        context_t context = frame->context;
        pop_frame();
        if (context == FUNCTION_CONTEXT) {
            break;
        }
    }
}

static void eval_br_if(instruction_t instr) {
    i32 val = pop_opd_i32();

    if (val != 0) {
        eval_br(instr);
    }
}

static void eval_block(instruction_t instr) {
    u32 arity = instr.block.resulttype.empty ? 0 : 1;
    push_label(&opd_stack);
    push_frame(instr.block.instructions, arity, instr.block.resulttype.type, CONTROL_CONTEXT);
}

static void eval_loop(instruction_t instr) {
    u32 arity = instr.block.resulttype.empty ? 0 : 1;
    push_label(&opd_stack);
    push_frame(instr.block.instructions, arity, instr.block.resulttype.type, LOOP_CONTEXT);
}

static void eval_if(instruction_t instr) {
    u32 arity = instr.if_block.resulttype.empty ? 0 : 1;
    i32 val = pop_opd_i32();
    push_label(&opd_stack);

    if (val != 0) {
        push_frame(instr.if_block.ifpath, arity, instr.if_block.resulttype.type, CONTROL_CONTEXT);
    } else if (instr.if_block.elsepath != NULL) {
        push_frame(instr.if_block.elsepath, arity, instr.if_block.resulttype.type, CONTROL_CONTEXT);
    }
}

static void eval_br(instruction_t instr) {
    int32_t lbl_idx = instr.labelidx;
    frame_t *frame = peek_frame();
    bool has_result = frame->arity > 0 ? true : false;
    valtype_t result_type = frame->result_type;
    val_t val;

    //loop jumps do not save result values
    if (has_result && frame->context != LOOP_CONTEXT) {
        pop_generic(result_type, &val);
    }

    while (lbl_idx >= 0) {
        while (!pop_label(&opd_stack));
        frame_t *current = peek_frame();

        if (lbl_idx == 0 && current->context == LOOP_CONTEXT) {
            push_label(&opd_stack);
            current->ip = 0;
        } else {
            pop_frame();
        }
        lbl_idx--;
    }

    if (has_result && frame->context != LOOP_CONTEXT) {
        push_generic(result_type, &val);
    }
}

bool is_control_instr(opcode_t opcode) {
    switch (opcode) {
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

void clean_to_func_marker() {
    frame_t *frame = peek_frame();
    bool has_result = frame->arity > 0 ? true : false;
    val_t val;

    if (has_result) {
        pop_generic(frame->result_type, &val);
    }

    while (!pop_func_marker(&opd_stack));

    if (has_result) {
        push_generic(frame->result_type, &val);
    }
}

void clean_to_label() {
    frame_t *frame = peek_frame();
    bool has_result = frame->arity > 0 ? true : false;
    val_t val;

    if (has_result) {
        pop_generic(frame->result_type, &val);
    }

    while (!pop_label(&opd_stack));

    if (has_result) {
        push_generic(frame->result_type, &val);
    }
}