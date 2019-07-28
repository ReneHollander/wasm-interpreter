#ifndef TYPE_H
#define TYPE_H

#include "value.h"
#include "vec.h"

typedef enum valtype {
    VALTYPE_I32 = 0x7F,
    VALTYPE_I64 = 0x7E,
    VALTYPE_F32 = 0x7D,
    VALTYPE_F64 = 0x7C,
} valtype_t;

CREATE_VEC(valtype_t, valtype);

typedef struct blocktype {
    bool empty;
    valtype_t type;
} blocktype_t;

typedef struct functype {
    vec_valtype_t *t1;
    vec_valtype_t *t2;
} functype_t;

CREATE_VEC(functype_t, functype);

typedef struct limits {
    u32 min;
    u32 max;
    bool has_max;
} limits_t;

typedef struct memtype {
    limits_t lim;
} memtype_t;

CREATE_VEC(memtype_t, memtype);

typedef struct tabletype {
    limits_t lim;
} tabletype_t;

CREATE_VEC(tabletype_t, tabletype);

typedef enum mutability {
    MUTABILITY_CONST = 0x00,
    MUTABILITY_VAR = 0x01
} mutability_t;

typedef struct globaltype {
    valtype_t t;
    mutability_t m;
} globaltype_t;

#endif // TYPE_H
