#include <stdlib.h>
#include<stdio.h>
#include "stack.h"

void validate_push(stack *s);

void validate_pop(stack *s);

void stack_init(stack *s, uint32_t size) {
    s->top = -1;
    s->entry = malloc(size * sizeof(entry *));
    s->size = size;

    if (s->entry == NULL) {
        fprintf(stderr, "could not allocate %ld bytes of memory\n", (size * sizeof(entry *)));
        exit(EXIT_FAILURE);
    }
}

bool stack_is_empty(stack *s) {
    return s->top == -1;
}

void push_i32(stack *s, i32 value) {
    validate_push(s);

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).value.i32 = value;
    (**(s->entry + s->top)).valtype = VALTYPE_I32;
    (**(s->entry + s->top)).type = VALUE;
}

void push_i64(stack *s, i64 value) {
    validate_push(s);

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).value.i64 = value;
    (**(s->entry + s->top)).valtype = VALTYPE_I64;
    (**(s->entry + s->top)).type = VALUE;
}

void push_f32(stack *s, f32 value) {
    validate_push(s);

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).value.f32 = value;
    (**(s->entry + s->top)).valtype = VALTYPE_F32;
    (**(s->entry + s->top)).type = VALUE;
}

void push_f64(stack *s, f64 value) {
    validate_push(s);

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).value.f64 = value;
    (**(s->entry + s->top)).valtype = VALTYPE_F64;
    (**(s->entry + s->top)).type = VALUE;
}

void push_label(stack *s) {
    validate_push(s);

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).type = LABEL;
}

void push_func_marker(stack *s) {
    validate_push(s);

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).type = FUNCTION;
}

void drop(stack *s) {
    validate_pop(s);

    free(*(s->entry + s->top--));
}

i32 pop_i32(stack *s) {
    validate_pop(s);

    i32 val = (**(s->entry + (s->top))).value.i32;
    free(*(s->entry + s->top--));

    return val;
}

i64 pop_i64(stack *s) {
    validate_pop(s);

    i64 val = (**(s->entry + (s->top))).value.i64;
    free(*(s->entry + s->top--));

    return val;
}

f32 pop_f32(stack *s) {
    validate_pop(s);

    f32 val = (**(s->entry + (s->top))).value.f32;
    free(*(s->entry + s->top--));

    return val;
}

f64 pop_f64(stack *s) {
    validate_pop(s);

    f64 val = (**(s->entry + (s->top))).value.f64;
    free(*(s->entry + s->top--));

    return val;
}

bool pop_label(stack *s) {
    validate_pop(s);

    type_t type = (**(s->entry + (s->top))).type;
    free(*(s->entry + s->top--));

    return type == LABEL;
}

bool pop_func_marker(stack *s) {
    validate_pop(s);

    type_t type = (**(s->entry + (s->top))).type;
    free(*(s->entry + s->top--));

    return type == FUNCTION;
}

i32 peek_i32(stack *s) {
    validate_pop(s);

    return (**(s->entry + (s->top))).value.i32;
}

i64 peek_i64(stack *s) {
    validate_pop(s);

    return (**(s->entry + (s->top))).value.i64;
}

f32 peek_f32(stack *s) {
    validate_pop(s);

    return (**(s->entry + (s->top))).value.f32;
}

f64 peek_f64(stack *s) {
    validate_pop(s);

    return (**(s->entry + (s->top))).value.f64;
}

void destroy(stack *s) {
    free(s->entry);
}

void validate_push(stack *s) {
    if (s->top >= (s->size - 1)) {
        fprintf(stderr, "tried to push onto full stack (location %d, size %d)\n", (s->top + 1), s->size);
        exit(EXIT_FAILURE);
    }
}

void validate_pop(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to pop/peek at empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }
}