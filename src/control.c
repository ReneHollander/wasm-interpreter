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

static void pop_result(valtype_t result_type, val_t *val);

static void push_result(valtype_t result_type, val_t *val);

void eval_control_instr(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    if (opcode == OP_CALL) {
        call_func(module_global->funcs->values[instr.funcidx]);
    } else if (opcode == OP_NOP) {
        //do nothing
    } else if (opcode == OP_UNREACHABLE) {
        interpreter_exit();
    } else if (opcode == OP_BLOCK) {
        u32 arity = instr.block.resulttype.empty ? 0 : 1;
        push_label(&opd_stack);
        push_frame(instr.block.instructions, arity, instr.block.resulttype.type, CONTROL_CONTEXT);
    } else if (opcode == OP_BR) {
        eval_br(instr);
    } else if (opcode == OP_BR_IF) {

    } else if (opcode == OP_IF) {

    } else if (opcode == OP_LOOP) {

    } else if (opcode == OP_RETURN) {
        eval_return();
    } else {
        fprintf(stderr, "not yet implemented control instruction (opcode %x)\n", opcode);
        interpreter_exit();
    }
}

void call_func(func_t func) {
    push_func_marker(&opd_stack);
    vec_valtype_t *fun_output = module_global->types->values[func.type].t2;
    push_frame(func.expression.instructions, fun_output->length, fun_output->values[0], FUNCTION_CONTEXT);

    //first we init the locals and afterwards the params because we want the params to come first in the list
    init_locals(func.locals);
    init_params(module_global->types->values[func.type].t1);
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

static void eval_br(instruction_t instr) {
    int32_t lbl_idx = instr.labelidx;
    frame_t *frame = peek_frame();
    bool has_result = frame->arity > 0 ? true : false;
    valtype_t result_type = frame->result_type;
    val_t val;

    if (has_result) {
        pop_result(result_type, &val);
    }

    //pop control frames until index < 0
    //for each frame pop the values from the operand stack
    while (lbl_idx >= 0) {
        while (!pop_label(&opd_stack));
        pop_frame();
        lbl_idx--;
    }

    if (has_result) {
        push_result(result_type, &val);
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
        pop_result(frame->result_type, &val);
    }

    while (!pop_func_marker(&opd_stack));

    if (has_result) {
        push_result(frame->result_type, &val);
    }
}

void clean_to_label() {
    frame_t *frame = peek_frame();
    bool has_result = frame->arity > 0 ? true : false;
    val_t val;

    if (has_result) {
        pop_result(frame->result_type, &val);
    }

    while (!pop_label(&opd_stack));

    if (has_result) {
        push_result(frame->result_type, &val);
    }
}

static void pop_result(valtype_t result_type, val_t *val) {
    switch (result_type) {
        case VALTYPE_I32:
            val->i32 = pop_i32(&opd_stack);
            break;
        case VALTYPE_F64:
            val->f64 = pop_f64(&opd_stack);
            break;
        case VALTYPE_I64:
            val->i64 = pop_i64(&opd_stack);
            break;
        case VALTYPE_F32:
            val->f32 = pop_f32(&opd_stack);
            break;
        default:
            fprintf(stderr, "unknown return valtype");
            interpreter_exit();
    }
}

static void push_result(valtype_t result_type, val_t *val) {
    switch (result_type) {
        case VALTYPE_I32:
            push_i32(&opd_stack, val->i32);
            break;
        case VALTYPE_F64:
            push_f64(&opd_stack, val->f64);
            break;
        case VALTYPE_I64:
            push_i64(&opd_stack, val->i64);
            break;
        case VALTYPE_F32:
            push_f32(&opd_stack, val->f32);
            break;
        default:
            fprintf(stderr, "unknown return valtype");
            interpreter_exit();
    }
}