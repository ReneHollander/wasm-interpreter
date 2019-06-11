#include<stdbool.h>
#include "type.h"
#include "control.h"

#ifndef WASM_INTERPRETER_STACK_H
#define WASM_INTERPRETER_STACK_H

typedef union value {
    i64 i64;
    i32 i32;
    f32 f32;
    f64 f64;
} value_t;

typedef enum marker {
    LABEL,              //marker for labels
    VALUE,              //marker for values
    FUNCTION            //marker for function calls
} marker_t;

typedef struct entry_t {
    value_t value;
    valtype_t valtype;  //only used for actual values
    marker_t marker;
} entry;

typedef struct stack_t {
    int top;
    entry **entry;
    int size;
} stack;

void stack_init(stack *s, uint32_t size);

bool stack_is_empty(stack *s);

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

valtype_t peek_valtype(stack *s);

void push_label(stack *s);

bool pop_label(stack *s);

void push_func_marker(stack *s);

bool pop_func_marker(stack *s);

void drop(stack *s);

void destroy(stack *s);

#endif //WASM_INTERPRETER_STACK_H
