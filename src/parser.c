#include "parser.h"

#include "type.h"
#include "value.h"
#include "instruction.h"
#include "stdlib.h"
#include "string.h"
#include "strings.h"

typedef struct parser_state {
    FILE *input;
} parser_state_t;

#define MAKE_NEXT_VEC(name, generator_func) \
static inline CAT(CAT(vec_, name), _t) *CAT(CAT(next_, name), _vec)(parser_state_t *state) { \
    u32 n = next_u32(state); \
    CAT(CAT(vec_, name), _t) *vec = CAT(CAT(vec_, name), _create)(); \
    for (int i = 0; i < n; i++) { \
        CAT(CAT(vec_, name), _add)(vec, generator_func(state)); \
    } \
    return vec; \
}

static inline byte next_byte(parser_state_t *state) {
    int value = fgetc(state->input);
    if (value == EOF) {
        THROW_EXCEPTION(EXCEPTION_PARSER_EOF_BEFORE_FINISHED);
    }
    return value;
}

static inline byte peek_byte(parser_state_t *state) {
    int value = fgetc(state->input);
    if (value == EOF) {
        THROW_EXCEPTION(EXCEPTION_PARSER_EOF_BEFORE_FINISHED);
    }
    ungetc(value, state->input);
    return value;
}

static void advance(parser_state_t *state, u32 count) {
    for (u32 i = 0; i < count; i++) {
        next_byte(state);
    }
}

static inline uint64_t read_LEB(parser_state_t *state, uint32_t maxbits, bool sign) {
    uint64_t result = 0;
    uint32_t shift = 0;
    uint32_t bcnt = 0;
    uint64_t byte;

    while (true) {
        byte = next_byte(state);
        result |= ((byte & 0x7f) << shift);
        shift += 7;
        if ((byte & 0x80) == 0) {
            break;
        }
        bcnt += 1;
        if (bcnt > (maxbits + 7 - 1) / 7) {
            THROW_EXCEPTION(EXCEPTION_PARSER_LEB_OVERFLOW);
        }
    }
    if (sign && (shift < maxbits) && (byte & 0x40)) {
        // Sign extend
        result |= -(1 << shift);
    }
    return result;
}

static inline u32 next_u32(parser_state_t *state) {
    return read_LEB(state, 32, false);
}

static inline s32 next_s32(parser_state_t *state) {
    return read_LEB(state, 32, true);
}

static inline s64 next_s64(parser_state_t *state) {
    return read_LEB(state, 64, true);
}

static inline i32 next_i32(parser_state_t *state) {
    return next_s32(state);
}

static inline i64 next_i64(parser_state_t *state) {
    return next_s64(state);
}

static inline f32 next_f32(parser_state_t *state) {
    f32 output;
    *((byte *) (&output) + 0) = next_byte(state);
    *((byte *) (&output) + 1) = next_byte(state);
    *((byte *) (&output) + 2) = next_byte(state);
    *((byte *) (&output) + 3) = next_byte(state);
    return output;
}

static inline f64 next_f64(parser_state_t *state) {
    f64 output;
    *((byte *) (&output) + 0) = next_byte(state);
    *((byte *) (&output) + 1) = next_byte(state);
    *((byte *) (&output) + 2) = next_byte(state);
    *((byte *) (&output) + 3) = next_byte(state);
    *((byte *) (&output) + 4) = next_byte(state);
    *((byte *) (&output) + 5) = next_byte(state);
    *((byte *) (&output) + 6) = next_byte(state);
    *((byte *) (&output) + 7) = next_byte(state);
    return output;
}

static inline typeidx next_typeidx(parser_state_t *state) {
    return next_u32(state);
}

static inline funcidx next_funcidx(parser_state_t *state) {
    return next_u32(state);
}

static inline tableidx next_tableidx(parser_state_t *state) {
    return next_u32(state);
}

static inline memidx next_memidx(parser_state_t *state) {
    return next_u32(state);
}

static inline globalidx next_globalidx(parser_state_t *state) {
    return next_u32(state);
}

static inline localidx next_localidx(parser_state_t *state) {
    return next_u32(state);
}

static inline labelidx next_labelidx(parser_state_t *state) {
    return next_u32(state);
}

MAKE_NEXT_VEC(byte, next_byte)

MAKE_NEXT_VEC(typeidx, next_typeidx)

MAKE_NEXT_VEC(funcidx, next_funcidx)

MAKE_NEXT_VEC(labelidx, next_labelidx)

static inline name next_name(parser_state_t *state) {
    // TODO: Should not use internal vec API.
    vec_byte_t *vec = next_byte_vec(state);
    name name = malloc(sizeof(char) * (vec->_length + 1));
    memcpy(name, vec->_elements, sizeof(char) * vec->_length);
    name[vec->_length] = '\0';
    vec_byte_free(vec);
    return name;
}

static inline instruction_t next_instruction(parser_state_t *state);

static inline memarg_t next_memarg(parser_state_t *state) {
    memarg_t memarg;
    memarg.align = next_u32(state);
    memarg.offset = next_u32(state);
    return memarg;
}

static inline insn_br_table_t next_insn_br_table(parser_state_t *state) {
    insn_br_table_t table;
    table.labels = next_labelidx_vec(state);
    table.default_label = next_labelidx(state);
    return table;
}

static inline valtype_t next_valtype(parser_state_t *state) {
    switch (next_byte(state)) {
        case 0x7F:
            return VALTYPE_I32;
        case 0x7E:
            return VALTYPE_I64;
        case 0x7D:
            return VALTYPE_F32;
        case 0x7C:
            return VALTYPE_F64;
        default:
            THROW_EXCEPTION(EXCEPTION_PARSER_UNEXPECTED_VALTYPE);
    }
}

MAKE_NEXT_VEC(valtype, next_valtype)

static inline blocktype_t next_blocktype(parser_state_t *state) {
    blocktype_t blocktype;
    if (peek_byte(state) == 0x40) {
        next_byte(state);
        blocktype.empty = true;
    } else {
        blocktype.type = next_valtype(state);
    }
    return blocktype;
}

static inline insn_block_t next_insn_block(parser_state_t *state) {
    insn_block_t block;
    block.resulttype = next_blocktype(state);
    block.instructions = vec_instruction_create();
    while (peek_byte(state) != 0x0B) {
        vec_instruction_add(block.instructions, next_instruction(state));
    }
    next_byte(state);
    return block;
}

static inline insn_if_t next_insn_if(parser_state_t *state) {
    insn_if_t if_block;
    if_block.resulttype = next_blocktype(state);
    if_block.ifpath = vec_instruction_create();
    while (peek_byte(state) != OP_ELSE && peek_byte(state) != 0x0B) {
        vec_instruction_add(if_block.ifpath, next_instruction(state));
    }
    if (peek_byte(state) == OP_ELSE) {
        next_byte(state);
        if_block.elsepath = vec_instruction_create();
        while (peek_byte(state) != 0x0B) {
            vec_instruction_add(if_block.elsepath, next_instruction(state));
        }
    } else {
        if_block.elsepath = NULL;
    }
    next_byte(state);
    return if_block;
}

static inline instruction_t next_instruction(parser_state_t *state) {
    instruction_t instruction;
    instruction.opcode = next_byte(state);
    switch (instruction.opcode) {
        case OP_BLOCK:
        case OP_LOOP:
            instruction.block = next_insn_block(state);
            break;
        case OP_IF:
            instruction.if_block = next_insn_if(state);
            break;
        case OP_BR:
        case OP_BR_IF:
            instruction.labelidx = next_labelidx(state);
            break;
        case OP_BR_TABLE:
            instruction.table = next_insn_br_table(state);
            break;
        case OP_CALL:
            instruction.funcidx = next_funcidx(state);
            break;
        case OP_CALL_INDIRECT:
            instruction.typeidx = next_typeidx(state);
            if (next_byte(state) != 0x00) THROW_EXCEPTION(EXCEPTION_PARSER_INVALID_TYPEIDX_FOR_CALL_INDIRECT);
            break;
        case OP_LOCAL_GET:
        case OP_LOCAL_SET:
        case OP_LOCAL_TEE:
            instruction.localidx = next_localidx(state);
            break;
        case OP_GLOBAL_GET:
        case OP_GLOBAL_SET:
            instruction.globalidx = next_globalidx(state);
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
            instruction.memarg = next_memarg(state);
            break;
        case OP_MEMORY_SIZE:
            if (next_byte(state) != 0x00) THROW_EXCEPTION(EXCEPTION_PARSER_MEMORY_INDEX_NOT_ZERO);
            break;
        case OP_MEMORY_GROW:
            if (next_byte(state) != 0x00) THROW_EXCEPTION(EXCEPTION_PARSER_MEMORY_INDEX_NOT_ZERO);
            break;
        case OP_I32_CONST:
            instruction.const_i32 = next_i32(state);
            break;
        case OP_I64_CONST:
            instruction.const_i64 = next_i64(state);
            break;
        case OP_F32_CONST:
            instruction.const_f32 = next_f32(state);
            break;
        case OP_F64_CONST:
            instruction.const_f64 = next_f64(state);
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
        case OP_F32_REINTERPRET_I32:
        case OP_F64_REINTERPRET_I64:
            break;
        default:
            THROW_EXCEPTION(EXCEPTION_PARSER_UNKNOWN_OPCODE);
    }
    return instruction;
}

expression_t next_expression(parser_state_t *state) {
    expression_t expression;
    expression.instructions = vec_instruction_create();
    while (peek_byte(state) != 0x0B) {
        vec_instruction_add(expression.instructions, next_instruction(state));
    }
    next_byte(state);
    return expression;
}

static inline limits_t next_limit(parser_state_t *state) {
    limits_t limits;
    switch (next_byte(state)) {
        case 0x00:
            limits.has_max = false;
            limits.min = next_u32(state);
            break;
        case 0x01:
            limits.has_max = true;
            limits.min = next_u32(state);
            limits.max = next_u32(state);
            break;
        default:
            THROW_EXCEPTION(EXCEPTION_PARSER_UNKNOWN_LIMIT_TYPE);
    }
    return limits;
}

static inline functype_t next_functype(parser_state_t *state) {
    functype_t functype;
    byte opcode = next_byte(state);
    if (opcode != 0x60) THROW_EXCEPTION(EXCEPTION_PARSER_UNEXPECTED_OPCODE);
    functype.t1 = next_valtype_vec(state);
    functype.t2 = next_valtype_vec(state);
    return functype;
}

MAKE_NEXT_VEC(functype, next_functype)

static type_section_t next_type_section(parser_state_t *state) {
    type_section_t type_section;
    type_section.types = next_functype_vec(state);
    return type_section;
}

static inline tabletype_t next_tabletype(parser_state_t *state) {
    tabletype_t tabletype;
    if (next_byte(state) != 0x70) THROW_EXCEPTION(EXCEPTION_PARSER_UNKNOWN_ELEMENT_TYPE);
    tabletype.lim = next_limit(state);
    return tabletype;
}

MAKE_NEXT_VEC(tabletype, next_tabletype)

static inline mutability_t next_mutability(parser_state_t *state) {
    switch (next_byte(state)) {
        case 0x00:
            return MUTABILITY_CONST;
        case 0x01:
            return MUTABILITY_VAR;
        default:
            THROW_EXCEPTION(EXCEPTION_PARSER_UNKNOWN_MUTABILITY_TYPE);
    }
}

static inline globaltype_t next_globaltype(parser_state_t *state) {
    globaltype_t globaltype;
    globaltype.t = next_valtype(state);
    globaltype.m = next_mutability(state);
    return globaltype;
}

static inline memtype_t next_memtype(parser_state_t *state) {
    memtype_t memtype;
    memtype.lim = next_limit(state);
    return memtype;
}

MAKE_NEXT_VEC(memtype, next_memtype)

static inline import_t next_import(parser_state_t *state) {
    import_t import;
    import.module = next_name(state);
    import.name = next_name(state);
    import.desc = next_byte(state);

    switch (import.desc) {
        case IMPORTDESC_FUNC:
            import.func = next_typeidx(state);
            break;
        case IMPORTDESC_TABLE:
            import.table = next_tabletype(state);
            break;
        case IMPORTDESC_MEM:
            import.mem = next_memtype(state);
            break;
        case IMPORTDESC_GLOBAL:
            import.global = next_globaltype(state);
            break;
        default:
            THROW_EXCEPTION(EXCEPTION_PARSER_UNKNOWN_IMPORTDESC_TYPE);
    }
    return import;
}

MAKE_NEXT_VEC(import, next_import)

static import_section_t next_import_section(parser_state_t *state) {
    import_section_t import_section;
    import_section.imports = next_import_vec(state);
    return import_section;
}

static function_section_t next_function_section(parser_state_t *state) {
    function_section_t function_section;
    function_section.functions = next_typeidx_vec(state);
    return function_section;
}

static table_section_t next_table_section(parser_state_t *state) {
    table_section_t table_section;
    table_section.tables = next_tabletype_vec(state);
    return table_section;
}

static memory_section_t next_memory_section(parser_state_t *state) {
    memory_section_t memory_section;
    memory_section.memories = next_memtype_vec(state);
    return memory_section;
}

static inline global_t next_global(parser_state_t *state) {
    global_t global;
    global.gt = next_globaltype(state);
    global.e = next_expression(state);
    return global;
}

MAKE_NEXT_VEC(global, next_global)

static global_section_t next_global_section(parser_state_t *state) {
    global_section_t global_section;
    global_section.globals = next_global_vec(state);
    return global_section;
}

static inline export_t next_export(parser_state_t *state) {
    export_t export;
    export.name = next_name(state);
    export.desc = next_byte(state);

    switch (export.desc) {
        case EXPORTDESC_FUNC:
            export.func = next_funcidx(state);
            break;
        case EXPORTDESC_TABLE:
            export.table = next_tableidx(state);
            break;
        case EXPORTDESC_MEM:
            export.mem = next_memidx(state);
            break;
        case EXPORTDESC_GLOBAL:
            export.global = next_globalidx(state);
            break;
        default:
            THROW_EXCEPTION(EXCEPTION_PARSER_UNKNOWN_EXPORTDESC_TYPE);
    }
    return export;
}

MAKE_NEXT_VEC(export, next_export)

static export_section_t next_export_section(parser_state_t *state) {
    export_section_t export_section;
    export_section.exports = next_export_vec(state);
    return export_section;
}

static start_section_t next_start_section(parser_state_t *state) {
    start_section_t start_section;
    start_section.start = next_funcidx(state);
    return start_section;
}

static inline element_t next_element(parser_state_t *state) {
    element_t element;
    element.table = next_tableidx(state);
    element.offset = next_expression(state);
    element.init = next_funcidx_vec(state);
    return element;
}

MAKE_NEXT_VEC(element, next_element)

static element_section_t next_element_section(parser_state_t *state) {
    element_section_t element_section;
    element_section.elements = next_element_vec(state);
    return element_section;
}

static inline locals_t next_locals(parser_state_t *state) {
    locals_t locals;
    locals.n = next_u32(state);
    locals.t = next_valtype(state);
    return locals;
}

MAKE_NEXT_VEC(locals, next_locals)

static inline func_t next_func(parser_state_t *state) {
    func_t func;
    u32 size = next_u32(state);
    func.locals = next_locals_vec(state);
    func.expression = next_expression(state);
    return func;
}

MAKE_NEXT_VEC(func, next_func)

static code_section_t next_code_section(parser_state_t *state) {
    code_section_t code_section;
    code_section.funcs = next_func_vec(state);
    return code_section;
}

static inline data_t next_data(parser_state_t *state) {
    data_t data;
    data.memidx = next_memidx(state);
    data.expression = next_expression(state);
    data.init = next_byte_vec(state);
    return data;
}

MAKE_NEXT_VEC(data, next_data)

static data_section_t next_data_section(parser_state_t *state) {
    data_section_t data_section;
    data_section.datas = next_data_vec(state);
    return data_section;
}

static section_t next_section(parser_state_t *state) {
    section_t section;

    byte id = next_byte(state);
    if (id > 11) THROW_EXCEPTION(EXCEPTION_PARSER_INVALID_SECTION_ID);
    section.id = id;

    u32 size = next_u32(state);
    section.size = size;

    switch (section.id) {
        case SECTION_TYPE_CUSTOM:
            advance(state, section.size);
            break;
        case SECTION_TYPE_TYPE:
            section.type_section = next_type_section(state);
            break;
        case SECTION_TYPE_IMPORT:
            section.import_section = next_import_section(state);
            break;
        case SECTION_TYPE_FUNCTION:
            section.function_section = next_function_section(state);
            break;
        case SECTION_TYPE_TABLE:
            section.table_section = next_table_section(state);
            break;
        case SECTION_TYPE_MEMORY:
            section.memory_section = next_memory_section(state);
            break;
        case SECTION_TYPE_GLOBAL:
            section.global_section = next_global_section(state);
            break;
        case SECTION_TYPE_EXPORT:
            section.export_section = next_export_section(state);
            break;
        case SECTION_TYPE_START:
            section.start_section = next_start_section(state);
            break;
        case SECTION_TYPE_ELEMENT:
            section.element_section = next_element_section(state);
            break;
        case SECTION_TYPE_CODE:
            section.code_section = next_code_section(state);
            break;
        case SECTION_TYPE_DATA:
            section.data_section = next_data_section(state);
            break;
        default:
            THROW_EXCEPTION(EXCEPTION_PARSER_UNKNOWN_SECTION_TYPE);
    }
    return section;
}

static void handle_imports(module_t *module, parse_error_f parse_error) {
    if (module->imports != NULL) {
        uint32_t func_count = 0;
        uint32_t global_count = 0;

        vec_import_iterator_t it = vec_import_iterator(module->imports, IT_FORWARDS);
        while (vec_import_has_next(&it)) {
            import_t *import = vec_import_nextp(&it);
            if (import->desc == IMPORTDESC_FUNC) {
                func_count++;
            } else if (import->desc == IMPORTDESC_TABLE) {
                //do nothing for now
            } else if (import->desc == IMPORTDESC_GLOBAL) {
                global_count++;
            } else if (import->desc == IMPORTDESC_MEM) {
                //do nothing for now
            } else {
                parse_error("unknown import desc");
            }
        }
        // TODO: figure this out.
//        func_count += vec_func_length(module->funcs);
//        module->funcs = realloc(module->funcs, sizeof(vec_func_t) + (sizeof(func_t) * func_count));
//
//        for (int i = func_count - 1; i >= 0; i--) {
//            if (i - module->funcs->length >= 0) {
//                module->funcs->values[i] = module->funcs->values[i - module->funcs->length];
//            }
//        }
//
//        global_count += module->globals->length;
//        module->globals = realloc(module->globals, sizeof(vec_global_t) + sizeof(globaltype_t) * global_count);
//
//        for (int i = global_count - 1; i >= 0; i--) {
//            if (i - module->globals->length >= 0) {
//                module->globals->values[i] = module->globals->values[i - module->globals->length];
//            }
//        }
    }
}

exception_t parse(FILE *input_file, module_t **module) {
    TRY_CATCH(
            parser_state_t state;
            state.input = input_file;

            *module = calloc(1, sizeof(module_t));

            // Check magic value of module.
            if (next_byte(&state) != 0x00 || next_byte(&state) != 0x61 || next_byte(&state) != 0x73 ||
                next_byte(&state) != 0x6D) {
                THROW_EXCEPTION(EXCEPTION_PARSER_UNKNOWN_MAGIC_VALUE);
            }

            // Check version number of module.
            if (next_byte(&state) != 0x01 || next_byte(&state) != 0x00 || next_byte(&state) != 0x00 ||
                next_byte(&state) != 0x00) {
                THROW_EXCEPTION(EXCEPTION_PARSER_VERSION_NOT_SUPPORTED);
            }

            function_section_t function_section;

            while (1) {
                int next = fgetc(state.input);
                if (next == EOF) break;
                ungetc(next, state.input);

                section_t section = next_section(&state);

                switch (section.id) {
                    case SECTION_TYPE_TYPE:
                        (*module)->types = section.type_section.types;
                        break;
                    case SECTION_TYPE_CUSTOM: // Ignore.
                        break;
                    case SECTION_TYPE_IMPORT:
                        (*module)->imports = section.import_section.imports;
                        break;
                    case SECTION_TYPE_FUNCTION:
                        function_section = section.function_section;
                        break;
                    case SECTION_TYPE_TABLE:
                        (*module)->tables = section.table_section.tables;
                        break;
                    case SECTION_TYPE_MEMORY:
                        (*module)->mems = section.memory_section.memories;
                        break;
                    case SECTION_TYPE_GLOBAL:
                        (*module)->globals = section.global_section.globals;
                        break;
                    case SECTION_TYPE_EXPORT:
                        (*module)->exports = section.export_section.exports;
                        break;
                    case SECTION_TYPE_START:
                        (*module)->has_start = true;
                        (*module)->start = section.start_section.start;
                        break;
                    case SECTION_TYPE_ELEMENT:
                        (*module)->elem = section.element_section.elements;
                        break;
                    case SECTION_TYPE_CODE:
                        (*module)->funcs = section.code_section.funcs;
                        break;
                    case SECTION_TYPE_DATA:
                        (*module)->data = section.data_section.datas;
                        break;
                }
            }

            for (u32 i = 0; i < vec_typeidx_length(function_section.functions); i++) {
                vec_func_getp((*module)->funcs, i)->type = vec_typeidx_get(function_section.functions, i);
            }
            return NO_EXCEPTION;,
            return exception;
    )
}
