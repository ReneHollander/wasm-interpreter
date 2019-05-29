#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include "util.h"
#include "../type.h"

DEFINE_VEC_STRUCT_PRIMITIVE(byte);
DEFINE_VEC_STRUCT_PRIMITIVE(typeidx);

typedef struct limit {
    u32 min;
    u32 max;
    bool has_max;
} limit_t;

typedef enum valtype {
    valtype_i32,
    valtype_i64,
    valtype_f32,
    valtype_f64,
} valtype_t;

DEFINE_VEC_STRUCT(valtype);

typedef struct functype {
    vec_valtype_t *t1;
    vec_valtype_t *t2;
} functype_t;

DEFINE_VEC_STRUCT(functype);

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

typedef enum mutability {
    MUTABILITY_CONST = 0x00,
    MUTABILITY_VAR = 0x01
} mutability_t;

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

typedef struct tabletype {
    limit_t lim;
} tabletype_t;

DEFINE_VEC_STRUCT(tabletype);

typedef struct memtype {
    limit_t lim;
} memtype_t;

typedef struct globaltype {
    valtype_t t;
    mutability_t m;
} globaltype_t;

typedef struct import {
    char *module;
    char *name;
    importdesc_t type;
    union {
        typeidx x;
        tabletype_t tt;
        memtype_t mt;
        globaltype_t gt;
    };
} import_t;

DEFINE_VEC_STRUCT(import);

typedef struct import_section {
    vec_import_t *im;
} import_section_t;

typedef struct function_section {
    vec_typeidx_t *x;
} function_section_t;

typedef struct table_section {
    vec_tabletype_t *tab;
} table_section_t;

typedef struct section {
    section_type_t id;
    u32 size;
    union {
        custom_section_t custom_section;
        type_section_t type_section;
        import_section_t import_section;
        function_section_t function_section;
        table_section_t table_section;
    };
} section_t;

void set_input(FILE *i);

void parse();

#endif // PARSER_PARSER_H
