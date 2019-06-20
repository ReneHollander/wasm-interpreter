#ifndef WASM_INTERPRETER_VARIABLE_H
#define WASM_INTERPRETER_VARIABLE_H

#include "type.h"
#include "instruction.h"
#include "list.h"
#include "module.h"

typedef union val {
    i32 i32;
    i64 i64;
    f32 f32;
    f64 f64;
} val_t;

typedef struct global_entry {
    val_t val;
    valtype_t valtype;
} global_entry_t;

typedef struct local_entry {
    val_t val;
    valtype_t valtype;
} local_entry_t;

typedef enum context {
    FUNCTION_CONTEXT,
    CONTROL_CONTEXT,
    LOOP_CONTEXT
} context_t;

typedef struct frame {
    node_t *locals;                 //parameters followed by local variables
    uint32_t ip;                    //index of the next instruction to be executed
    vec_instruction_t *instrs;      //pointer to the vector of instructions
    uint32_t arity;                 //number of result arguments (<= 1)
    context_t context;              /*indicates whether this is a stack frame used for function calls,
                                        or a control frame used for control operations, or a loop frame */
    valtype_t result_type;          //type of result (if any)
} frame_t;

bool is_variable_instr(opcode_t opcode);

void eval_variable_instr(instruction_t instr);

void init_globals(vec_global_t *_globals);

void init_locals(vec_locals_t *locals);

void init_params(vec_valtype_t *params);

/* frame operations */
void push_frame(vec_instruction_t *instrs, uint32_t arity, valtype_t result_type, context_t context);

void pop_frame(void);

frame_t *peek_frame(void);

frame_t *peek_func_frame(void);

/* **************** */

#endif //WASM_INTERPRETER_VARIABLE_H
