#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include "util.h"
#include "../type.h"
#include "../instruction.h"

typedef enum section_type {
    SECTION_TYPE_CUSTOM = 0,
    SECTION_TYPE_TYPE = 1,
    SECTION_TYPE_IMPORT = 2,
    SECTION_TYPE_FUNCTION = 3,
    SECTION_TYPE_TABLE = 4,
    SECTION_TYPE_MEMORY = 5,
    SECTION_TYPE_GLOBAL = 6,
    SECTION_TYPE_EXPORT = 7,
    SECTION_TYPE_START = 8,
    SECTION_TYPE_ELEMENT = 9,
    SECTION_TYPE_CODE = 10,
    SECTION_TYPE_DATA = 11,
} section_type_t;

typedef struct custom_section {
    char *name;
    u32 length;
    byte *bytes;
} custom_section_t;

typedef struct type_section {
    vec_functype_t *ft;
} type_section_t;

typedef enum importdesc {
    IMPORTDESC_FUNC = 0x00,
    IMPORTDESC_TABLE = 0x01,
    IMPORTDESC_MEM = 0x02,
    IMPORTDESC_GLOBAL = 0x03,
} importdesc_t;

typedef enum exportdesc {
    EXPORTDESC_FUNC = 0x00,
    EXPORTDESC_TABLE = 0x01,
    EXPORTDESC_MEM = 0x02,
    EXPORTDESC_GLOBAL = 0x03,
} exportdesc_t;

typedef struct globaltype {
    valtype_t t;
    mutability_t m;
} globaltype_t;

typedef struct import {
    char *module;
    char *name;
    importdesc_t desc;
    union {
        typeidx func;
        tabletype_t table;
        memtype_t mem;
        globaltype_t global;
    };
} import_t;

DEFINE_VEC_STRUCT(import);

typedef struct export {
    char *name;
    exportdesc_t desc;
    union {
        funcidx func;
        tableidx table;
        memidx mem;
        globalidx global;
    };
} export_t;

DEFINE_VEC_STRUCT(export);

typedef struct global {
    globaltype_t gt;
    expression_t e;
} global_t;

DEFINE_VEC_STRUCT(global);

typedef struct import_section {
    vec_import_t *imports;
} import_section_t;

typedef struct function_section {
    vec_typeidx_t *functions;
} function_section_t;

typedef struct table_section {
    vec_tabletype_t *tables;
} table_section_t;

typedef struct memory_section {
    vec_memtype_t *memories;
} memory_section_t;

typedef struct global_section {
    vec_global_t *globals;
} global_section_t;

typedef struct export_section {
    vec_export_t *exports;
} export_section_t;

typedef struct section {
    section_type_t id;
    u32 size;
    union {
        custom_section_t custom_section;
        type_section_t type_section;
        import_section_t import_section;
        function_section_t function_section;
        table_section_t table_section;
        memory_section_t memory_section;
        global_section_t global_section;
        export_section_t export_section;
    };
} section_t;

void set_input(FILE *i);

void parse();

#endif // PARSER_PARSER_H
