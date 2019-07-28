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
#include "strings.h"
#include "eval_types.h"
#include "table.h"
#include "import.h"

static void eval_parametric_instr(eval_state_t *eval_state, instruction_t *instr);

static void eval_global_instrs(eval_state_t *eval_state, vec_instruction_t *instructions);

static bool is_parametric_instr(const opcode_t *opcode);

static void eval_select(eval_state_t *eval_state);

static instruction_t *fetch_next_instr(eval_state_t *eval_state);

eval_state_t *create_interpreter() {
    eval_state_t *eval_state = calloc(sizeof(eval_state_t), 1);

    stack *opd_stack = malloc(sizeof(stack));
    stack_init(opd_stack, 1000);
    eval_state->opd_stack = opd_stack;
    list_init(&eval_state->frames);
    list_init(&eval_state->table);
    list_init(&eval_state->modules);

    return eval_state;
}

void free_interpreter(eval_state_t *eval_state) {
    // TODO: Proper cleanup.
}

/* Intended to call a specific exported function */
return_value_t interpret_function(eval_state_t *eval_state, char *func_name, node_t *args) {
    func_t *func = find_exported_func(eval_state, eval_state->module, func_name);
    for (int i = 0; i < length(&args); i++) {
        parameter_value_t *param = get_at(&args, i);
        push_generic(eval_state, param->type, &param->val);
    }

    eval_call(eval_state, func);
    eval_instrs(eval_state);

    return_value_t return_value = {0};
    vec_valtype_t *fun_output = vec_functype_get(eval_state->module->types, func->type).t2;
    if (vec_valtype_length(fun_output) == 1) {
        return_value.type = vec_valtype_get(fun_output, 0);
        pop_generic(eval_state, vec_valtype_get(fun_output, 0), &return_value.val);
    } else if (vec_valtype_length(fun_output) > 1) {
        interpreter_error(eval_state, "more than one result is not supported currently\n");
    }

    //if the operand stack is not empty, something went wrong
    if (!stack_is_empty(eval_state->opd_stack)) {
        print_stack(eval_state->opd_stack);
        interpreter_error(eval_state, "operand stack is not empty");
    }

    return return_value;
}

void init_datas(eval_state_t *eval_state, vec_data_t *_datas) {
    if (_datas == NULL) {
        return;
    }

    if (vec_data_length(_datas) == 0) return;

    if (vec_data_length(_datas) > 1) {
        interpreter_error(eval_state, "only one data section allowed");
    }

    memory_t *memory = get_current_memory();
    if (memory == NULL) {
        interpreter_error(eval_state, "no memory found");
    }

    data_t data = vec_data_get(_datas, 0);
    if (data.memidx != 0) {
        interpreter_error(eval_state, "data section only for memory with index 0 supported");
    }

    for (int j = 0; j < vec_instruction_length(data.expression.instructions); j++) {
        eval_instr(eval_state, vec_instruction_getp(data.expression.instructions, j));
    }

    //the result of the expression should be on the operand stack now, so we can consume it
    valtype_t result_type;
    val_t val;
    pop_unknown(eval_state, &result_type, &val);
    if (result_type != VALTYPE_I32) {
        interpreter_error(eval_state, "data expression does not evaluate to i32");
    }
    i32 offset = val.i32;

    if (memory->size * PAGE_SIZE < offset + vec_byte_length(data.init)) {
        interpreter_error(eval_state, "data section does not fit into memory");
    }

    // TODO: fix unsafe array access
    memcpy(memory->data + offset, data.init->_elements, data.init->_length);
}

void init_interpreter(eval_state_t *eval_state) {
    bool hasMem = false;
    memtype_t memtype = {0};

    if (eval_state->module->mems != NULL) {
        if (vec_memtype_length(eval_state->module->mems) > 1) {
            interpreter_error(eval_state, "only one memory section supported");
        }
        memtype = vec_memtype_get(eval_state->module->mems, 0);
        memory_t *mem = create_memory(memtype.lim.min);
        use_memory(mem);
    }
    if (eval_state->module->imports != NULL) {
        for (int i = 0; i < vec_import_length(eval_state->module->imports); i++) {
            if (vec_import_get(eval_state->module->imports, i).desc == IMPORTDESC_MEM) {
                if (hasMem) {
                    interpreter_error(eval_state, "only one memory import supported");
                } else {
                    memtype = vec_import_get(eval_state->module->imports, i).mem;
                    hasMem = true;
                }
            }
        }
    }
    //TODO fill eval_state->modules with all parsed modules
    //TODO uncomment: init_imports(eval_state, eval_state->module->imports);
    if (hasMem) {
        init_memory(eval_state, memtype);
    }
    init_tables(eval_state);
    init_globals(eval_state, eval_state->module->globals);
    init_datas(eval_state, eval_state->module->data);

    if (eval_state->module->has_start) {
        eval_call(eval_state, vec_func_getp(eval_state->module->funcs, eval_state->module->start));
        eval_instrs(eval_state);
    }

    if (eval_state->module->exports == NULL || eval_state->module->types == NULL || eval_state->module->funcs == NULL) {
        interpreter_error(eval_state, "could not find all required sections \n");
    }
}

void eval_instrs(eval_state_t *eval_state) {
    for (instruction_t *instr; (instr = fetch_next_instr(eval_state)) != NULL;) {
        eval_instr(eval_state, instr);
    }
}

static instruction_t *fetch_next_instr(eval_state_t *eval_state) {
    for (frame_t *frame; (frame = peek_frame(eval_state)) != NULL;) {
        //if the ip is past the last instruction - clean all but the result argument and pop the frame
        if (frame->ip >= vec_instruction_length(frame->instrs)) {
            if (frame->context == FUNCTION_CONTEXT) {
                clean_to_func_marker(eval_state);
            } else if (frame->context == CONTROL_CONTEXT || frame->context == LOOP_CONTEXT) {
                clean_to_label(eval_state);
            }
            pop_frame(eval_state);
            continue;
        }

        return vec_instruction_getp(frame->instrs, frame->ip++);
    }
}

void eval_instr(eval_state_t *eval_state, instruction_t *instr) {
    opcode_t *opcode = &instr->opcode;

    if (is_numeric_instr(opcode)) {
        eval_numeric_instr(eval_state, instr);
    } else if (is_variable_instr(opcode)) {
        eval_variable_instr(eval_state, instr);
    } else if (is_control_instr(opcode)) {
        eval_control_instr(eval_state, instr);
    } else if (is_parametric_instr(opcode)) {
        eval_parametric_instr(eval_state, instr);
    } else if (is_memory_instr(opcode)) {
        eval_memory_instr(eval_state, *instr);
    } else {
        fprintf(stderr, "not yet implemented instruction %s (opcode 0x%x)\n", opcode2str(*opcode), *opcode);
        interpreter_exit(eval_state);
    }
}

static bool is_parametric_instr(const opcode_t *opcode) {
    switch (*opcode) {
        case OP_DROP:
        case OP_SELECT:
            return true;
        default:
            return false;
    }
}

static void eval_parametric_instr(eval_state_t *eval_state, instruction_t *instr) {
    opcode_t opcode = instr->opcode;

    if (opcode == OP_DROP) {
        drop(eval_state->opd_stack);
    } else if (opcode == OP_SELECT) {
        eval_select(eval_state);
    } else {
        fprintf(stderr, "not yet implemented parametric instruction (opcode %x)\n", opcode);
        interpreter_exit(eval_state);
    }
}

static void eval_select(eval_state_t *eval_state) {
    i32 c = pop_opd_i32(eval_state);
    valtype_t valtype = peek_valtype(eval_state->opd_stack);
    val_t val1;
    val_t val2;
    pop_generic(eval_state, valtype, &val2);
    pop_generic(eval_state, valtype, &val1);

    if (c != 0) {
        push_generic(eval_state, valtype, &val1);
    } else {
        push_generic(eval_state, valtype, &val2);
    }
}

void interpreter_error(eval_state_t *eval_state, char *err_message) {
    fprintf(stderr, "%s\n", err_message);
    interpreter_exit(eval_state);
}

void interpreter_exit(eval_state_t *eval_state) {
    free_interpreter(eval_state);
    exit(EXIT_FAILURE);
}