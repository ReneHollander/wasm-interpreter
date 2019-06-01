#ifndef WASM_INTERPRETER_VARIABLE_H
#define WASM_INTERPRETER_VARIABLE_H

#include "type.h"
#include "instruction.h"
#include "list.h"

typedef union val {
    i32 i32;
    i64 i64;
    f32 f32;
    f64 f64;
} val_t;

typedef struct global_entry {
    val_t val;
    valtype_t valtype;
    uint32_t idx;
} global_entry_t;

typedef struct local_entry {
    val_t val;
    valtype_t valtype;
    uint32_t idx;
} local_entry_t;

typedef struct frame {
    node_t *locals;
    node_t *params;
} frame_t;

bool is_variable_instr(opcode_t opcode);

/* frame operations */
void push_frame(void);

void pop_frame(void);

frame_t *peek_frame(void);

/* local operations */
void insert_local_i32(i32 val);

i32 get_local_i32(localidx idx);

void set_local_i32(localidx idx, i32 val);

void insert_local_i64(i64 val);

i64 get_local_i64(localidx idx);

void set_local_i64(localidx idx, i64 val);

void insert_local_f32(f32 val);

f32 get_local_f32(localidx idx);

void set_local_f32(localidx idx, f32 val);

void insert_local_f64(f64 val);

f64 get_local_f64(localidx idx);

void set_local_f64(localidx idx, f64 val);

valtype_t get_local_valtype(localidx idx);

valtype_t get_param_valtype(localidx idx);

/* global operations */
void insert_global_i32(i32 val);

i32 get_global_i32(globalidx idx);

void set_global_i32(globalidx idx, i32 val);

void insert_global_i64(i64 val);

i64 get_global_i64(globalidx idx);

void set_global_i64(globalidx idx, i64 val);

void insert_global_f32(f32 val);

f32 get_global_f32(globalidx idx);

void set_global_f32(globalidx idx, f32 val);

void insert_global_f64(f64 val);

f64 get_global_f64(globalidx idx);

void set_global_f64(globalidx idx, f64 val);

valtype_t get_global_valtype(globalidx idx);

/* **************** */

#endif //WASM_INTERPRETER_VARIABLE_H
