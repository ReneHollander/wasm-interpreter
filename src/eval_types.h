#ifndef WASM_INTERPRETER_EVAL_TYPES_H
#define WASM_INTERPRETER_EVAL_TYPES_H

#include "instruction.h"
#include "module.h"

typedef union val {
    i32 i32;
    i64 i64;
    f32 f32;
    f64 f64;
} val_t;

typedef struct parameter_value {
    val_t val;
    valtype_t type;
} parameter_value_t;

CREATE_VEC(parameter_value_t, parameter_value)

typedef struct return_value {
    val_t val;
    valtype_t type;
} return_value_t;

typedef struct global_entry {
    val_t val;
    valtype_t valtype;
} global_entry_t;

CREATE_VEC(global_entry_t, global_entry)

typedef struct local_entry {
    val_t val;
    valtype_t valtype;
} local_entry_t;

CREATE_VEC(local_entry_t, local_entry)

typedef enum context {
    FUNCTION_CONTEXT,
    CONTROL_CONTEXT,
    LOOP_CONTEXT
} context_t;

typedef struct frame {
    uint32_t ip;                    /* Index of the next instruction to be executed */
    vec_instruction_t *instrs;      /* Pointer to the vector of instructions */
    uint32_t arity;                 /* Number of result arguments (<= 1) */
    context_t context;              /* Indicates whether this is a stack frame used for function calls,
                                        or a control frame used for control operations, or a loop frame */
    valtype_t result_type;          /* Type of result (if any) */
    vec_local_entry_t *locals;          /* Array of parameters followed by local variables */
} frame_t;

CREATE_VEC(frame_t, frame)

typedef struct table_entry {
    bool initialized;
    funcidx funcidx;
} table_entry_t;

CREATE_VEC(table_entry_t, table_entry)

typedef enum marker {
    MARKER_UNKNOWN = 0,
    MARKER_LABEL = 1,              // Marker for labels.
    MARKER_VALUE = 2,              // Marker for values.
    MARKER_FUNCTION = 3,           // Marker for function calls.
} marker_t;

typedef struct stack_entry {
    val_t value;
    valtype_t valtype;
    marker_t marker;
} stack_entry_t;

CREATE_VEC(stack_entry_t, stack_entry)

typedef vec_stack_entry_t opd_stack_t;

typedef struct eval_state {
    vec_frame_t *frames;  /* List of stack and control frames. */
    vec_global_entry_t *globals;  /* List of global variables. */
    vec_table_entry_t *table;  /* List of table entries. */
    vec_module_t *modules;  /* List of modules. */
    opd_stack_t *opd_stack;  /* Operand stack. */
    module_t *module;  /* Pointer to current module */
} eval_state_t;

static inline frame_t *peek_func_frame(vec_frame_t *vec) {
    vec_frame_iterator_t it = vec_frame_iterator(vec, IT_BACKWARDS);
    while (vec_frame_has_next(&it)) {
        frame_t *frame = vec_frame_nextp(&it);
        if (frame->context == FUNCTION_CONTEXT) {
            return frame;
        }
    }
    return NULL;
}

#endif //WASM_INTERPRETER_EVAL_TYPES_H
