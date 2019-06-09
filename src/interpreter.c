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
#include "control.h"
#include "opd_stack.h"

static void init(void);

static void eval_parametric_instr(instruction_t instr);

static void eval_global_expr(expression_t expr);

static void eval_global_instrs(vec_instruction_t *instructions);

static bool is_parametric_instr(opcode_t opcode);

static void eval_select(void);

static func_t find_func(vec_export_t *exports, vec_func_t *funcs, char *func_name);

static void print_result(func_t func);

static instruction_t *fetch_next_instr(void);

/* Intended to call the start function of the module - not yet implemented */
void interpret(module_t *module) {
    module_global = module;
    init();

    if (module->types == NULL || module->funcs == NULL) {
        interpreter_error("could not find all required sections \n");
    }

    if (module->has_start) {
        //call start function
    } else {
        interpreter_error("no start function found\n");
    }
}

/* Intended to call a specific exported function */
void interpret_function(module_t *module, char *func_name) {
    module_global = module;
    init();

    if (module->exports == NULL || module->types == NULL || module->funcs == NULL) {
        interpreter_error("could not find all required sections \n");
    }

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
                interpreter_exit();
        }
    } else if (fun_output->length > 1) {
        interpreter_error("more than one result is not supported currently\n");
    }
}

static void init(void) {
    stack_init(&opd_stack, 1000);

    init_globals(module_global->globals);
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
        interpreter_exit();
    }

    return funcs->values[idx];
}

void eval_instrs(void) {
    instruction_t *instr;

    while ((instr = fetch_next_instr()) != NULL) {
        eval_instr(*instr);
    }
}

static instruction_t *fetch_next_instr(void) {
    frame_t *frame;

    while ((frame = peek_frame()) != NULL) {
        //if the ip is past the last instruction - clean all but the result argument and pop the frame
        if (frame->ip >= frame->instrs->length) {
            if (frame->context == FUNCTION_CONTEXT) {
                clean_to_func_marker();
            } else if (frame->context == CONTROL_CONTEXT || frame->context == LOOP_CONTEXT) {
                clean_to_label();
            }
            pop_frame();
            continue;
        }

        return &frame->instrs->values[frame->ip++];
    }
}

void eval_instr(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    if (is_numeric_instr(opcode)) {
        eval_numeric_instr(instr);
    } else if (is_variable_instr(opcode)) {
        eval_variable_instr(instr);
    } else if (is_control_instr(opcode)) {
        eval_control_instr(instr);
    } else if (is_parametric_instr(opcode)) {
        eval_parametric_instr(instr);
    } else {
        fprintf(stderr, "not yet implemented instruction (opcode %x)\n", opcode);
        interpreter_exit();
    }
}

static bool is_parametric_instr(opcode_t opcode) {
    switch (opcode) {
        case OP_DROP:
        case OP_SELECT:
            return true;
        default:
            return false;
    }
}

static void eval_parametric_instr(instruction_t instr) {
    opcode_t opcode = instr.opcode;

    if (opcode == OP_DROP) {
        drop(&opd_stack);
    } else if (opcode == OP_SELECT) {
        eval_select();
    } else {
        fprintf(stderr, "not yet implemented parametric instruction (opcode %x)\n", opcode);
        interpreter_exit();
    }
}

static void eval_select(void) {
    i32 c = pop_opd_i32();
    valtype_t valtype = peek_valtype(&opd_stack);
    val_t val1;
    val_t val2;
    pop_generic(valtype, &val2);
    pop_generic(valtype, &val1);

    if (c != 0) {
        push_generic(valtype, &val1);
    } else {
        push_generic(valtype, &val2);
    }
}

void interpreter_error(char *err_message) {
    fprintf(stderr, "%s\n", err_message);
    interpreter_exit();
}

void interpreter_exit(void) {
    destroy(&opd_stack);
    exit(EXIT_FAILURE);
}