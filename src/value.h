#ifndef VALUE_H
#define VALUE_H

#include "stdint.h"
#include "stdbool.h"
#include "util.h"

typedef uint8_t byte;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef s32 i32;
typedef s64 i64;

typedef float f32;
typedef double f64;

typedef char* name;

typedef u32 typeidx;
typedef u32 funcidx;
typedef u32 tableidx;
typedef u32 memidx;
typedef u32 globalidx;
typedef u32 localidx;
typedef u32 labelidx;

#define DEFINE_VEC_STRUCT(type) \
typedef struct CAT(vec_, type) { \
    u32 length; \
    CAT(type, _t) values[0]; \
} CAT(CAT(vec_, type), _t)

#define DEFINE_VEC_STRUCT_PRIMITIVE(type) \
typedef struct CAT(vec_, type) { \
    u32 length; \
    type values[0]; \
} CAT(CAT(vec_, type), _t)

DEFINE_VEC_STRUCT_PRIMITIVE(byte);
DEFINE_VEC_STRUCT_PRIMITIVE(typeidx);
DEFINE_VEC_STRUCT_PRIMITIVE(labelidx);

#endif // VALUE_H
