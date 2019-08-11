#ifndef WASM_INTERPRETER_OPCODE_H
#define WASM_INTERPRETER_OPCODE_H

#include "eval_types.h"
#include "instruction.h"
#include "numeric_opcode_handlers.h"
#include "variable_opcode_handlers.h"
#include "memory_opcode_handlers.h"
#include "parametric_opcode_handlers.h"
#include "control_opcode_handlers.h"

#define REGISTER_OP_HANDLER(op) case op: CAT(op, _HANDLER(eval_state, instr)); return

static inline void handle_instruction(eval_state_t *eval_state, instruction_t *instr) {
    switch (instr->opcode) {
        REGISTER_OP_HANDLER(OP_LOCAL_GET);
        REGISTER_OP_HANDLER(OP_LOCAL_SET);
        REGISTER_OP_HANDLER(OP_LOCAL_TEE);
        REGISTER_OP_HANDLER(OP_GLOBAL_GET);
        REGISTER_OP_HANDLER(OP_GLOBAL_SET);

        REGISTER_OP_HANDLER(OP_I32_CONST);
        REGISTER_OP_HANDLER(OP_I64_CONST);
        REGISTER_OP_HANDLER(OP_F32_CONST);
        REGISTER_OP_HANDLER(OP_F64_CONST);

        REGISTER_OP_HANDLER(OP_I32_EQZ);
        REGISTER_OP_HANDLER(OP_I32_EQ);
        REGISTER_OP_HANDLER(OP_I32_NE);
        REGISTER_OP_HANDLER(OP_I32_LT_S);
        REGISTER_OP_HANDLER(OP_I32_LT_U);
        REGISTER_OP_HANDLER(OP_I32_GT_S);
        REGISTER_OP_HANDLER(OP_I32_GT_U);
        REGISTER_OP_HANDLER(OP_I32_LE_S);
        REGISTER_OP_HANDLER(OP_I32_LE_U);
        REGISTER_OP_HANDLER(OP_I32_GE_S);
        REGISTER_OP_HANDLER(OP_I32_GE_U);

        REGISTER_OP_HANDLER(OP_I64_EQZ);
        REGISTER_OP_HANDLER(OP_I64_EQ);
        REGISTER_OP_HANDLER(OP_I64_NE);
        REGISTER_OP_HANDLER(OP_I64_LT_S);
        REGISTER_OP_HANDLER(OP_I64_LT_U);
        REGISTER_OP_HANDLER(OP_I64_GT_S);
        REGISTER_OP_HANDLER(OP_I64_GT_U);
        REGISTER_OP_HANDLER(OP_I64_LE_S);
        REGISTER_OP_HANDLER(OP_I64_LE_U);
        REGISTER_OP_HANDLER(OP_I64_GE_S);
        REGISTER_OP_HANDLER(OP_I64_GE_U);

        REGISTER_OP_HANDLER(OP_F32_EQ);
        REGISTER_OP_HANDLER(OP_F32_NE);
        REGISTER_OP_HANDLER(OP_F32_LT);
        REGISTER_OP_HANDLER(OP_F32_GT);
        REGISTER_OP_HANDLER(OP_F32_LE);
        REGISTER_OP_HANDLER(OP_F32_GE);

        REGISTER_OP_HANDLER(OP_F64_EQ);
        REGISTER_OP_HANDLER(OP_F64_NE);
        REGISTER_OP_HANDLER(OP_F64_LT);
        REGISTER_OP_HANDLER(OP_F64_GT);
        REGISTER_OP_HANDLER(OP_F64_LE);
        REGISTER_OP_HANDLER(OP_F64_GE);

        REGISTER_OP_HANDLER(OP_I32_CLZ);
        REGISTER_OP_HANDLER(OP_I32_CTZ);
        REGISTER_OP_HANDLER(OP_I32_POPCNT);
        REGISTER_OP_HANDLER(OP_I32_ADD);
        REGISTER_OP_HANDLER(OP_I32_SUB);
        REGISTER_OP_HANDLER(OP_I32_MUL);
        REGISTER_OP_HANDLER(OP_I32_DIV_S);
        REGISTER_OP_HANDLER(OP_I32_DIV_U);
        REGISTER_OP_HANDLER(OP_I32_REM_S);
        REGISTER_OP_HANDLER(OP_I32_REM_U);
        REGISTER_OP_HANDLER(OP_I32_AND);
        REGISTER_OP_HANDLER(OP_I32_OR);
        REGISTER_OP_HANDLER(OP_I32_XOR);
        REGISTER_OP_HANDLER(OP_I32_SHL);
        REGISTER_OP_HANDLER(OP_I32_SHR_U);
        REGISTER_OP_HANDLER(OP_I32_SHR_S);
        REGISTER_OP_HANDLER(OP_I32_ROTL);
        REGISTER_OP_HANDLER(OP_I32_ROTR);

        REGISTER_OP_HANDLER(OP_I64_CLZ);
        REGISTER_OP_HANDLER(OP_I64_CTZ);
        REGISTER_OP_HANDLER(OP_I64_POPCNT);
        REGISTER_OP_HANDLER(OP_I64_ADD);
        REGISTER_OP_HANDLER(OP_I64_SUB);
        REGISTER_OP_HANDLER(OP_I64_MUL);
        REGISTER_OP_HANDLER(OP_I64_DIV_S);
        REGISTER_OP_HANDLER(OP_I64_DIV_U);
        REGISTER_OP_HANDLER(OP_I64_REM_S);
        REGISTER_OP_HANDLER(OP_I64_REM_U);
        REGISTER_OP_HANDLER(OP_I64_AND);
        REGISTER_OP_HANDLER(OP_I64_OR);
        REGISTER_OP_HANDLER(OP_I64_XOR);
        REGISTER_OP_HANDLER(OP_I64_SHL);
        REGISTER_OP_HANDLER(OP_I64_SHR_U);
        REGISTER_OP_HANDLER(OP_I64_SHR_S);
        REGISTER_OP_HANDLER(OP_I64_ROTL);
        REGISTER_OP_HANDLER(OP_I64_ROTR);

        REGISTER_OP_HANDLER(OP_F32_ABS);
        REGISTER_OP_HANDLER(OP_F32_NEG);
        REGISTER_OP_HANDLER(OP_F32_CEIL);
        REGISTER_OP_HANDLER(OP_F32_FLOOR);
        REGISTER_OP_HANDLER(OP_F32_TRUNC);
        REGISTER_OP_HANDLER(OP_F32_NEAREST);
        REGISTER_OP_HANDLER(OP_F32_SQRT);
        REGISTER_OP_HANDLER(OP_F32_ADD);
        REGISTER_OP_HANDLER(OP_F32_SUB);
        REGISTER_OP_HANDLER(OP_F32_MUL);
        REGISTER_OP_HANDLER(OP_F32_DIV);
        REGISTER_OP_HANDLER(OP_F32_MIN);
        REGISTER_OP_HANDLER(OP_F32_MAX);
        REGISTER_OP_HANDLER(OP_F32_COPYSIGN);

        REGISTER_OP_HANDLER(OP_F64_ABS);
        REGISTER_OP_HANDLER(OP_F64_NEG);
        REGISTER_OP_HANDLER(OP_F64_CEIL);
        REGISTER_OP_HANDLER(OP_F64_FLOOR);
        REGISTER_OP_HANDLER(OP_F64_TRUNC);
        REGISTER_OP_HANDLER(OP_F64_NEAREST);
        REGISTER_OP_HANDLER(OP_F64_SQRT);
        REGISTER_OP_HANDLER(OP_F64_ADD);
        REGISTER_OP_HANDLER(OP_F64_SUB);
        REGISTER_OP_HANDLER(OP_F64_MUL);
        REGISTER_OP_HANDLER(OP_F64_DIV);
        REGISTER_OP_HANDLER(OP_F64_MIN);
        REGISTER_OP_HANDLER(OP_F64_MAX);
        REGISTER_OP_HANDLER(OP_F64_COPYSIGN);

        REGISTER_OP_HANDLER(OP_I32_WRAP_I64);
        REGISTER_OP_HANDLER(OP_I32_TRUNC_F32_S);
        REGISTER_OP_HANDLER(OP_I32_TRUNC_F32_U);
        REGISTER_OP_HANDLER(OP_I32_TRUNC_F64_S);
        REGISTER_OP_HANDLER(OP_I32_TRUNC_F64_U);
        REGISTER_OP_HANDLER(OP_I64_EXTEND_I32_S);
        REGISTER_OP_HANDLER(OP_I64_EXTEND_I32_U);
        REGISTER_OP_HANDLER(OP_I64_TRUNC_F32_S);
        REGISTER_OP_HANDLER(OP_I64_TRUNC_F32_U);
        REGISTER_OP_HANDLER(OP_I64_TRUNC_F64_S);
        REGISTER_OP_HANDLER(OP_I64_TRUNC_F64_U);
        REGISTER_OP_HANDLER(OP_F32_CONVERT_I32_S);
        REGISTER_OP_HANDLER(OP_F32_CONVERT_I32_U);
        REGISTER_OP_HANDLER(OP_F32_CONVERT_I64_S);
        REGISTER_OP_HANDLER(OP_F32_CONVERT_I64_U);
        REGISTER_OP_HANDLER(OP_F32_DEMOTE_F64);
        REGISTER_OP_HANDLER(OP_F64_CONVERT_I32_S);
        REGISTER_OP_HANDLER(OP_F64_CONVERT_I32_U);
        REGISTER_OP_HANDLER(OP_F64_CONVERT_I64_S);
        REGISTER_OP_HANDLER(OP_F64_CONVERT_I64_U);
        REGISTER_OP_HANDLER(OP_F64_PROMOTE_F32);
        REGISTER_OP_HANDLER(OP_I32_REINTERPRET_F32);
        REGISTER_OP_HANDLER(OP_I64_REINTERPRET_F64);
        REGISTER_OP_HANDLER(OP_F32_REINTERPRET_I32);
        REGISTER_OP_HANDLER(OP_F64_REINTERPRET_I64);

        REGISTER_OP_HANDLER(OP_I32_LOAD);
        REGISTER_OP_HANDLER(OP_I64_LOAD);
        REGISTER_OP_HANDLER(OP_F32_LOAD);
        REGISTER_OP_HANDLER(OP_F64_LOAD);
        REGISTER_OP_HANDLER(OP_I32_LOAD8_S);
        REGISTER_OP_HANDLER(OP_I32_LOAD8_U);
        REGISTER_OP_HANDLER(OP_I32_LOAD16_S);
        REGISTER_OP_HANDLER(OP_I32_LOAD16_U);
        REGISTER_OP_HANDLER(OP_I64_LOAD8_S);
        REGISTER_OP_HANDLER(OP_I64_LOAD8_U);
        REGISTER_OP_HANDLER(OP_I64_LOAD16_S);
        REGISTER_OP_HANDLER(OP_I64_LOAD16_U);
        REGISTER_OP_HANDLER(OP_I64_LOAD32_S);
        REGISTER_OP_HANDLER(OP_I64_LOAD32_U);
        REGISTER_OP_HANDLER(OP_I32_STORE);
        REGISTER_OP_HANDLER(OP_I64_STORE);
        REGISTER_OP_HANDLER(OP_F32_STORE);
        REGISTER_OP_HANDLER(OP_F64_STORE);
        REGISTER_OP_HANDLER(OP_I32_STORE8);
        REGISTER_OP_HANDLER(OP_I32_STORE16);
        REGISTER_OP_HANDLER(OP_I64_STORE8);
        REGISTER_OP_HANDLER(OP_I64_STORE16);
        REGISTER_OP_HANDLER(OP_I64_STORE32);
        REGISTER_OP_HANDLER(OP_MEMORY_SIZE);
        REGISTER_OP_HANDLER(OP_MEMORY_GROW);

        REGISTER_OP_HANDLER(OP_DROP);
        REGISTER_OP_HANDLER(OP_SELECT);

        REGISTER_OP_HANDLER(OP_NOP);
        REGISTER_OP_HANDLER(OP_UNREACHABLE);
        REGISTER_OP_HANDLER(OP_BLOCK);
        REGISTER_OP_HANDLER(OP_LOOP);
        REGISTER_OP_HANDLER(OP_IF);
        REGISTER_OP_HANDLER(OP_BR);
        REGISTER_OP_HANDLER(OP_BR_IF);
        REGISTER_OP_HANDLER(OP_BR_TABLE);
        REGISTER_OP_HANDLER(OP_RETURN);
        REGISTER_OP_HANDLER(OP_CALL);
        REGISTER_OP_HANDLER(OP_CALL_INDIRECT);

        default:
            THROW_EXCEPTION_WITH_MSG(EXCEPTION_INTERPRETER_INVALID_INSTRUCTION, "opcode %s (0x%x) not implemented",
                                     opcode2str(instr->opcode), instr->opcode);
    }
}

#undef REGISTER_OP_HANDLER
#undef HAS_OP_HANDLER

#endif //WASM_INTERPRETER_OPCODE_H