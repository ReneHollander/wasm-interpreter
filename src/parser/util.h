#ifndef PARSER_UTIL_H
#define PARSER_UTIL_H

#include "stdlib.h"
#include "../util.h"

#define MAKE_NEXT_VEC(result_type, element_type, generator_func, name) \
static inline result_type *name() { \
    u32 n = next_u32(); \
    result_type *vec = malloc(sizeof(result_type) + (sizeof(element_type) * n)); \
    vec->length = n; \
    for (int i = 0; i < n; i++) { \
        vec->values[i] = generator_func(); \
    } \
    return vec; \
}

#define MAKE_NEXT_VEC_BY_REFERENCE(result_type, element_type, generator_func, name) \
static inline result_type *name() { \
    u32 n = next_u32(); \
    result_type *vec = malloc(sizeof(result_type) + (sizeof(element_type) * n)); \
    vec->length = n; \
    for (int i = 0; i < n; i++) { \
        generator_func(&vec->values[i]); \
    } \
    return vec; \
}

#endif // PARSER_UTIL_H
