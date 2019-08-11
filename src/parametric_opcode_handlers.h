#ifndef WASM_INTERPRETER_PARAMETRIC_OPCODE_HANDLERS_H
#define WASM_INTERPRETER_PARAMETRIC_OPCODE_HANDLERS_H

#include "handler.h"
#include "stack.h"

OP_HANDLER(OP_DROP) {
    drop(eval_state->opd_stack);
}

OP_HANDLER(OP_SELECT) {
    i32 c = pop_i32(eval_state->opd_stack);
    valtype_t valtype = peek_valtype(eval_state->opd_stack);
    val_t val1;
    val_t val2;
    pop_generic_assert_type(eval_state->opd_stack, valtype, &val2);
    pop_generic_assert_type(eval_state->opd_stack, valtype, &val1);

    if (c != 0) {
        push_generic(eval_state->opd_stack, valtype, val1);
    } else {
        push_generic(eval_state->opd_stack, valtype, val2);
    }
}

#endif // WASM_INTERPRETER_PARAMETRIC_OPCODE_HANDLERS_H
