#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "interpreter.h"
#include "instruction.h"
#include "stack.h"
#include "numeric.h"
#include "list.h"
#include "memory.h"
#include "variable.h"

static void init(void);

static void init_globals(vec_global_t *globals);

static void init_global(global_t global);

static void init_locals(vec_locals_t *locals);

static void init_local(locals_t local);

static void init_params(vec_valtype_t *params);

static void init_param(valtype_t param);

static void call_func(func_t func);

static void eval_expr(expression_t expr);

static void eval_instr(instruction_t instr);

static void eval_instrs(vec_instruction_t *instructions);

static void eval_numeric(instruction_t instr);

static void eval_variable(instruction_t instr);

static void eval_local_get(localidx idx);

static void eval_local_set(localidx idx);

static void eval_local_tee(localidx idx);

static void eval_global_get(localidx idx);

static void eval_global_set(localidx idx);

static i32 pop_opd_i32(void);

static i64 pop_opd_i64(void);

static f32 pop_opd_f32(void);

static f64 pop_opd_f64(void);

static void push_opd_i32(i32 val);

static void push_opd_i64(i64 val);

static void push_opd_f32(f32 val);

static void push_opd_f64(f64 val);

static i32 peek_opd_i32();

static i64 peek_opd_i64();

static f32 peek_opd_f32();

static f64 peek_opd_f64();

static func_t find_func(vec_export_t *exports, vec_func_t *funcs, char *func_name);

static void interpreter_error(char *err_message);

static void print_result(func_t func);

stack opd_stack;

module_t *module_global;

/* Intended to call the start function of the module - not yet implemented */
void interpret(module_t *module) {
    module_global = module;
    init();

    if (module->has_start) {
        //call start function
    }
}

/* Intended to call a specific exported function */
void interpret_function(module_t *module, char *func_name) {
    module_global = module;
    init();

    func_t func = find_func(module->exports, module->funcs, func_name);
    call_func(func);

    print_result(func);

    //if the operand stack is not empty, something went wrong
    if (!stack_is_empty(&opd_stack)) {
        interpreter_error("operand stack is not empty");
    }
}

static void print_result(func_t func) {
    vec_valtype_t *fun_output = module_global->types->values[func.type].t2;

    if (fun_output->length == 1) {
        valtype_t valtype = fun_output->values[0];

        switch (valtype) {
            case VALTYPE_I32:
                fprintf(stdout, "result is: %d\n", pop_opd_i32());
                break;
            case VALTYPE_I64:
                fprintf(stdout, "result is: %lld\n", pop_opd_i64());
                break;
            case VALTYPE_F32:
                fprintf(stdout, "result is: %f\n", pop_opd_f32());
                break;
            case VALTYPE_F64:
                fprintf(stdout, "result is: %f\n", pop_opd_f64());
                break;
            default:
                fprintf(stderr, "unknown result valtype: %d\n", valtype);
                exit(EXIT_FAILURE);
        }
    } else if (fun_output->length > 1) {
        interpreter_error("more than one result type is not supported currently\n");
    }
}

static void init(void) {
    stack_init(&opd_stack, 1000);

    init_globals(module_global->globals);
}

static void call_func(func_t func) {
    push_frame();

    //first we init the locals and afterwards the params because we want the params to come first in the list
    init_locals(func.locals);
    init_params(module_global->types->values[func.type].t1);
    eval_expr(func.expression);

    pop_frame();
}

static void init_params(vec_valtype_t *params) {
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
            exit(EXIT_FAILURE);
    }
}

static void init_locals(vec_locals_t *locals) {
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
                exit(EXIT_FAILURE);
        }
    }
}

static void init_globals(vec_global_t *globals) {
    if (globals == NULL) {
        return;
    }

    //insert in reverse order so we have them in the correct order within the list
    for (int i = globals->length - 1; i >= 0; i--) {
        init_global(globals->values[i]);
    }
}

static void init_global(global_t global) {
    eval_expr(global.e);

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
            exit(EXIT_FAILURE);
    }
}

static func_t find_func(vec_export_t *exports, vec_func_t *funcs, char *func_name) {
    funcidx idx = -1;

    for (int i = 0; i < exports->length; i++) {
        if (strcmp(func_name, exports->values[i].name) == 0
            && exports->values[i].desc == EXPORTDESC_FUNC) {
            idx = exports->values[i].func;
        }
    }

    if (idx == -1) {
        fprintf(stderr, "could not find function with name: %s in exports\n", func_name);
        exit(EXIT_FAILURE);
    }

    return funcs->values[idx];
}

static void eval_expr(expression_t expr) {
    eval_instrs(expr.instructions);
}

static void eval_instrs(vec_instruction_t *instructions) {
    u32 length = instructions->length;

    for (int i = 0; i < length; i++) {
        eval_instr(instructions->values[i]);
    }
}

static void eval_instr(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    if (is_numeric_instr(opcode)) {
        eval_numeric(instr);
    } else if (is_variable_instr(opcode)) {
        eval_variable(instr);
    } else if (opcode == OP_CALL) {
        func_t func = module_global->funcs->values[instr.funcidx];
        call_func(func);
    } else {
        fprintf(stderr, "not yet implemented (opcode %x)\n", opcode);
        exit(EXIT_FAILURE);
    }

}

static void eval_numeric(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    if (opcode == OP_I32_CONST) {
        push_opd_i32(instr.const_i32);
    } else if (opcode == OP_I64_CONST) {
        push_opd_i64(instr.const_i64);
    } else if (opcode == OP_F32_CONST) {
        push_opd_f32(instr.const_f32);
    } else if (opcode == OP_F64_CONST) {
        push_opd_f64(instr.const_f64);
    } else if (opcode == OP_F32_ADD) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_f32(add_f32(op1, op2));
    } else if (opcode == OP_F64_ADD) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_f64(add_f64(op1, op2));
    } else if (opcode == OP_I32_ADD) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(add_i32(op1, op2));
    } else if (opcode == OP_I64_ADD) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(add_i64(op1, op2));
    } else if (opcode == OP_F32_SUB) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_f32(sub_f32(op1, op2));
    } else if (opcode == OP_F64_SUB) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_f64(sub_f64(op1, op2));
    } else if (opcode == OP_I32_SUB) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(sub_i32(op1, op2));
    } else if (opcode == OP_I64_SUB) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(sub_i64(op1, op2));
    } else if (opcode == OP_F32_MUL) {
        f32 op2 = pop_opd_f32();
        f32 op1 = pop_opd_f32();
        push_opd_f32(mul_f32(op1, op2));
    } else if (opcode == OP_F64_MUL) {
        f64 op2 = pop_opd_f64();
        f64 op1 = pop_opd_f64();
        push_opd_f64(mul_f64(op1, op2));
    } else if (opcode == OP_I32_MUL) {
        i32 op2 = pop_opd_i32();
        i32 op1 = pop_opd_i32();
        push_opd_i32(mul_i32(op1, op2));
    } else if (opcode == OP_I64_MUL) {
        i64 op2 = pop_opd_i64();
        i64 op1 = pop_opd_i64();
        push_opd_i64(mul_i64(op1, op2));
    } else {
        fprintf(stderr, "numeric instruction with opcode %X currently not supported\n", opcode);
        exit(EXIT_FAILURE);
    }
}

static void eval_variable(instruction_t instr) {
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
            exit(EXIT_FAILURE);
    }
}

static void eval_local_get(localidx idx) {
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
            exit(EXIT_FAILURE);
    }
}

static void eval_local_set(localidx idx) {
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
            exit(EXIT_FAILURE);
    }
}

static void eval_local_tee(localidx idx) {
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
            exit(EXIT_FAILURE);
    }
}

static void eval_global_get(localidx idx) {
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
            exit(EXIT_FAILURE);
    }
}

static void eval_global_set(localidx idx) {
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
            exit(EXIT_FAILURE);
    }
}

/* Some convenience functions defining operand stack operations */
static i32 pop_opd_i32(void) {
    return pop_i32(&opd_stack);
}

static i64 pop_opd_i64(void) {
    return pop_i64(&opd_stack);
}

static f32 pop_opd_f32(void) {
    return pop_f32(&opd_stack);
}

static f64 pop_opd_f64(void) {
    return pop_f64(&opd_stack);
}

static void push_opd_i32(i32 val) {
    push_i32(&opd_stack, val);
}

static void push_opd_i64(i64 val) {
    push_i64(&opd_stack, val);
}

static void push_opd_f32(f32 val) {
    push_f32(&opd_stack, val);
}

static void push_opd_f64(f64 val) {
    push_f64(&opd_stack, val);
}

static i32 peek_opd_i32() {
    return peek_i32(&opd_stack);
}

static i64 peek_opd_i64() {
    return peek_i64(&opd_stack);
}

static f32 peek_opd_f32() {
    return peek_f32(&opd_stack);
}

static f64 peek_opd_f64() {
    return peek_f64(&opd_stack);
}

/* **************************** */

static void interpreter_error(char *err_message) {
    fprintf(stderr, "%s\n", err_message);
    exit(EXIT_FAILURE);
}