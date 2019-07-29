#ifndef MODULE_H
#define MODULE_H

#include "type.h"
#include "value.h"
#include "instruction.h"

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

typedef struct type_section {
    vec_functype_t *types;
} type_section_t;

typedef enum importdesc {
    IMPORTDESC_FUNC = 0x00,
    IMPORTDESC_TABLE = 0x01,
    IMPORTDESC_MEM = 0x02,
    IMPORTDESC_GLOBAL = 0x03,
} importdesc_t;

typedef struct import {
    name module;
    name name;
    importdesc_t desc;
    union {
        typeidx func;
        tabletype_t table;
        memtype_t mem;
        globaltype_t global;
    };
} import_t;

CREATE_VEC(import_t, import)

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

typedef struct global {
    globaltype_t gt;
    expression_t e;
} global_t;

CREATE_VEC(global_t, global)

typedef struct global_section {
    vec_global_t *globals;
} global_section_t;

typedef enum exportdesc {
    EXPORTDESC_FUNC = 0x00,
    EXPORTDESC_TABLE = 0x01,
    EXPORTDESC_MEM = 0x02,
    EXPORTDESC_GLOBAL = 0x03,
} exportdesc_t;

typedef struct export {
    name name;
    exportdesc_t desc;
    union {
        funcidx func;
        tableidx table;
        memidx mem;
        globalidx global;
    };
} export_t;

CREATE_VEC(export_t, export)

typedef struct export_section {
    vec_export_t *exports;
} export_section_t;

typedef struct start_section {
    funcidx start;
} start_section_t;

typedef struct element {
    tableidx table;
    expression_t offset;
    vec_funcidx_t *init;
} element_t;

CREATE_VEC(element_t, element)

typedef struct element_section {
    vec_element_t *elements;
} element_section_t;

typedef struct locals {
    u32 n;
    valtype_t t;
} locals_t;

CREATE_VEC(locals_t, locals)

typedef struct func {
    typeidx type;
    vec_locals_t *locals;
    expression_t expression;
} func_t;

CREATE_VEC(func_t, func)

typedef struct code_section {
    vec_func_t *funcs;
} code_section_t;

typedef struct data {
    memidx memidx;
    expression_t expression;
    vec_byte_t *init;
} data_t;

CREATE_VEC(data_t, data)

typedef struct data_section {
    vec_data_t *datas;
} data_section_t;

typedef struct section {
    section_type_t id;
    u32 size;
    union {
        type_section_t type_section;
        import_section_t import_section;
        function_section_t function_section;
        table_section_t table_section;
        memory_section_t memory_section;
        global_section_t global_section;
        export_section_t export_section;
        start_section_t start_section;
        element_section_t element_section;
        code_section_t code_section;
        data_section_t data_section;
    };
} section_t;

typedef struct module {
    vec_functype_t *types;
    vec_func_t *funcs;
    vec_tabletype_t *tables;
    vec_memtype_t *mems;
    vec_global_t *globals;
    vec_element_t *elem;
    vec_data_t *data;
    bool has_start;
    funcidx start;
    vec_import_t *imports;
    vec_export_t *exports;
    name name;
} module_t;

CREATE_VEC(module_t, module)

#endif // MODULE_H
