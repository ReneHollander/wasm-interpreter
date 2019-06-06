#include "opd_stack.h"
#include "instruction.h"

/* Some convenience functions defining operand stack operations */
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