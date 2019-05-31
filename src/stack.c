#include <stdlib.h>
#include<stdio.h>
#include "stack.h"

void stack_init(stack *s, uint32_t size) {
    s->top = -1;
    s->entry = malloc(size * sizeof(entry *));
    s->size = size;

    if (s->entry == NULL) {
        fprintf(stderr, "could not allocate %ld bytes of memory\n", (size * sizeof(entry *)));
        exit(EXIT_FAILURE);
    }
}

void push_i32(stack *s, int32_t value) {
    if (s->top >= (s->size - 1)) {
        fprintf(stderr, "tried to push onto full stack (location %d, size %d)\n", (s->top + 1), s->size);
        exit(EXIT_FAILURE);
    }

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).i32 = value;
}

void push_i64(stack *s, int64_t value) {
    if (s->top >= (s->size - 1)) {
        fprintf(stderr, "tried to push onto full stack (location %d, size %d)\n", (s->top + 1), s->size);
        exit(EXIT_FAILURE);
    }

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).i64 = value;
}

void push_f32(stack *s, f32 value) {
    if (s->top >= (s->size - 1)) {
        fprintf(stderr, "tried to push onto full stack (location %d, size %d)\n", (s->top + 1), s->size);
        exit(EXIT_FAILURE);
    }

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).f32 = value;
}

void push_f64(stack *s, f64 value) {
    if (s->top >= (s->size - 1)) {
        fprintf(stderr, "tried to push onto full stack (location %d, size %d)\n", (s->top + 1), s->size);
        exit(EXIT_FAILURE);
    }

    *(s->entry + (++s->top)) = malloc(sizeof(entry));
    (**(s->entry + s->top)).f64 = value;
}

i32 pop_i32(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to pop from empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }

    return (**(s->entry + (s->top--))).i32;
}

i64 pop_i64(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to pop from empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }

    return (**(s->entry + (s->top--))).i64;
}

f32 pop_f32(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to pop from empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }

    return (**(s->entry + (s->top--))).f32;
}

f64 pop_f64(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to pop from empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }

    return (**(s->entry + (s->top--))).f64;
}

i32 peek_i32(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to peek at empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }

    return (**(s->entry + (s->top))).i32;
}

i64 peek_i64(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to peek at empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }

    return (**(s->entry + (s->top))).i64;
}

f32 peek_f32(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to peek at empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }

    return (**(s->entry + (s->top))).f32;
}

f64 peek_f64(stack *s) {
    if (s->top <= -1) {
        fprintf(stderr, "tried to peek at empty stack (location %d)\n", (s->top));
        exit(EXIT_FAILURE);
    }

    return (**(s->entry + (s->top))).f64;
}

void destroy(stack *s) {
    //TODO free memory of stack
}