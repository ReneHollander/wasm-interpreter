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

static frame_t *peek_func_frame(void);

static void init_param(valtype_t param_valtype);

static void init_local(locals_t local);

static void init_global(global_t global);

static void eval_global_expr(expression_t expr);

static void eval_global_instrs(vec_instruction_t *instructions);

static valtype_t get_local_valtype(localidx idx);

static valtype_t get_global_valtype(localidx idx);

/* local operations */
static void insert_local_i32(i32 val);

static i32 get_local_i32(localidx idx);

static void set_local_i32(localidx idx, i32 val);

static void insert_local_i64(i64 val);

static i64 get_local_i64(localidx idx);

static void set_local_i64(localidx idx, i64 val);

static void insert_local_f32(f32 val);

static f32 get_local_f32(localidx idx);

static void set_local_f32(localidx idx, f32 val);

static void insert_local_f64(f64 val);

static f64 get_local_f64(localidx idx);

static void set_local_f64(localidx idx, f64 val);

/* global operations */
static void insert_global_i32(i32 val);

static i32 get_global_i32(globalidx idx);

static void set_global_i32(globalidx idx, i32 val);

static void insert_global_i64(i64 val);

static i64 get_global_i64(globalidx idx);

static void set_global_i64(globalidx idx, i64 val);

static void insert_global_f32(f32 val);

static f32 get_global_f32(globalidx idx);

static void set_global_f32(globalidx idx, f32 val);

static void insert_global_f64(f64 val);

static f64 get_global_f64(globalidx idx);

static void set_global_f64(globalidx idx, f64 val);

/* Eval operations */
static void eval_local_get(localidx idx);

static void eval_local_set(localidx idx);

static void eval_local_tee(localidx idx);

static void eval_global_get(localidx idx);

static void eval_global_set(localidx idx);

/* Head pointer for list of stack frames */
static node_t *frames;

/* Head pointer for list of globals */
static node_t *globals;

void eval_variable_instr(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    switch (opcode) {
        case OP_LOCAL_GET:
            eval_local_get(instr.localidx);
            break;
        case OP_LOCAL_SET:
            eval_local_set(instr.localidx);
            break;
        case OP_LOCAL_TEE:
            eval_local_tee(instr.localidx);
            break;
        case OP_GLOBAL_GET:
            eval_global_get(instr.globalidx);
            break;
        case OP_GLOBAL_SET:
            eval_global_set(instr.globalidx);
            break;
        default:
            fprintf(stderr, "variable instruction with opcode %X currently not supported\n", opcode);
            interpreter_exit();
    }
}

void init_params(vec_valtype_t *params) {
    //insert in reverse order so we have them in the correct order within the list
    for (int i = params->length - 1; i >= 0; i--) {
        init_param(params->values[i]);
    }
}

static void init_param(valtype_t param_valtype) {
    //we consume the function params from the stack here
    switch (param_valtype) {
        case VALTYPE_I32:
            insert_local_i32(pop_opd_i32());
            break;
        case VALTYPE_I64:
            insert_local_i64(pop_opd_i64());
            break;
        case VALTYPE_F32:
            insert_local_f32(pop_opd_f32());
            break;
        case VALTYPE_F64:
            insert_local_f64(pop_opd_f64());
            break;
        default:
            interpreter_exit();
    }
}

void init_locals(vec_locals_t *locals) {
    //insert in reverse order so we have them in the correct order within the list
    for (int i = locals->length - 1; i >= 0; i--) {
        init_local(locals->values[i]);
    }
}

static void init_local(locals_t local) {
    for (int i = 0; i < local.n; i++) {
        switch (local.t) {
            case VALTYPE_I32:
                insert_local_i32(0);
                break;
            case VALTYPE_I64:
                insert_local_i64(0);
                break;
            case VALTYPE_F32:
                insert_local_f32(0);
                break;
            case VALTYPE_F64:
                insert_local_f64(0);
                break;
            default:
                fprintf(stderr, "unknown valtype for local: %d\n", local.t);
                interpreter_exit();
        }
    }
}

void init_globals(vec_global_t *_globals) {
    if (_globals == NULL) {
        return;
    }

    //insert in reverse order so we have them in the correct order within the list
    for (int i = _globals->length - 1; i >= 0; i--) {
        init_global(_globals->values[i]);
    }
}

static void init_global(global_t global) {
    eval_global_expr(global.e);

    //the result of the expression should be on the operand stack now, so we can consume it
    switch (global.gt.t) {
        case VALTYPE_I32:
            insert_global_i32(pop_opd_i32());
            break;
        case VALTYPE_I64:
            insert_global_i64(pop_opd_i64());
            break;
        case VALTYPE_F32:
            insert_global_f32(pop_opd_f32());
            break;
        case VALTYPE_F64:
            insert_global_f64(pop_opd_f64());
            break;
        default:
            fprintf(stderr, "unknown global valtype: %X", global.gt.t);
            interpreter_exit();
    }
}

static void eval_global_expr(expression_t expr) {
    eval_global_instrs(expr.instructions);
}

static void eval_global_instrs(vec_instruction_t *instructions) {
    u32 length = instructions->length;

    for (int i = 0; i < length; i++) {
        eval_instr(instructions->values[i]);
    }
}

void eval_local_get(localidx idx) {
    valtype_t valtype = get_local_valtype(idx);

    switch (valtype) {
        case VALTYPE_I32:
            push_opd_i32(get_local_i32(idx));
            break;
        case VALTYPE_I64:
            push_opd_i64(get_local_i64(idx));
            break;
        case VALTYPE_F32:
            push_opd_f32(get_local_f32(idx));
            break;
        case VALTYPE_F64:
            push_opd_f64(get_local_f64(idx));
            break;
        default:
            fprintf(stderr, "unknown local valtype: %X\n", valtype);
            interpreter_exit();
    }
}

void eval_local_set(localidx idx) {
    valtype_t valtype = get_local_valtype(idx);

    switch (valtype) {
        case VALTYPE_I32:
            set_local_i32(idx, pop_opd_i32());
            break;
        case VALTYPE_I64:
            set_local_i64(idx, pop_opd_i64());
            break;
        case VALTYPE_F32:
            set_local_f32(idx, pop_opd_f32());
            break;
        case VALTYPE_F64:
            set_local_f64(idx, pop_opd_f64());
            break;
        default:
            fprintf(stderr, "unknown local valtype: %X\n", valtype);
            interpreter_exit();
    }
}

void eval_local_tee(localidx idx) {
    valtype_t valtype = get_local_valtype(idx);

    switch (valtype) {
        case VALTYPE_I32:
            set_local_i32(idx, peek_opd_i32());
            break;
        case VALTYPE_I64:
            set_local_i64(idx, peek_opd_i64());
            break;
        case VALTYPE_F32:
            set_local_f32(idx, peek_opd_f32());
            break;
        case VALTYPE_F64:
            set_local_f64(idx, peek_opd_f64());
            break;
        default:
            fprintf(stderr, "unknown local valtype: %X\n", valtype);
            interpreter_exit();
    }
}

void eval_global_get(localidx idx) {
    valtype_t valtype = get_global_valtype(idx);

    switch (valtype) {
        case VALTYPE_I32:
            push_opd_i32(get_global_i32(idx));
            break;
        case VALTYPE_I64:
            push_opd_i64(get_global_i64(idx));
            break;
        case VALTYPE_F32:
            push_opd_f32(get_global_f32(idx));
            break;
        case VALTYPE_F64:
            push_opd_f64(get_global_f64(idx));
            break;
        default:
            fprintf(stderr, "unknown global valtype: %X\n", valtype);
            interpreter_exit();
    }
}

void eval_global_set(localidx idx) {
    valtype_t valtype = get_global_valtype(idx);

    switch (valtype) {
        case VALTYPE_I32:
            set_global_i32(idx, pop_opd_i32());
            break;
        case VALTYPE_I64:
            set_global_i64(idx, pop_opd_i64());
            break;
        case VALTYPE_F32:
            set_global_f32(idx, pop_opd_f32());
            break;
        case VALTYPE_F64:
            set_global_f64(idx, pop_opd_f64());
            break;
        default:
            fprintf(stderr, "unknown global valtype: %X\n", valtype);
            interpreter_exit();
    }
}

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

static valtype_t get_local_valtype(localidx idx) {
    frame_t *frame = peek_func_frame();

    if (idx >= length(&frame->locals)) {
        fprintf(stderr, "accessed invalid local index: %d\n", idx);
        exit(EXIT_FAILURE);
    }

    local_entry_t *local = get_at(&frame->locals, idx);
    return local->valtype;
}

static valtype_t get_global_valtype(globalidx idx) {
    if (idx >= length(&globals)) {
        fprintf(stderr, "accessed invalid global index: %d\n", idx);
        exit(EXIT_FAILURE);
    }

    global_entry_t *global = get_at(&globals, idx);
    return global->valtype;
}

/* Frame operations start here */

void push_frame(vec_instruction_t *instrs, uint32_t arity, valtype_t result_type, context_t context) {
    frame_t frame;
    frame.ip = 0;
    frame.locals = NULL;
    frame.instrs = instrs;
    frame.context = context;
    frame.arity = arity;
    frame.result_type = result_type;
    insert_first(&frames, &frame, sizeof(frame_t));
}

void pop_frame(void) {
    delete_first(&frames);
}

frame_t *peek_frame(void) {
    return (frame_t *) get_at(&frames, 0);
}

static frame_t *peek_func_frame(void) {
    node_t *cur = frames;

    for (int i = 0; cur != NULL; i++) {
        if (((frame_t *) cur->data)->context == FUNCTION_CONTEXT) {
            return cur->data;
        }

        cur = cur->next;
    }

    return NULL;
}

/* Local operations start here */

void insert_local_i32(i32 val) {
    frame_t *current = peek_func_frame();
    local_entry_t local;
    local.val.i32 = val;
    local.valtype = VALTYPE_I32;
    insert_first(&current->locals, &local, sizeof(local_entry_t));
}

i32 get_local_i32(localidx idx) {
    frame_t *current = peek_func_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    return local->val.i32;
}

void set_local_i32(localidx idx, i32 val) {
    frame_t *current = peek_func_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    local->val.i32 = val;
}


void insert_local_i64(i64 val) {
    frame_t *current = peek_func_frame();
    local_entry_t local;
    local.val.i64 = val;
    local.valtype = VALTYPE_I64;
    insert_first(&current->locals, &local, sizeof(local_entry_t));
}

i64 get_local_i64(localidx idx) {
    frame_t *current = peek_func_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    return local->val.i64;
}

void set_local_i64(localidx idx, i64 val) {
    frame_t *current = peek_func_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    local->val.i64 = val;
}


void insert_local_f32(f32 val) {
    frame_t *current = peek_func_frame();
    local_entry_t local;
    local.val.f32 = val;
    local.valtype = VALTYPE_F32;
    insert_first(&current->locals, &local, sizeof(local_entry_t));
}

f32 get_local_f32(localidx idx) {
    frame_t *current = peek_func_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    return local->val.f32;
}

void set_local_f32(localidx idx, f32 val) {
    frame_t *current = peek_func_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    local->val.f32 = val;
}


void insert_local_f64(f64 val) {
    frame_t *current = peek_func_frame();
    local_entry_t local;
    local.val.f64 = val;
    local.valtype = VALTYPE_F64;
    insert_first(&current->locals, &local, sizeof(local_entry_t));
}

f64 get_local_f64(localidx idx) {
    frame_t *current = peek_func_frame();
    local_entry_t *local = get_at(&current->locals, idx);
    return local->val.f64;
}

void set_local_f64(localidx idx, f64 val) {
    frame_t *current = peek_func_frame();
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