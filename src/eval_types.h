#ifndef WASM_INTERPRETER_EVAL_TYPES_H
#define WASM_INTERPRETER_EVAL_TYPES_H

#include "list.h"
#include "stack.h"
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

typedef struct return_value {
    val_t val;
    valtype_t type;
} return_value_t;

typedef struct global_entry {
    val_t val;
    valtype_t valtype;
} global_entry_t;

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

typedef struct eval_state {
    vec_frame_t *frames;     /* Head pointer to list of stack and control frames */
    node_t *globals;    /* Head pointer to list of globals */
    node_t *table;      /* Head pointer to list of table entries */
    node_t *modules;    /* Head pointer to list of all parsed modules */
    stack *opd_stack;   /* Pointer to operand stack */
    module_t *module;   /* Pointer to current module */
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
