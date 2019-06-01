/*
 * Contains everything related to
 * variable instructions
 */

#include <stdlib.h>
#include <stdio.h>
#include "variable.h"
#include "list.h"


/* Head pointer for list of frames */
node_t *frames;

/* Head pointer for list of globals */
node_t *globals;

bool is_variable_instr(opcode_t opcode) {
    switch (opcode) {
        case OP_LOCAL_GET:
        case OP_LOCAL_SET:
        case OP_LOCAL_TEE:
        case OP_GLOBAL_GET:
        case OP_GLOBAL_SET:
            return true;
        default:
            return false;
    }
}

valtype_t get_local_valtype(localidx idx) {
    frame_t *frame = peek_frame();

    if (idx >= length(&frame->locals)) {
        fprintf(stderr, "accessed invalid local index: %d\n", idx);
        exit(EXIT_FAILURE);
    }

    local_entry_t *local = get_at(&frame->locals, idx);
    return local->valtype;
}

valtype_t get_param_valtype(localidx idx) {
    frame_t *frame = peek_frame();

    if (idx >= length(&frame->params)) {
        fprintf(stderr, "accessed invalid param index: %d\n", idx);
        exit(EXIT_FAILURE);
    }

    local_entry_t *param = get_at(&frame->params, idx);
    return param->valtype;
}

valtype_t get_global_valtype(globalidx idx) {
    if (idx >= length(&globals)) {
        fprintf(stderr, "accessed invalid global index: %d\n", idx);
        exit(EXIT_FAILURE);
    }

    global_entry_t *global = get_at(&globals, idx);
    return global->valtype;
}

/* Frame operations start here */

void push_frame(void) {
    frame_t *frame = malloc(sizeof(frame_t));
    insert_first(&frames, frame, sizeof(frame_t));
}

void pop_frame(void) {
    delete_first(&frames);
}

frame_t *peek_frame(void) {
    return (frame_t *) get_at(&frames, 0);
}

/* Local operations start here */

void insert_local_i32(i32 val) {
    frame_t *current = peek_frame();
    local_entry_t local;
    local.val.i32 = val;
    local.valtype = VALTYPE_I32;
    insert_first(&current->locals, &local, sizeof(local_entry_t));
}

i32 get_local_i32(localidx idx) {
    frame_t *current = peek_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    return local->val.i32;
}

void set_local_i32(localidx idx, i32 val) {
    frame_t *current = peek_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    local->val.i32 = val;
}


void insert_local_i64(i64 val) {
    frame_t *current = peek_frame();
    local_entry_t local;
    local.val.i64 = val;
    local.valtype = VALTYPE_I64;
    insert_first(&current->locals, &local, sizeof(local_entry_t));
}

i64 get_local_i64(localidx idx) {
    frame_t *current = peek_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    return local->val.i64;
}

void set_local_i64(localidx idx, i64 val) {
    frame_t *current = peek_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    local->val.i64 = val;
}


void insert_local_f32(f32 val) {
    frame_t *current = peek_frame();
    local_entry_t local;
    local.val.f32 = val;
    local.valtype = VALTYPE_F32;
    insert_first(&current->locals, &local, sizeof(local_entry_t));
}

f32 get_local_f32(localidx idx) {
    frame_t *current = peek_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    return local->val.f32;
}

void set_local_f32(localidx idx, f32 val) {
    frame_t *current = peek_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    local->val.f32 = val;
}


void insert_local_f64(f64 val) {
    frame_t *current = peek_frame();
    local_entry_t local;
    local.val.f64 = val;
    local.valtype = VALTYPE_F64;
    insert_first(&current->locals, &local, sizeof(local_entry_t));
}

f64 get_local_f64(localidx idx) {
    frame_t *current = peek_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    return local->val.f64;
}

void set_local_f64(localidx idx, f64 val) {
    frame_t *current = peek_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    local->val.f64 = val;
}

/* Global operations start here */

void insert_global_i32(i32 val) {
    global_entry_t global;
    global.val.i32 = val;
    global.valtype = VALTYPE_I32;
    insert_first(&globals, &global, sizeof(global_entry_t));
}

i32 get_global_i32(globalidx idx) {
    global_entry_t *global = get_at(&globals, idx);
    return global->val.i32;
}

void set_global_i32(globalidx idx, i32 val) {
    global_entry_t *global = get_at(&globals, idx);
    global->val.i32 = val;
}


void insert_global_i64(i64 val) {
    global_entry_t global;
    global.val.i64 = val;
    global.valtype = VALTYPE_I64;
    insert_first(&globals, &global, sizeof(global_entry_t));
}

i64 get_global_i64(globalidx idx) {
    global_entry_t *global = get_at(&globals, idx);
    return global->val.i64;
}

void set_global_i64(globalidx idx, i64 val) {
    global_entry_t *global = get_at(&globals, idx);
    global->val.i64 = val;
}


void insert_global_f32(f32 val) {
    global_entry_t global;
    global.val.f32 = val;
    global.valtype = VALTYPE_F32;
    insert_first(&globals, &global, sizeof(global_entry_t));
}

f32 get_global_f32(globalidx idx) {
    global_entry_t *global = get_at(&globals, idx);
    return global->val.f32;
}

void set_global_f32(globalidx idx, f32 val) {
    global_entry_t *global = get_at(&globals, idx);
    global->val.f32 = val;
}


void insert_global_f64(f64 val) {
    global_entry_t global;
    global.val.f64 = val;
    global.valtype = VALTYPE_F64;
    insert_first(&globals, &global, sizeof(global_entry_t));
}

f64 get_global_f64(globalidx idx) {
    global_entry_t *global = get_at(&globals, idx);
    return global->val.f64;
}

void set_global_f64(globalidx idx, f64 val) {
    global_entry_t *global = get_at(&globals, idx);
    global->val.f64 = val;
}