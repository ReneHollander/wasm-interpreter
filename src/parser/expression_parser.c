#include "expression_parser.h"
#include "../instruction.h"

static inline void parse_instruction(instruction_t *instruction);

static inline instruction_t *allocate_instruction(vec_instruction_t **vec) {
    (*vec)->length = (*vec)->length + 1;
    *vec = realloc(*vec, (sizeof(vec_instruction_t) + (sizeof(instruction_t) * (*vec)->length)));
    return &((*vec)->values[(*vec)->length - 1]);
}

static inline void parse_memarg(memarg_t *memarg) {
    memarg->align = next_u32();
    memarg->offset = next_u32();
}

static inline void parse_insn_br_table(insn_br_table_t *table) {
    table->labels = next_vec_labelidx();
    table->default_label = next_labelidx();
}

static inline valtype_t parse_valtype() {
    switch (next_byte()) {
        case 0x7F:
            return valtype_i32;
        case 0x7E:
            return valtype_i64;
        case 0x7D:
            return valtype_f32;
        case 0x7C:
            return valtype_f64;
        default:
            parse_error("unexpected valtype");
            __builtin_unreachable();
    }
}

static inline void parse_blocktype(blocktype_t *blocktype) {
    if (peek_byte() == 0x40) {
        next_byte();
        blocktype->empty = true;
    } else {
        blocktype->type = parse_valtype();
    }
}

static inline void parse_insn_block(insn_block_t *block) {
    parse_blocktype(&block->resulttype);
    block->instructions = malloc(sizeof(vec_instruction_t));
    while (peek_byte() != 0x0B) {
        instruction_t *allocated = allocate_instruction(&block->instructions);
        parse_instruction(allocated);
    }
    next_byte();
}

static inline void parse_insn_if(insn_if_t *if_block) {
    parse_blocktype(&if_block->resulttype);
    if_block->ifpath = malloc(sizeof(vec_instruction_t));
    while (peek_byte() != 0x05 && peek_byte() != 0x0B) {
        instruction_t *allocated = allocate_instruction(&if_block->ifpath);
        parse_instruction(allocated);
    }
    if (next_byte() == 0x05) {
        if_block->elsepath = malloc(sizeof(vec_instruction_t));
        while (peek_byte() != 0x0B) {
            instruction_t *allocated = allocate_instruction(&if_block->elsepath);
            parse_instruction(allocated);
        }
    } else {
        if_block->elsepath = NULL;
    }
    next_byte();
}

static inline void parse_instruction(instruction_t *instruction) {
    instruction->opcode = next_byte();

    switch (instruction->opcode) {
        case OP_BLOCK:
        case OP_LOOP:
            parse_insn_block(&instruction->block);
            break;
        case OP_IF:
            parse_insn_if(&instruction->if_block);
            break;
        case OP_BR:
        case OP_BR_IF:
            instruction->labelidx = next_labelidx();
            break;
        case OP_BR_TABLE:
            parse_insn_br_table(&instruction->table);
            break;
        case OP_CALL:
            instruction->funcidx = next_funcidx();
            break;
        case OP_CALL_INDIRECT:
            instruction->typeidx = next_typeidx();
            if (next_byte() != 0x00) parse_error("invalid tableidx for call indirect");
            break;
        case OP_LOCAL_GET:
        case OP_LOCAL_SET:
        case OP_LOCAL_TEE:
            instruction->localidx = next_localidx();
            break;
        case OP_GLOBAL_GET:
        case OP_GLOBAL_SET:
            instruction->globalidx = next_globalidx();
            break;
        case OP_I32_LOAD :
        case OP_I64_LOAD:
        case OP_F32_LOAD :
        case OP_F64_LOAD :
        case OP_I32_LOAD8_S :
        case OP_I32_LOAD8_U :
        case OP_I32_LOAD16_S:
        case OP_I32_LOAD16_U :
        case OP_I64_LOAD8_S:
        case OP_I64_LOAD8_U :
        case OP_I64_LOAD16_S :
        case OP_I64_LOAD16_U :
        case OP_I64_LOAD32_S :
        case OP_I64_LOAD32_U :
        case OP_I32_STORE:
        case OP_I64_STORE :
        case OP_F32_STORE :
        case OP_F64_STORE:
        case OP_I32_STORE8 :
        case OP_I32_STORE16:
        case OP_I64_STORE8 :
        case OP_I64_STORE16 :
        case OP_I64_STORE32 :
            parse_memarg(&instruction->memarg);
            break;
        case OP_MEMORY_SIZE:
            if (next_byte() != 0x00) parse_error("unknown index for memory.size");
            break;
        case OP_MEMORY_GROW:
            if (next_byte() != 0x00) parse_error("unknown index for memory.grow");
            break;
        case OP_I32_CONST:
            instruction->const_i32 = next_i32();
            break;
        case OP_I64_CONST:
            instruction->const_i64 = next_i64();
            break;
        case OP_F32_CONST:
            instruction->const_f32 = next_f32();
            break;
        case OP_F64_CONST:
            instruction->const_f64 = next_f64();
            break;
        case OP_UNREACHABLE:
        case OP_NOP:
        case OP_RETURN:
        case OP_DROP:
        case OP_SELECT:
        case OP_I32_EQZ:
        case OP_I32_EQ:
        case OP_I32_NE:
        case OP_I32_LT_S:
        case OP_I32_LT_U:
        case OP_I32_GT_S:
        case OP_I32_GT_U:
        case OP_I32_LE_S:
        case OP_I32_LE_U:
        case OP_I32_GE_S:
        case OP_I32_GE_U:
        case OP_I64_EQZ:
        case OP_I64_EQ:
        case OP_I64_NE:
        case OP_I64_LT_S:
        case OP_I64_LT_U:
        case OP_I64_GT_S:
        case OP_I64_GT_U:
        case OP_I64_LE_S:
        case OP_I64_LE_U:
        case OP_I64_GE_S:
        case OP_I64_GE_U:
        case OP_F32_EQ:
        case OP_F32_NE:
        case OP_F32_LT:
        case OP_F32_GT:
        case OP_F32_LE:
        case OP_F32_GE:
        case OP_F64_EQ:
        case OP_F64_NE:
        case OP_F64_LT:
        case OP_F64_GT:
        case OP_F64_LE:
        case OP_F64_GE:
        case OP_I32_CLZ:
        case OP_I32_CTZ:
        case OP_I32_POPCNT:
        case OP_I32_ADD:
        case OP_I32_SUB:
        case OP_I32_MUL:
        case OP_I32_DIV_S:
        case OP_I32_DIV_U:
        case OP_I32_REM_S:
        case OP_I32_REM_U:
        case OP_I32_AND:
        case OP_I32_OR:
        case OP_I32_XOR:
        case OP_I32_SHL:
        case OP_I32_SHR_U:
        case OP_I32_SHR_S:
        case OP_I32_ROTL:
        case OP_I32_ROTR:
        case OP_I64_CLZ:
        case OP_I64_CTZ:
        case OP_I64_POPCNT:
        case OP_I64_ADD:
        case OP_I64_SUB:
        case OP_I64_MUL:
        case OP_I64_DIV_S:
        case OP_I64_DIV_U:
        case OP_I64_REM_S:
        case OP_I64_REM_U:
        case OP_I64_AND:
        case OP_I64_OR:
        case OP_I64_XOR:
        case OP_I64_SHL:
        case OP_I64_SHR_U:
        case OP_I64_SHR_S:
        case OP_I64_ROTL:
        case OP_I64_ROTR:
        case OP_F32_ABS:
        case OP_F32_NEG:
        case OP_F32_CEIL:
        case OP_F32_FLOOR:
        case OP_F32_TRUNC:
        case OP_F32_NEAREST:
        case OP_F32_SQRT:
        case OP_F32_ADD:
        case OP_F32_SUB:
        case OP_F32_MUL:
        case OP_F32_DIV:
        case OP_F32_MIN:
        case OP_F32_MAX:
        case OP_F32_COPYSIGN:
        case OP_F64_ABS:
        case OP_F64_NEG:
        case OP_F64_CEIL:
        case OP_F64_FLOOR:
        case OP_F64_TRUNC:
        case OP_F64_NEAREST:
        case OP_F64_SQRT:
        case OP_F64_ADD:
        case OP_F64_SUB:
        case OP_F64_MUL:
        case OP_F64_DIV:
        case OP_F64_MIN:
        case OP_F64_MAX:
        case OP_F64_COPYSIGN:
        case OP_I32_WRAP_I64:
        case OP_I32_TRUNC_F32_S:
        case OP_I32_TRUNC_F32_U:
        case OP_I32_TRUNC_F64_S:
        case OP_I32_TRUNC_F64_U:
        case OP_I64_EXTEND_I32_S:
        case OP_I64_EXTEND_I32_U:
        case OP_I64_TRUNC_F32_S:
        case OP_I64_TRUNC_F32_U:
        case OP_I64_TRUNC_F64_S:
        case OP_I64_TRUNC_F64_U:
        case OP_F32_CONVERT_I32_S:
        case OP_F32_CONVERT_I32_U:
        case OP_F32_CONVERT_I64_S:
        case OP_F32_CONVERT_I64_U:
        case OP_F32_DEMOTE_F64:
        case OP_F64_CONVERT_I32_S:
        case OP_F64_CONVERT_I32_U:
        case OP_F64_CONVERT_I64_S:
        case OP_F64_CONVERT_I64_U:
        case OP_F64_PROMOTE_F32:
        case OP_I32_REINTERPRET_F32:
        case OP_I64_REINTERPRET_F64:
        case OP_F32_REINTERPRET_F32:
        case OP_F64_REINTERPRET_I64:
            break;
        default:
            parse_error("unknown opcode");
    }
}

void parse_expression(expression_t *expression) {
    expression->instructions = malloc(sizeof(vec_instruction_t));
    while (peek_byte() != 0x0B) {
        instruction_t *allocated = allocate_instruction(&expression->instructions);
        parse_instruction(allocated);
    }
    next_byte();
}
