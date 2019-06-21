#include <stdio.h>
#include "variable.h"
#include "opd_stack.h"
#include "instruction.h"
#include "interpreter.h"

/* Some convenience functions defining operand stack operations */

void pop_unknown(eval_state_t *eval_state, valtype_t *result_type, val_t *val) {
    *result_type = peek_valtype(eval_state->opd_stack);
    pop_generic(eval_state, *result_type, val);
}

void pop_generic(eval_state_t *eval_state, valtype_t result_type, val_t *val) {
    switch (result_type) {
        case VALTYPE_I32:
            val->i32 = pop_opd_i32(eval_state);
            break;
        case VALTYPE_F64:
            val->f64 = pop_opd_f64(eval_state);
            break;
        case VALTYPE_I64:
            val->i64 = pop_opd_i64(eval_state);
            break;
        case VALTYPE_F32:
            val->f32 = pop_opd_f32(eval_state);
            break;
        default:
            fprintf(stderr, "unknown return valtype");
            interpreter_exit(eval_state);
    }
}

void push_generic(eval_state_t *eval_state, valtype_t result_type, val_t *val) {
    switch (result_type) {
        case VALTYPE_I32:
            push_i32(eval_state->opd_stack, val->i32);
            break;
        case VALTYPE_F64:
            push_f64(eval_state->opd_stack, val->f64);
            break;
        case VALTYPE_I64:
            push_i64(eval_state->opd_stack, val->i64);
            break;
        case VALTYPE_F32:
            push_f32(eval_state->opd_stack, val->f32);
            break;
        default:
            fprintf(stderr, "unknown return valtype");
            interpreter_exit(eval_state);
    }
}

i32 pop_opd_i32(eval_state_t *eval_state) {
    return pop_i32(eval_state->opd_stack);
}

i64 pop_opd_i64(eval_state_t *eval_state) {
    return pop_i64(eval_state->opd_stack);
}

f32 pop_opd_f32(eval_state_t *eval_state) {
    return pop_f32(eval_state->opd_stack);
}

f64 pop_opd_f64(eval_state_t *eval_state) {
    return pop_f64(eval_state->opd_stack);
}

void push_opd_i32(eval_state_t *eval_state, i32 val) {
    push_i32(eval_state->opd_stack, val);
}

void push_opd_i64(eval_state_t *eval_state, i64 val) {
    push_i64(eval_state->opd_stack, val);
}

void push_opd_f32(eval_state_t *eval_state, f32 val) {
    push_f32(eval_state->opd_stack, val);
}

void push_opd_f64(eval_state_t *eval_state, f64 val) {
    push_f64(eval_state->opd_stack, val);
}

i32 peek_opd_i32(eval_state_t *eval_state) {
    return peek_i32(eval_state->opd_stack);
}

i64 peek_opd_i64(eval_state_t *eval_state) {
    return peek_i64(eval_state->opd_stack);
}

f32 peek_opd_f32(eval_state_t *eval_state) {
    return peek_f32(eval_state->opd_stack);
}

f64 peek_opd_f64(eval_state_t *eval_state) {
    return peek_f64(eval_state->opd_stack);
}

/* **************************** */