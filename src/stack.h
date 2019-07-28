#ifndef WASM_INTERPRETER_STACK_H
#define WASM_INTERPRETER_STACK_H

#include <stdbool.h>
#include <assert.h>

#include "type.h"
#include "eval_types.h"
#include "interpreter.h"

static inline void push_i32(opd_stack_t *stack, i32 value) {
    vec_stack_entry_add(stack, (stack_entry_t) {
            .valtype = VALTYPE_I32,
            .value = (val_t) {
                    .i32 = value,
            },
            .marker = MARKER_VALUE,
    });
}

static inline void push_i64(opd_stack_t *stack, i64 value) {
    vec_stack_entry_add(stack, (stack_entry_t) {
            .valtype = VALTYPE_I64,
            .value = (val_t) {
                    .i64 = value,
            },
            .marker = MARKER_VALUE,
    });
}

static inline void push_f32(opd_stack_t *stack, f32 value) {
    vec_stack_entry_add(stack, (stack_entry_t) {
            .valtype = VALTYPE_F32,
            .value = (val_t) {
                    .f32 = value,
            },
            .marker = MARKER_VALUE,
    });
}

static inline void push_f64(opd_stack_t *stack, f64 value) {
    vec_stack_entry_add(stack, (stack_entry_t) {
            .valtype = VALTYPE_F64,
            .value = (val_t) {
                    .f64 = value,
            },
            .marker = MARKER_VALUE,
    });
}

static inline void push_generic(opd_stack_t *stack, valtype_t valtype, val_t val) {
    vec_stack_entry_add(stack, (stack_entry_t) {
            .valtype = valtype,
            .value = val,
            .marker = MARKER_VALUE,
    });
}

static inline i32 pop_i32(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_pop(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == VALTYPE_I32);
    return entry.value.i32;
}

static inline i64 pop_i64(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_pop(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == VALTYPE_I64);
    return entry.value.i64;
}

static inline f32 pop_f32(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_pop(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == VALTYPE_F32);
    return entry.value.f32;
}

static inline f64 pop_f64(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_pop(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == VALTYPE_F64);
    return entry.value.f64;
}

static inline void pop_generic(opd_stack_t *stack, valtype_t *valtype, val_t *val) {
    stack_entry_t entry = vec_stack_entry_pop(stack);
    assert(entry.marker == MARKER_VALUE);
    *valtype = entry.valtype;
    *val = entry.value;
}

static inline void pop_generic_assert_type(opd_stack_t *stack, valtype_t valtype, val_t *val) {
    stack_entry_t entry = vec_stack_entry_pop(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == valtype);
    *val = entry.value;
}

static inline i32 peek_i32(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_peek(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == VALTYPE_I32);
    return entry.value.i32;
}

static inline i64 peek_i64(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_peek(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == VALTYPE_I64);
    return entry.value.i64;
}

static inline f32 peek_f32(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_peek(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == VALTYPE_F32);
    return entry.value.f32;
}

static inline f64 peek_f64(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_peek(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == VALTYPE_F64);
    return entry.value.f64;
}

static inline void peek_generic(opd_stack_t *stack, valtype_t *valtype, val_t *val) {
    stack_entry_t entry = vec_stack_entry_peek(stack);
    assert(entry.marker == MARKER_VALUE);
    *valtype = entry.valtype;
    *val = entry.value;
}

static inline void peek_generic_assert_type(opd_stack_t *stack, valtype_t valtype, val_t *val) {
    stack_entry_t entry = vec_stack_entry_peek(stack);
    assert(entry.marker == MARKER_VALUE);
    assert(entry.valtype == valtype);
    *val = entry.value;
}

static inline valtype_t peek_valtype(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_peek(stack);
    assert(entry.marker == MARKER_VALUE);
    return entry.valtype;
}

static inline marker_t peek_marker(opd_stack_t *stack) {
    stack_entry_t entry = vec_stack_entry_peek(stack);
    return entry.marker;
}

static inline void push_marker_label(opd_stack_t *stack) {
    vec_stack_entry_add(stack, (stack_entry_t) {
            .marker = MARKER_LABEL,
    });
}

static inline bool pop_marker_label(opd_stack_t *stack) {
    return vec_stack_entry_pop(stack).marker == MARKER_LABEL;
}

static inline void push_marker_func(opd_stack_t *stack) {
    vec_stack_entry_add(stack, (stack_entry_t) {
            .marker = MARKER_FUNCTION,
    });
}

static inline bool pop_marker_func(opd_stack_t *stack) {
    return vec_stack_entry_pop(stack).marker == MARKER_FUNCTION;
}

static inline bool pop_marker_label_or_func(opd_stack_t *stack) {
    marker_t marker = vec_stack_entry_pop(stack).marker;
    return marker == MARKER_FUNCTION || marker == MARKER_LABEL;
}

static inline stack_entry_t *peek(opd_stack_t *stack) {
    return vec_stack_entry_peekp_or(stack, NULL);
}

static inline void drop(opd_stack_t *stack) {
    vec_stack_entry_pop(stack);
}

#endif //WASM_INTERPRETER_STACK_H
