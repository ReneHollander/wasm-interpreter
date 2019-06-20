#include <stdio.h>
#include "variable.h"
#include "opd_stack.h"
#include "instruction.h"
#include "interpreter.h"

/* Some convenience functions defining operand stack operations */

void pop_generic(valtype_t result_type, val_t *val) {
    switch (result_type) {
        case VALTYPE_I32:
            val->i32 = pop_opd_i32();
            break;
        case VALTYPE_F64:
            val->f64 = pop_opd_f64();
            break;
        case VALTYPE_I64:
            val->i64 = pop_opd_i64();
            break;
        case VALTYPE_F32:
            val->f32 = pop_opd_f32();
            break;
        default:
            fprintf(stderr, "unknown return valtype");
            interpreter_exit();
    }
}

void push_generic(valtype_t result_type, val_t *val) {
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

i32 pop_opd_i32(void) {
    return pop_i32(&opd_stack);
}

i64 pop_opd_i64(void) {
    return pop_i64(&opd_stack);
}

f32 pop_opd_f32(void) {
    return pop_f32(&opd_stack);
}

f64 pop_opd_f64(void) {
    return pop_f64(&opd_stack);
}

void push_opd_i32(i32 val) {
    push_i32(&opd_stack, val);
}

void push_opd_i64(i64 val) {
    push_i64(&opd_stack, val);
}

void push_opd_f32(f32 val) {
    push_f32(&opd_stack, val);
}

void push_opd_f64(f64 val) {
    push_f64(&opd_stack, val);
}

i32 peek_opd_i32() {
    return peek_i32(&opd_stack);
}

i64 peek_opd_i64() {
    return peek_i64(&opd_stack);
}

f32 peek_opd_f32() {
    return peek_f32(&opd_stack);
}

f64 peek_opd_f64() {
    return peek_f64(&opd_stack);
}

/* **************************** */