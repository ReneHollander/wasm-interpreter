#ifndef TYPE_H
#define TYPE_H

#include "util.h"
#include "stdint.h"
#include "stdbool.h"

typedef uint8_t byte;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u24;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s24;
typedef int32_t s32;
typedef int64_t s64;

typedef s32 i32;
typedef s64 i64;

typedef float f32;
typedef double f64;

typedef u32 typeidx;
typedef u32 funcidx;
typedef u32 tableidx;
typedef u32 memidx;
typedef u32 globalidx;
typedef u32 localidx;
typedef u32 labelidx;

DEFINE_VEC_STRUCT_PRIMITIVE(byte);
DEFINE_VEC_STRUCT_PRIMITIVE(typeidx);
DEFINE_VEC_STRUCT_PRIMITIVE(labelidx);

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

typedef enum mutability {
    MUTABILITY_CONST = 0x00,
    MUTABILITY_VAR = 0x01
} mutability_t;

typedef struct tabletype {
    limit_t lim;
} tabletype_t;

DEFINE_VEC_STRUCT(tabletype);

typedef struct memtype {
    limit_t lim;
} memtype_t;

DEFINE_VEC_STRUCT(memtype);

#endif // TYPE_H
