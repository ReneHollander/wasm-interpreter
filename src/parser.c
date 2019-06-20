#include "parser.h"

#include "type.h"
#include "value.h"
#include "instruction.h"
#include "module.h"
#include "stdlib.h"
#include "string.h"
#include "strings.h"

typedef struct parser_state {
    FILE *input;
    parse_error_f parse_error;
} parser_state_t;

#define MAKE_NEXT_VEC(result_type, element_type, generator_func, name) \
static inline result_type *name(parser_state_t *state) { \
    u32 n = next_u32(state); \
    result_type *vec = malloc(sizeof(result_type) + (sizeof(element_type) * n)); \
    vec->length = n; \
    for (int i = 0; i < n; i++) { \
        vec->values[i] = generator_func(state); \
    } \
    return vec; \
}

#define MAKE_NEXT_VEC_BY_REFERENCE(result_type, element_type, generator_func, name) \
static inline result_type *name(parser_state_t *state) { \
    u32 n = next_u32(state); \
    result_type *vec = malloc(sizeof(result_type) + (sizeof(element_type) * n)); \
    vec->length = n; \
    for (int i = 0; i < n; i++) { \
        generator_func(state, &vec->values[i]); \
    } \
    return vec; \
}

static byte next_byte(parser_state_t *state) {
    int value = fgetc(state->input);
    if (value == EOF) {
        state->parse_error("reached eof while parsing");
    }
    return value;
}

static byte peek_byte(parser_state_t *state) {
    int value = fgetc(state->input);
    if (value == EOF) {
        state->parse_error("reached eof while parsing");
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
            state->parse_error("leb overflow");
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

MAKE_NEXT_VEC(vec_byte_t, byte, next_byte, next_vec_byte)

MAKE_NEXT_VEC(vec_typeidx_t, typeidx, next_typeidx, next_vec_typeidx)

MAKE_NEXT_VEC(vec_funcidx_t, funcidx, next_funcidx, next_vec_funcidx)

MAKE_NEXT_VEC(vec_labelidx_t, labelidx, next_labelidx, next_vec_labelidx)

static inline name next_name(parser_state_t *state) {
    vec_byte_t *vec = next_vec_byte(state);
    name name = malloc(sizeof(char) * (vec->length + 1));
    memcpy(name, vec->values, sizeof(char) * vec->length);
    name[vec->length] = '\0';
    free(vec);
    return name;
}

static inline void parse_instruction(parser_state_t *state, instruction_t *instruction);

static inline instruction_t *allocate_instruction(vec_instruction_t **vec) {
    (*vec)->length = (*vec)->length + 1;
    *vec = realloc(*vec, (sizeof(vec_instruction_t) + (sizeof(instruction_t) * (*vec)->length)));
    return &((*vec)->values[(*vec)->length - 1]);
}

static inline void parse_memarg(parser_state_t *state, memarg_t *memarg) {
    memarg->align = next_u32(state);
    memarg->offset = next_u32(state);
}

static inline void parse_insn_br_table(parser_state_t *state, insn_br_table_t *table) {
    table->labels = next_vec_labelidx(state);
    table->default_label = next_labelidx(state);
}

static inline valtype_t parse_valtype(parser_state_t *state) {
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
            state->parse_error("unexpected valtype");
            __builtin_unreachable();
    }
}

MAKE_NEXT_VEC(vec_valtype_t, valtype_t, parse_valtype, parse_vec_valtype)

static inline void parse_blocktype(parser_state_t *state, blocktype_t *blocktype) {
    if (peek_byte(state) == 0x40) {
        next_byte(state);
        blocktype->empty = true;
    } else {
        blocktype->type = parse_valtype(state);
    }
}

static inline void parse_insn_block(parser_state_t *state, insn_block_t *block) {
    parse_blocktype(state, &block->resulttype);
    block->instructions = calloc(1, sizeof(vec_instruction_t));
    while (peek_byte(state) != 0x0B) {
        instruction_t *allocated = allocate_instruction(&block->instructions);
        parse_instruction(state, allocated);
    }
    next_byte(state);
}

static inline void parse_insn_if(parser_state_t *state, insn_if_t *if_block) {
    parse_blocktype(state, &if_block->resulttype);
    if_block->ifpath = calloc(1, sizeof(vec_instruction_t));
    while (peek_byte(state) != OP_ELSE && peek_byte(state) != 0x0B) {
        instruction_t *allocated = allocate_instruction(&if_block->ifpath);
        parse_instruction(state, allocated);
    }
    if (peek_byte(state) == OP_ELSE) {
        next_byte(state);
        if_block->elsepath = calloc(1, sizeof(vec_instruction_t));
        while (peek_byte(state) != 0x0B) {
            instruction_t *allocated = allocate_instruction(&if_block->elsepath);
            parse_instruction(state, allocated);
        }
    } else {
        if_block->elsepath = NULL;
    }
    next_byte(state);
}

static inline void parse_instruction(parser_state_t *state, instruction_t *instruction) {
    instruction->opcode = next_byte(state);
    switch (instruction->opcode) {
        case OP_BLOCK:
        case OP_LOOP:
            parse_insn_block(state, &instruction->block);
            break;
        case OP_IF:
            parse_insn_if(state, &instruction->if_block);
            break;
        case OP_BR:
        case OP_BR_IF:
            instruction->labelidx = next_labelidx(state);
            break;
        case OP_BR_TABLE:
            parse_insn_br_table(state, &instruction->table);
            break;
        case OP_CALL:
            instruction->funcidx = next_funcidx(state);
            break;
        case OP_CALL_INDIRECT:
            instruction->typeidx = next_typeidx(state);
            if (next_byte(state) != 0x00) state->parse_error("invalid tableidx for call indirect");
            break;
        case OP_LOCAL_GET:
        case OP_LOCAL_SET:
        case OP_LOCAL_TEE:
            instruction->localidx = next_localidx(state);
            break;
        case OP_GLOBAL_GET:
        case OP_GLOBAL_SET:
            instruction->globalidx = next_globalidx(state);
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
            parse_memarg(state, &instruction->memarg);
            break;
        case OP_MEMORY_SIZE:
            if (next_byte(state) != 0x00) state->parse_error("unknown index for memory.size");
            break;
        case OP_MEMORY_GROW:
            if (next_byte(state) != 0x00) state->parse_error("unknown index for memory.grow");
            break;
        case OP_I32_CONST:
            instruction->const_i32 = next_i32(state);
            break;
        case OP_I64_CONST:
            instruction->const_i64 = next_i64(state);
            break;
        case OP_F32_CONST:
            instruction->const_f32 = next_f32(state);
            break;
        case OP_F64_CONST:
            instruction->const_f64 = next_f64(state);
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
            state->parse_error("unknown opcode");
            __builtin_unreachable();
    }
}

void parse_expression(parser_state_t *state, expression_t *expression) {
    expression->instructions = calloc(1, sizeof(vec_instruction_t));
    while (peek_byte(state) != 0x0B) {
        instruction_t *allocated = allocate_instruction(&expression->instructions);
        parse_instruction(state, allocated);
    }
    next_byte(state);
}

static inline void parse_limit(parser_state_t *state, limits_t *limits) {
    switch (next_byte(state)) {
        case 0x00:
            limits->has_max = false;
            limits->min = next_u32(state);
            break;
        case 0x01:
            limits->has_max = true;
            limits->min = next_u32(state);
            limits->max = next_u32(state);
            break;
        default:
            state->parse_error("unknown limit type");
            __builtin_unreachable();
    }
}

static inline void parse_functype(parser_state_t *state, functype_t *functype) {
    byte opcode = next_byte(state);
    if (opcode != 0x60) state->parse_error("expected opcode 0x60");
    functype->t1 = parse_vec_valtype(state);
    functype->t2 = parse_vec_valtype(state);
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_functype_t, functype_t, parse_functype, parse_vec_functype)

static void parse_type_section(parser_state_t *state, section_t *section) {
    section->type_section.types = parse_vec_functype(state);
}

static inline void parse_tabletype(parser_state_t *state, tabletype_t *tabletype) {
    if (next_byte(state) != 0x70) state->parse_error("unknown elemtype");
    parse_limit(state, &tabletype->lim);
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_tabletype_t, tabletype_t, parse_tabletype, parse_vec_tabletype)

static inline mutability_t parse_mutability(parser_state_t *state) {
    switch (next_byte(state)) {
        case 0x00:
            return MUTABILITY_CONST;
        case 0x01:
            return MUTABILITY_VAR;
        default:
            state->parse_error("unknown mutability type");
            __builtin_unreachable();
    }
}

static inline void parse_globaltype(parser_state_t *state, globaltype_t *globaltype) {
    globaltype->t = parse_valtype(state);
    globaltype->m = parse_mutability(state);
}

static inline void parse_memtype(parser_state_t *state, memtype_t *memtype) {
    parse_limit(state, &memtype->lim);
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_memtype_t, memtype_t, parse_memtype, parse_vec_memtype)

static inline void parse_import(parser_state_t *state, import_t *import) {
    import->module = next_name(state);
    import->name = next_name(state);
    import->desc = next_byte(state);

    switch (import->desc) {
        case IMPORTDESC_FUNC:
            import->func = next_typeidx(state);
            break;
        case IMPORTDESC_TABLE:
            parse_tabletype(state, &import->table);
            break;
        case IMPORTDESC_MEM:
            parse_memtype(state, &import->mem);
            break;
        case IMPORTDESC_GLOBAL:
            parse_globaltype(state, &import->global);
            break;
        default:
            state->parse_error("unknown importdesc type");
            __builtin_unreachable();
    }
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_import_t, import_t, parse_import, parse_vec_import)

static void parse_import_section(parser_state_t *state, section_t *section) {
    section->import_section.imports = parse_vec_import(state);
}

static void parse_function_section(parser_state_t *state, section_t *section) {
    section->function_section.functions = next_vec_typeidx(state);
}

static void parse_table_section(parser_state_t *state, section_t *section) {
    section->table_section.tables = parse_vec_tabletype(state);
}

static void parse_memory_section(parser_state_t *state, section_t *section) {
    section->memory_section.memories = parse_vec_memtype(state);
}

static inline void parse_global(parser_state_t *state, global_t *global) {
    parse_globaltype(state, &global->gt);
    parse_expression(state, &global->e);
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_global_t, global_t, parse_global, parse_vec_global)

static void parse_global_section(parser_state_t *state, section_t *section) {
    section->global_section.globals = parse_vec_global(state);
}

static inline void parse_export(parser_state_t *state, export_t *export) {
    export->name = next_name(state);
    export->desc = next_byte(state);

    switch (export->desc) {
        case EXPORTDESC_FUNC:
            export->func = next_funcidx(state);
            break;
        case EXPORTDESC_TABLE:
            export->table = next_tableidx(state);
            break;
        case EXPORTDESC_MEM:
            export->mem = next_memidx(state);
            break;
        case EXPORTDESC_GLOBAL:
            export->global = next_globalidx(state);
            break;
        default:
            state->parse_error("unknown exportdesc type");
            __builtin_unreachable();
    }
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_export_t, export_t, parse_export, parse_vec_export)

static void parse_export_section(parser_state_t *state, section_t *section) {
    section->export_section.exports = parse_vec_export(state);
}

static void parse_start_section(parser_state_t *state, section_t *section) {
    section->start_section.start = next_funcidx(state);
}

static inline void parse_element(parser_state_t *state, element_t *element) {
    element->table = next_tableidx(state);
    parse_expression(state, &element->offset);
    element->init = next_vec_funcidx(state);
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_element_t, element_t, parse_element, parse_vec_element)

static void parse_element_section(parser_state_t *state, section_t *section) {
    section->element_section.elements = parse_vec_element(state);
}

static inline void parse_locals(parser_state_t *state, locals_t *locals) {
    locals->n = next_u32(state);
    locals->t = parse_valtype(state);
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_locals_t, locals_t, parse_locals, parse_vec_locals);

static inline void parse_func(parser_state_t *state, func_t *func) {
    u32 size = next_u32(state);
    func->locals = parse_vec_locals(state);
    parse_expression(state, &func->expression);
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_func_t, func_t, parse_func, parse_vec_func)

static void parse_code_section(parser_state_t *state, section_t *section) {
    section->code_section.funcs = parse_vec_func(state);
}

static inline void parse_data(parser_state_t *state, data_t *data) {
    data->memidx = next_memidx(state);
    parse_expression(state, &data->expression);
    data->init = next_vec_byte(state);
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_data_t, data_t, parse_data, parse_vec_data)

static void parse_data_section(parser_state_t *state, section_t *section) {
    section->data_section.datas = parse_vec_data(state);
}

static void parse_section(parser_state_t *state, section_t *section) {
    byte id = next_byte(state);
    if (id > 11) state->parse_error("invalid section id");
    section->id = id;

    u32 size = next_u32(state);
    section->size = size;

    switch (section->id) {
        case SECTION_TYPE_CUSTOM:
            advance(state, section->size);
            break;
        case SECTION_TYPE_TYPE:
            parse_type_section(state, section);
            break;
        case SECTION_TYPE_IMPORT:
            parse_import_section(state, section);
            break;
        case SECTION_TYPE_FUNCTION:
            parse_function_section(state, section);
            break;
        case SECTION_TYPE_TABLE:
            parse_table_section(state, section);
            break;
        case SECTION_TYPE_MEMORY:
            parse_memory_section(state, section);
            break;
        case SECTION_TYPE_GLOBAL:
            parse_global_section(state, section);
            break;
        case SECTION_TYPE_EXPORT:
            parse_export_section(state, section);
            break;
        case SECTION_TYPE_START:
            parse_start_section(state, section);
            break;
        case SECTION_TYPE_ELEMENT:
            parse_element_section(state, section);
            break;
        case SECTION_TYPE_CODE:
            parse_code_section(state, section);
            break;
        case SECTION_TYPE_DATA:
            parse_data_section(state, section);
            break;
        default:
            state->parse_error("unknown section type");
            __builtin_unreachable();
    }
}

module_t *parse(FILE *input_file, parse_error_f parse_error) {
    parser_state_t state;
    state.input = input_file;
    state.parse_error = parse_error;

    module_t *module = calloc(1, sizeof(module_t));

    // Check magic value of module.
    if (next_byte(&state) != 0x00 || next_byte(&state) != 0x61 || next_byte(&state) != 0x73 ||
        next_byte(&state) != 0x6D) {
        parse_error("unknown magic value");
    }

    // Check version number of module.
    if (next_byte(&state) != 0x01 || next_byte(&state) != 0x00 || next_byte(&state) != 0x00 ||
        next_byte(&state) != 0x00) {
        parse_error("unknown version number");
    }

    function_section_t function_section;

    while (1) {
        int next = fgetc(state.input);
        if (next == EOF) break;
        ungetc(next, state.input);

        section_t section;
        parse_section(&state, &section);

        switch (section.id) {
            case SECTION_TYPE_TYPE:
                module->types = section.type_section.types;
                break;
            case SECTION_TYPE_CUSTOM: // Ignore.
                break;
            case SECTION_TYPE_IMPORT:
                module->imports = section.import_section.imports;
                break;
            case SECTION_TYPE_FUNCTION:
                function_section = section.function_section;
                break;
            case SECTION_TYPE_TABLE:
                module->tables = section.table_section.tables;
                break;
            case SECTION_TYPE_MEMORY:
                module->mems = section.memory_section.memories;
                break;
            case SECTION_TYPE_GLOBAL:
                module->globals = section.global_section.globals;
                break;
            case SECTION_TYPE_EXPORT:
                module->exports = section.export_section.exports;
                break;
            case SECTION_TYPE_START:
                module->has_start = true;
                module->start = section.start_section.start;
                break;
            case SECTION_TYPE_ELEMENT:
                module->elem = section.element_section.elements;
                break;
            case SECTION_TYPE_CODE:
                module->funcs = section.code_section.funcs;
                break;
            case SECTION_TYPE_DATA:
                module->data = section.data_section.datas;
                break;
        }
    }

    for (u32 i = 0; i < function_section.functions->length; i++) {
        module->funcs->values[i].type = function_section.functions->values[i];
    }

    return module;
}
