/*
 * Contains everything related to
 * variable instructions
 */

#include <stdlib.h>
#include <stdio.h>
#include "variable.h"
#include "list.h"
#include "interpreter.h"
#include "opd_stack.h"

static void init_param(eval_state_t *eval_state, valtype_t param_valtype, localidx idx);

static void init_local(eval_state_t *eval_state, locals_t *local, uint32_t offset);

static void init_global(eval_state_t *eval_state, global_t *global, globalidx idx);

static void eval_global_instrs(eval_state_t *eval_state, vec_instruction_t *instructions);

static valtype_t get_local_valtype(eval_state_t *eval_state, localidx idx);

static valtype_t get_global_valtype(eval_state_t *eval_state, localidx idx);

/* local operations */
static void insert_local_i32(eval_state_t *eval_state, i32 val, localidx idx);

static i32 get_local_i32(eval_state_t *eval_state, localidx idx);

static void set_local_i32(eval_state_t *eval_state, localidx idx, i32 val);

static void insert_local_i64(eval_state_t *eval_state, i64 val, localidx idx);

static i64 get_local_i64(eval_state_t *eval_state, localidx idx);

static void set_local_i64(eval_state_t *eval_state, localidx idx, i64 val);

static void insert_local_f32(eval_state_t *eval_state, f32 val, localidx idx);

static f32 get_local_f32(eval_state_t *eval_state, localidx idx);

static void set_local_f32(eval_state_t *eval_state, localidx idx, f32 val);

static void insert_local_f64(eval_state_t *eval_state, f64 val, localidx idx);

static f64 get_local_f64(eval_state_t *eval_state, localidx idx);

static void set_local_f64(eval_state_t *eval_state, localidx idx, f64 val);

/* global operations */
static void insert_global_i32(eval_state_t *eval_state, globalidx idx, i32 val);

static i32 get_global_i32(eval_state_t *eval_state, globalidx idx);

static void set_global_i32(eval_state_t *eval_state, globalidx idx, i32 val);

static void insert_global_i64(eval_state_t *eval_state, globalidx idx, i64 val);

static i64 get_global_i64(eval_state_t *eval_state, globalidx idx);

static void set_global_i64(eval_state_t *eval_state, globalidx idx, i64 val);

static void insert_global_f32(eval_state_t *eval_state, globalidx idx, f32 val);

static f32 get_global_f32(eval_state_t *eval_state, globalidx idx);

static void set_global_f32(eval_state_t *eval_state, globalidx idx, f32 val);

static void insert_global_f64(eval_state_t *eval_state, globalidx idx, f64 val);

static f64 get_global_f64(eval_state_t *eval_state, globalidx idx);

static void set_global_f64(eval_state_t *eval_state, globalidx idx, f64 val);

/* Eval operations */
static void eval_local_get(eval_state_t *eval_state, localidx idx);

static void eval_local_set(eval_state_t *eval_state, localidx idx);

static void eval_local_tee(eval_state_t *eval_state, localidx idx);

static void eval_global_get(eval_state_t *eval_state, localidx idx);

static void eval_global_set(eval_state_t *eval_state, localidx idx);

void eval_variable_instr(eval_state_t *eval_state, instruction_t *instr) {
    opcode_t opcode = (*instr).opcode;

    switch (opcode) {
        case OP_LOCAL_GET:
            eval_local_get(eval_state, instr->localidx);
            break;
        case OP_LOCAL_SET:
            eval_local_set(eval_state, instr->localidx);
            break;
        case OP_LOCAL_TEE:
            eval_local_tee(eval_state, instr->localidx);
            break;
        case OP_GLOBAL_GET:
            eval_global_get(eval_state, instr->globalidx);
            break;
        case OP_GLOBAL_SET:
            eval_global_set(eval_state, instr->globalidx);
            break;
        default:
            fprintf(stderr, "variable instruction with opcode %X currently not supported\n", opcode);
            interpreter_exit(eval_state);
    }
}

void init_params(eval_state_t *eval_state, vec_valtype_t *params) {
    for (int i = vec_valtype_length(params) - 1; i >= 0; i--) {
        init_param(eval_state, vec_valtype_get(params, i), i);
    }
}

static void init_param(eval_state_t *eval_state, valtype_t param_valtype, localidx idx) {
    //we consume the function params from the stack here
    switch (param_valtype) {
        case VALTYPE_I32:
            insert_local_i32(eval_state, pop_opd_i32(eval_state), idx);
            break;
        case VALTYPE_I64:
            insert_local_i64(eval_state, pop_opd_i64(eval_state), idx);
            break;
        case VALTYPE_F32:
            insert_local_f32(eval_state, pop_opd_f32(eval_state), idx);
            break;
        case VALTYPE_F64:
            insert_local_f64(eval_state, pop_opd_f64(eval_state), idx);
            break;
        default:
            interpreter_exit(eval_state);
    }
}

void init_locals(eval_state_t *eval_state, vec_locals_t *locals, uint32_t offset) {
    for (int i = 0; i < vec_locals_length(locals); i++) {
        init_local(eval_state, vec_locals_getp(locals, i), offset);
        offset += vec_locals_getp(locals, i)->n;
    }
}

static void init_local(eval_state_t *eval_state, locals_t *local, uint32_t offset) {
    for (int i = 0; i < local->n; i++) {
        switch (local->t) {
            case VALTYPE_I32:
                insert_local_i32(eval_state, 0, i + offset);
                break;
            case VALTYPE_I64:
                insert_local_i64(eval_state, 0, i + offset);
                break;
            case VALTYPE_F32:
                insert_local_f32(eval_state, 0, i + offset);
                break;
            case VALTYPE_F64:
                insert_local_f64(eval_state, 0, i + offset);
                break;
            default:
                fprintf(stderr, "unknown valtype for local: %d\n", local->t);
                interpreter_exit(eval_state);
        }
    }
}

void init_globals(eval_state_t *eval_state, vec_global_t *_globals) {
    if (_globals == NULL) {
        return;
    }

//    eval_state->num_globals = vec_global_length(_globals);
    eval_state->globals = malloc(vec_global_length(_globals) * sizeof(global_entry_t));

    for (int i = 0; i < vec_global_length(_globals); i++) {
        init_global(eval_state, vec_global_getp(_globals, i), i);
    }
}

static void init_global(eval_state_t *eval_state, global_t *global, globalidx idx) {
    eval_global_instrs(eval_state, global->e.instructions);

    //the result of the expression should be on the operand stack now, so we can consume it
    switch (global->gt.t) {
        case VALTYPE_I32:
            insert_global_i32(eval_state, idx, pop_opd_i32(eval_state));
            break;
        case VALTYPE_I64:
            insert_global_i64(eval_state, idx, pop_opd_i64(eval_state));
            break;
        case VALTYPE_F32:
            insert_global_f32(eval_state, idx, pop_opd_f32(eval_state));
            break;
        case VALTYPE_F64:
            insert_global_f64(eval_state, idx, pop_opd_f64(eval_state));
            break;
        default:
            fprintf(stderr, "unknown global valtype: %X", global->gt.t);
            interpreter_exit(eval_state);
    }
}

static void eval_global_instrs(eval_state_t *eval_state, vec_instruction_t *instructions) {
    for (int i = 0; i < vec_instruction_length(instructions); i++) {
        eval_instr(eval_state, vec_instruction_getp(instructions, i));
    }
}

void eval_local_get(eval_state_t *eval_state, localidx idx) {
    valtype_t valtype = get_local_valtype(eval_state, idx);

    switch (valtype) {
        case VALTYPE_I32:
            push_opd_i32(eval_state, get_local_i32(eval_state, idx));
            break;
        case VALTYPE_I64:
            push_opd_i64(eval_state, get_local_i64(eval_state, idx));
            break;
        case VALTYPE_F32:
            push_opd_f32(eval_state, get_local_f32(eval_state, idx));
            break;
        case VALTYPE_F64:
            push_opd_f64(eval_state, get_local_f64(eval_state, idx));
            break;
        default:
            fprintf(stderr, "unknown local valtype: %X\n", valtype);
            interpreter_exit(eval_state);
    }
}

void eval_local_set(eval_state_t *eval_state, localidx idx) {
    valtype_t valtype = get_local_valtype(eval_state, idx);

    switch (valtype) {
        case VALTYPE_I32:
            set_local_i32(eval_state, idx, pop_opd_i32(eval_state));
            break;
        case VALTYPE_I64:
            set_local_i64(eval_state, idx, pop_opd_i64(eval_state));
            break;
        case VALTYPE_F32:
            set_local_f32(eval_state, idx, pop_opd_f32(eval_state));
            break;
        case VALTYPE_F64:
            set_local_f64(eval_state, idx, pop_opd_f64(eval_state));
            break;
        default:
            fprintf(stderr, "unknown local valtype: %X\n", valtype);
            interpreter_exit(eval_state);
    }
}

void eval_local_tee(eval_state_t *eval_state, localidx idx) {
    valtype_t valtype = get_local_valtype(eval_state, idx);

    switch (valtype) {
        case VALTYPE_I32:
            set_local_i32(eval_state, idx, peek_opd_i32(eval_state));
            break;
        case VALTYPE_I64:
            set_local_i64(eval_state, idx, peek_opd_i64(eval_state));
            break;
        case VALTYPE_F32:
            set_local_f32(eval_state, idx, peek_opd_f32(eval_state));
            break;
        case VALTYPE_F64:
            set_local_f64(eval_state, idx, peek_opd_f64(eval_state));
            break;
        default:
            fprintf(stderr, "unknown local valtype: %X\n", valtype);
            interpreter_exit(eval_state);
    }
}

void eval_global_get(eval_state_t *eval_state, globalidx idx) {
    valtype_t valtype = get_global_valtype(eval_state, idx);

    switch (valtype) {
        case VALTYPE_I32:
            push_opd_i32(eval_state, get_global_i32(eval_state, idx));
            break;
        case VALTYPE_I64:
            push_opd_i64(eval_state, get_global_i64(eval_state, idx));
            break;
        case VALTYPE_F32:
            push_opd_f32(eval_state, get_global_f32(eval_state, idx));
            break;
        case VALTYPE_F64:
            push_opd_f64(eval_state, get_global_f64(eval_state, idx));
            break;
        default:
            fprintf(stderr, "unknown global valtype: %X\n", valtype);
            interpreter_exit(eval_state);
    }
}

void eval_global_set(eval_state_t *eval_state, globalidx idx) {
    valtype_t valtype = get_global_valtype(eval_state, idx);

    switch (valtype) {
        case VALTYPE_I32:
            set_global_i32(eval_state, idx, pop_opd_i32(eval_state));
            break;
        case VALTYPE_I64:
            set_global_i64(eval_state, idx, pop_opd_i64(eval_state));
            break;
        case VALTYPE_F32:
            set_global_f32(eval_state, idx, pop_opd_f32(eval_state));
            break;
        case VALTYPE_F64:
            set_global_f64(eval_state, idx, pop_opd_f64(eval_state));
            break;
        default:
            fprintf(stderr, "unknown global valtype: %X\n", valtype);
            interpreter_exit(eval_state);
    }
}

bool is_variable_instr(const opcode_t *opcode) {
    switch (*opcode) {
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

static valtype_t get_local_valtype(eval_state_t *eval_state, localidx idx) {
    frame_t *frame = peek_func_frame(eval_state->frames);

    if (idx >= frame->num_locals) {
        fprintf(stderr, "accessed invalid local index: %d\n", idx);
        exit(EXIT_FAILURE);
    }

    return frame->locals[idx].valtype;
}

static valtype_t get_global_valtype(eval_state_t *eval_state, globalidx idx) {
    if (idx >= length(&eval_state->globals)) {
        fprintf(stderr, "accessed invalid global index: %d\n", idx);
        exit(EXIT_FAILURE);
    }

    global_entry_t *global = get_at(&eval_state->globals, idx);
    return global->valtype;
}

/* Local operations start here */

static void insert_local_i32(eval_state_t *eval_state, i32 val, localidx idx) {
    frame_t *current = peek_func_frame(eval_state->frames);
    current->locals[idx].val.i32 = val;
    current->locals[idx].valtype = VALTYPE_I32;
}

static i32 get_local_i32(eval_state_t *eval_state, localidx idx) {
    frame_t *current = peek_func_frame(eval_state->frames);
    return current->locals[idx].val.i32;
}

static void set_local_i32(eval_state_t *eval_state, localidx idx, i32 val) {
    frame_t *current = peek_func_frame(eval_state->frames);
    current->locals[idx].val.i32 = val;
}


static void insert_local_i64(eval_state_t *eval_state, i64 val, localidx idx) {
    frame_t *current = peek_func_frame(eval_state->frames);
    current->locals[idx].val.i64 = val;
    current->locals[idx].valtype = VALTYPE_I64;
}

static i64 get_local_i64(eval_state_t *eval_state, localidx idx) {
    frame_t *current = peek_func_frame(eval_state->frames);
    return current->locals[idx].val.i64;
}

static void set_local_i64(eval_state_t *eval_state, localidx idx, i64 val) {
    frame_t *current = peek_func_frame(eval_state->frames);
    current->locals[idx].val.i64 = val;
}


static void insert_local_f32(eval_state_t *eval_state, f32 val, localidx idx) {
    frame_t *current = peek_func_frame(eval_state->frames);
    current->locals[idx].val.f32 = val;
    current->locals[idx].valtype = VALTYPE_F32;
}

static f32 get_local_f32(eval_state_t *eval_state, localidx idx) {
    frame_t *current = peek_func_frame(eval_state->frames);
    return current->locals[idx].val.f32;
}

static void set_local_f32(eval_state_t *eval_state, localidx idx, f32 val) {
    frame_t *current = peek_func_frame(eval_state->frames);
    current->locals[idx].val.f32 = val;
}


static void insert_local_f64(eval_state_t *eval_state, f64 val, localidx idx) {
    frame_t *current = peek_func_frame(eval_state->frames);
    current->locals[idx].val.f64 = val;
    current->locals[idx].valtype = VALTYPE_F64;
}

static f64 get_local_f64(eval_state_t *eval_state, localidx idx) {
    frame_t *current = peek_func_frame(eval_state->frames);
    return current->locals[idx].val.f64;
}

static void set_local_f64(eval_state_t *eval_state, localidx idx, f64 val) {
    frame_t *current = peek_func_frame(eval_state->frames);
    current->locals[idx].val.f64 = val;
}

/* Global operations start here */

static void insert_global_i32(eval_state_t *eval_state, globalidx idx, i32 val) {
    global_entry_t global;
    global.val.i32 = val;
    global.valtype = VALTYPE_I32;
    insert_first(&eval_state->globals, &global, sizeof(global_entry_t));
}

i32 get_global_i32(eval_state_t *eval_state, globalidx idx) {
    global_entry_t *global = get_at(&eval_state->globals, idx);
    return global->val.i32;
}

void set_global_i32(eval_state_t *eval_state, globalidx idx, i32 val) {
    global_entry_t *global = get_at(&eval_state->globals, idx);
    global->val.i32 = val;
}


void insert_global_i64(eval_state_t *eval_state, globalidx idx, i64 val) {
    global_entry_t global;
    global.val.i64 = val;
    global.valtype = VALTYPE_I64;
    insert_first(&eval_state->globals, &global, sizeof(global_entry_t));
}

i64 get_global_i64(eval_state_t *eval_state, globalidx idx) {
    global_entry_t *global = get_at(&eval_state->globals, idx);
    return global->val.i64;
}

void set_global_i64(eval_state_t *eval_state, globalidx idx, i64 val) {
    global_entry_t *global = get_at(&eval_state->globals, idx);
    global->val.i64 = val;
}


void insert_global_f32(eval_state_t *eval_state, globalidx idx, f32 val) {
    global_entry_t global;
    global.val.f32 = val;
    global.valtype = VALTYPE_F32;
    insert_first(&eval_state->globals, &global, sizeof(global_entry_t));
}

f32 get_global_f32(eval_state_t *eval_state, globalidx idx) {
    global_entry_t *global = get_at(&eval_state->globals, idx);
    return global->val.f32;
}

void set_global_f32(eval_state_t *eval_state, globalidx idx, f32 val) {
    global_entry_t *global = get_at(&eval_state->globals, idx);
    global->val.f32 = val;
}


void insert_global_f64(eval_state_t *eval_state, globalidx idx, f64 val) {
    global_entry_t global;
    global.val.f64 = val;
    global.valtype = VALTYPE_F64;
    insert_first(&eval_state->globals, &global, sizeof(global_entry_t));
}

f64 get_global_f64(eval_state_t *eval_state, globalidx idx) {
    global_entry_t *global = get_at(&eval_state->globals, idx);
    return global->val.f64;
}

void set_global_f64(eval_state_t *eval_state, globalidx idx, f64 val) {
    global_entry_t *global = get_at(&eval_state->globals, idx);
    global->val.f64 = val;
}