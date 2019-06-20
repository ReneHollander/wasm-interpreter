#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "type.h"

typedef enum opcode {
    OP_UNREACHABLE = 0x00,
    OP_NOP = 0x01,
    OP_BLOCK = 0x02,
    OP_LOOP = 0x03,
    OP_IF = 0x04,
    OP_ELSE = 0x05,
    OP_BR = 0x0C,
    OP_BR_IF = 0x0D,
    OP_BR_TABLE = 0x0E,
    OP_RETURN = 0x0F,
    OP_CALL = 0x10,
    OP_CALL_INDIRECT = 0x11,

    OP_DROP = 0x1A,
    OP_SELECT = 0x1B,

    OP_LOCAL_GET = 0x20,
    OP_LOCAL_SET = 0x21,
    OP_LOCAL_TEE = 0x22,
    OP_GLOBAL_GET = 0x23,
    OP_GLOBAL_SET = 0x24,

    OP_I32_LOAD = 0x28,
    OP_I64_LOAD = 0x29,
    OP_F32_LOAD = 0x2A,
    OP_F64_LOAD = 0x2B,
    OP_I32_LOAD8_S = 0x2C,
    OP_I32_LOAD8_U = 0x2D,
    OP_I32_LOAD16_S = 0x2E,
    OP_I32_LOAD16_U = 0x2F,
    OP_I64_LOAD8_S = 0x30,
    OP_I64_LOAD8_U = 0x31,
    OP_I64_LOAD16_S = 0x32,
    OP_I64_LOAD16_U = 0x33,
    OP_I64_LOAD32_S = 0x34,
    OP_I64_LOAD32_U = 0x35,
    OP_I32_STORE = 0x36,
    OP_I64_STORE = 0x37,
    OP_F32_STORE = 0x38,
    OP_F64_STORE = 0x39,
    OP_I32_STORE8 = 0x3A,
    OP_I32_STORE16 = 0x3B,
    OP_I64_STORE8 = 0x3C,
    OP_I64_STORE16 = 0x3D,
    OP_I64_STORE32 = 0x3E,
    OP_MEMORY_SIZE = 0x3F,
    OP_MEMORY_GROW = 0x40,

    OP_I32_CONST = 0x41,
    OP_I64_CONST = 0x42,
    OP_F32_CONST = 0x43,
    OP_F64_CONST = 0x44,

    OP_I32_EQZ = 0x45,
    OP_I32_EQ = 0x46,
    OP_I32_NE = 0x47,
    OP_I32_LT_S = 0x48,
    OP_I32_LT_U = 0x49,
    OP_I32_GT_S = 0x4A,
    OP_I32_GT_U = 0x4B,
    OP_I32_LE_S = 0x4C,
    OP_I32_LE_U = 0x4D,
    OP_I32_GE_S = 0x4E,
    OP_I32_GE_U = 0x4F,

    OP_I64_EQZ = 0x50,
    OP_I64_EQ = 0x51,
    OP_I64_NE = 0x52,
    OP_I64_LT_S = 0x53,
    OP_I64_LT_U = 0x54,
    OP_I64_GT_S = 0x55,
    OP_I64_GT_U = 0x56,
    OP_I64_LE_S = 0x57,
    OP_I64_LE_U = 0x58,
    OP_I64_GE_S = 0x59,
    OP_I64_GE_U = 0x5A,

    OP_F32_EQ = 0x5B,
    OP_F32_NE = 0x5C,
    OP_F32_LT = 0x5D,
    OP_F32_GT = 0x5E,
    OP_F32_LE = 0x5F,
    OP_F32_GE = 0x60,

    OP_F64_EQ = 0x61,
    OP_F64_NE = 0x62,
    OP_F64_LT = 0x63,
    OP_F64_GT = 0x64,
    OP_F64_LE = 0x65,
    OP_F64_GE = 0x66,

    OP_I32_CLZ = 0x67,
    OP_I32_CTZ = 0x68,
    OP_I32_POPCNT = 0x69,
    OP_I32_ADD = 0x6A,
    OP_I32_SUB = 0x6B,
    OP_I32_MUL = 0x6C,
    OP_I32_DIV_S = 0x6D,
    OP_I32_DIV_U = 0x6E,
    OP_I32_REM_S = 0x6F,
    OP_I32_REM_U = 0x70,
    OP_I32_AND = 0x71,
    OP_I32_OR = 0x72,
    OP_I32_XOR = 0x73,
    OP_I32_SHL = 0x74,
    OP_I32_SHR_S = 0x75,
    OP_I32_SHR_U = 0x76,
    OP_I32_ROTL = 0x77,
    OP_I32_ROTR = 0x78,

    OP_I64_CLZ = 0x79,
    OP_I64_CTZ = 0x7A,
    OP_I64_POPCNT = 0x7B,
    OP_I64_ADD = 0x7C,
    OP_I64_SUB = 0x7D,
    OP_I64_MUL = 0x7E,
    OP_I64_DIV_S = 0x7F,
    OP_I64_DIV_U = 0x80,
    OP_I64_REM_S = 0x81,
    OP_I64_REM_U = 0x82,
    OP_I64_AND = 0x83,
    OP_I64_OR = 0x84,
    OP_I64_XOR = 0x85,
    OP_I64_SHL = 0x86,
    OP_I64_SHR_S = 0x87,
    OP_I64_SHR_U = 0x88,
    OP_I64_ROTL = 0x89,
    OP_I64_ROTR = 0x8A,

    OP_F32_ABS = 0x8B,
    OP_F32_NEG = 0x8C,
    OP_F32_CEIL = 0x8D,
    OP_F32_FLOOR = 0x8E,
    OP_F32_TRUNC = 0x8F,
    OP_F32_NEAREST = 0x90,
    OP_F32_SQRT = 0x91,
    OP_F32_ADD = 0x92,
    OP_F32_SUB = 0x93,
    OP_F32_MUL = 0x94,
    OP_F32_DIV = 0x95,
    OP_F32_MIN = 0x96,
    OP_F32_MAX = 0x97,
    OP_F32_COPYSIGN = 0x98,

    OP_F64_ABS = 0x99,
    OP_F64_NEG = 0x9A,
    OP_F64_CEIL = 0x9B,
    OP_F64_FLOOR = 0x9C,
    OP_F64_TRUNC = 0x9D,
    OP_F64_NEAREST = 0x9E,
    OP_F64_SQRT = 0x9F,
    OP_F64_ADD = 0xA0,
    OP_F64_SUB = 0xA1,
    OP_F64_MUL = 0xA2,
    OP_F64_DIV = 0xA3,
    OP_F64_MIN = 0xA4,
    OP_F64_MAX = 0xA5,
    OP_F64_COPYSIGN = 0xA6,

    OP_I32_WRAP_I64 = 0xA7,
    OP_I32_TRUNC_F32_S = 0xA8,
    OP_I32_TRUNC_F32_U = 0xA9,
    OP_I32_TRUNC_F64_S = 0xAA,
    OP_I32_TRUNC_F64_U = 0xAB,
    OP_I64_EXTEND_I32_S = 0xAC,
    OP_I64_EXTEND_I32_U = 0xAD,
    OP_I64_TRUNC_F32_S = 0xAE,
    OP_I64_TRUNC_F32_U = 0xAF,
    OP_I64_TRUNC_F64_S = 0xB0,
    OP_I64_TRUNC_F64_U = 0xB1,
    OP_F32_CONVERT_I32_S = 0xB2,
    OP_F32_CONVERT_I32_U = 0xB3,
    OP_F32_CONVERT_I64_S = 0xB4,
    OP_F32_CONVERT_I64_U = 0xB5,
    OP_F32_DEMOTE_F64 = 0xB6,
    OP_F64_CONVERT_I32_S = 0xB7,
    OP_F64_CONVERT_I32_U = 0xB8,
    OP_F64_CONVERT_I64_S = 0xB9,
    OP_F64_CONVERT_I64_U = 0xBA,
    OP_F64_PROMOTE_F32 = 0xBB,
    OP_I32_REINTERPRET_F32 = 0xBC,
    OP_I64_REINTERPRET_F64 = 0xBD,
    OP_F32_REINTERPRET_I32 = 0xBE,
    OP_F64_REINTERPRET_I64 = 0xBF,
} opcode_t;

typedef struct instruction instruction_t;
typedef struct vec_instruction vec_instruction_t;

typedef struct insn_block {
    blocktype_t resulttype;
    vec_instruction_t *instructions;
} insn_block_t;

typedef struct insn_if {
    blocktype_t resulttype;
    vec_instruction_t *ifpath;
    vec_instruction_t *elsepath; // Can be NULL to signify empty else path.
} insn_if_t;

typedef struct insn_br_table {
    vec_labelidx_t *labels; // Labels that can be used as jump targets.
    labelidx default_label; // Default label to jump to when index out of bounds in labels.
} insn_br_table_t;

typedef struct memarg {
    u32 align;
    u32 offset;
} memarg_t;

struct instruction {
    union {
        insn_block_t block; // Used by OP_BLOCK and OP_LOOP.
        insn_if_t if_block; // Used by OP_IF and OP_ELSE.
        labelidx labelidx; // Used by OP_BR and OP_BR_IF.
        insn_br_table_t table; // Used by OP_TABLE.
        funcidx funcidx; // Used by OP_CALL.
        typeidx typeidx; // Used by OP_CALL_INDIRECT.
        localidx localidx; // Used BY OP_LOCAL_GET, OP_LOCAL_SET, OP_LOCAL_TEE.
        globalidx globalidx; // Used BY OP_GLOBAL_GET, OP_GLOBAL_SET.
        memarg_t memarg; // Used by OP_*_LOAD*, OP_*_STORE*.
        i32 const_i32; // Used by OP_I32_CONST.
        i64 const_i64; // Used by OP_I64_CONST.
        f32 const_f32; // Used by OP_F32_CONST.
        f64 const_f64; // Used by OP_F64_CONST.
    };
    opcode_t opcode;
};

struct vec_instruction {
    u32 length;
    instruction_t values[0];
};

typedef struct expression {
    vec_instruction_t *instructions;
} expression_t;

#endif // INSTRUCTION_H
