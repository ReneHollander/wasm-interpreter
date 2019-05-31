#include<stdbool.h>
#include "type.h"

#ifndef WASM_INTERPRETER_STACK_H
#define WASM_INTERPRETER_STACK_H

typedef union entry_t {
    int64_t i64;
    int32_t i32;
    f32 f32;
    f64 f64;
} entry;

typedef struct stack_t {
    int top;
    entry **entry;
    int size;
} stack;

void stack_init(stack *s, uint32_t size);

void push_i32(stack *s, i32 value);

void push_i64(stack *s, i64 value);

void push_f32(stack *s, f32 value);

void push_f64(stack *s, f64 value);

i32 pop_i32(stack *s);

i64 pop_i64(stack *s);

f32 pop_f32(stack *s);

f64 pop_f64(stack *s);

i32 peek_i32(stack *s);

i64 peek_i64(stack *s);

f32 peek_f32(stack *s);

f64 peek_f64(stack *s);

#endif //WASM_INTERPRETER_STACK_H
