#include <stdlib.h>

#include "parser.h"
#include "next.h"

FILE *input;
size_t consumed_counter;

extern void parse_error(char *msg);

void set_input(FILE *i) {
    input = i;
}

static void parse_custom_section(section_t *section) {
    TRACK_CONSUMED_BYTES(name_size, section->custom_section.name, next_name());

    size_t remaining_size = section->size - name_size;
    section->custom_section.bytes = malloc(sizeof(byte) * remaining_size);
    for (int i = 0; i < remaining_size; i++) {
        section->custom_section.bytes[i] = next_byte();
    }
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

MAKE_NEXT_VEC(vec_valtype_t, valtype_t, parse_valtype, parse_vec_valtype)

static inline void parse_functype(functype_t *functype) {
    byte opcode = next_byte();
    if (opcode != 0x60) parse_error("expected opcode 0x60");
    functype->t1 = parse_vec_valtype();
    functype->t2 = parse_vec_valtype();
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_functype_t, functype_t, parse_functype, parse_vec_functype)

static inline void parse_limit(limit_t *limit) {
    switch (next_byte()) {
        case 0x00:
            limit->has_max = false;
            limit->min = next_u32();
            break;
        case 0x01:
            limit->has_max = true;
            limit->min = next_u32();
            limit->max = next_u32();
            break;
        default:
            parse_error("unknown limit type");
    }
}

static inline mutability_t parse_mutability() {
    switch (next_byte()) {
        case 0x00:
            return MUTABILITY_CONST;
        case 0x01:
            return MUTABILITY_VAR;
        default:
            parse_error("unknown mutability type");
            __builtin_unreachable();
    }
}

static inline void parse_tabletype(tabletype_t *tabletype) {
    if (next_byte() != 0x70) parse_error("unknown elemtype");
    parse_limit(&tabletype->lim);
}

static inline void parse_memtype(memtype_t *memtype) {
    parse_limit(&memtype->lim);
}

static inline void parse_globaltype(globaltype_t *globaltype) {
    globaltype->t = parse_valtype();
    globaltype->m = parse_mutability();
}

static inline void parse_import(import_t *import) {
    import->module = next_name();
    import->name = next_name();

    printf("module=%s, name=%s\n", import->module, import->name);

    switch (next_byte()) {
        case IMPORTDESC_FUNC:
            import->x = next_u32();
            break;
        case IMPORTDESC_TABLE:
            parse_tabletype(&import->tt);
            break;
        case IMPORTDESC_MEM:
            parse_memtype(&import->mt);
            break;
        case IMPORTDESC_GLOBAL:
            parse_globaltype(&import->gt);
            break;
        default:
            parse_error("unknown importdesc type");
    }
}

MAKE_NEXT_VEC_BY_REFERENCE(vec_import_t, import_t, parse_import, parse_vec_import)
MAKE_NEXT_VEC_BY_REFERENCE(vec_tabletype_t, tabletype_t, parse_tabletype, parse_vec_tabletype)

static void parse_type_section(section_t *section) {
    section->type_section.ft = parse_vec_functype();
}

static void parse_import_section(section_t *section) {
    section->import_section.im = parse_vec_import();
}

static void parse_function_section(section_t *section) {
    section->function_section.x = next_vec_typeidx();
}

static void parse_table_section(section_t *section) {
    section->table_section.tab = parse_vec_tabletype();
}

static void parse_memory_section(section_t *section) {
    advance(section->size);
}

static void parse_global_section(section_t *section) {
    advance(section->size);
}

static void parse_export_section(section_t *section) {
    advance(section->size);
}

static void parse_start_section(section_t *section) {
    advance(section->size);
}

static void parse_element_section(section_t *section) {
    advance(section->size);
}

static void parse_code_section(section_t *section) {
    advance(section->size);
}

static void parse_data_section(section_t *section) {
    advance(section->size);
}

static void parse_section(section_t *section) {
    byte id = next_byte();
    if (id > 11) parse_error("invalid section id");
    section->id = id;

    u32 size = next_u32();
    section->size = size;

    switch (section->id) {
        case SECTION_TYPE_CUSTOM:
            parse_custom_section(section);
            break;
        case SECTION_TYPE_TYPE:
            parse_type_section(section);
            break;
        case SECTION_TYPE_IMPORT:
            parse_import_section(section);
            break;
        case SECTION_TYPE_FUNCTION:
            parse_function_section(section);
            break;
        case SECTION_TYPE_TABLE:
            parse_table_section(section);
            break;
        case SECTION_TYPE_MEMORY:
            parse_memory_section(section);
            break;
        case SECTION_TYPE_GLOBAL:
            parse_global_section(section);
            break;
        case SECTION_TYPE_EXPORT:
            parse_export_section(section);
            break;
        case SECTION_TYPE_START:
            parse_start_section(section);
            break;
        case SECTION_TYPE_ELEMENT:
            parse_element_section(section);
            break;
        case SECTION_TYPE_CODE:
            parse_code_section(section);
            break;
        case SECTION_TYPE_DATA:
            parse_data_section(section);
            break;
        default:
            parse_error("unknown section type");
    }
}

void parse() {
    consumed_counter = 0;

    // Check magic value of module.
    if (next_byte() != 0x00 || next_byte() != 0x61 || next_byte() != 0x73 || next_byte() != 0x6D) {
        parse_error("unknown magic value");
    }

    // Check version number of module.
    if (next_byte() != 0x01 || next_byte() != 0x00 || next_byte() != 0x00 || next_byte() != 0x00) {
        parse_error("unknown version number");
    }

    while (1) {
        int next = fgetc(input);
        if (next == EOF) break;
        ungetc(next, input);

        section_t section;
        parse_section(&section);
        printf("sectionid=%d\n", section.id);
    }
}
