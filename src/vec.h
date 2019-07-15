#ifndef WASM_INTERPRETER_VEC_H
#define WASM_INTERPRETER_VEC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "util.h"
#include "value.h"

static void vec_error(const char *function, const char *format, ...) {
    char fmt[1024];
    va_list args;
    va_start (args, format);
    snprintf(fmt, sizeof(fmt), "Vector error in %s: %s\n", function, format);
    vfprintf(stderr, fmt, args); \
    va_end (args);
    exit(1);
}

#define VEC_ERROR(args...) \
  vec_error(__FUNCTION__, args)

#define CREATE_VEC_(type, name, fn_prefix, vec_type, it_type) \
typedef struct vec_type { \
    u32 length; \
    u32 capacity; \
    type* elements; \
} vec_type; \
typedef struct it_type { \
    const vec_type* vec; \
    u32 idx; \
} it_type; \
static inline vec_type* CAT(fn_prefix, _create)() { \
    vec_type* vec = calloc(1, sizeof(vec_type)); \
    if (vec == NULL) VEC_ERROR("calloc failed"); \
    vec->length = 0; \
    vec->capacity = 0; \
    vec->elements = NULL; \
    return vec; \
} \
static inline void CAT(fn_prefix, _free)(vec_type* vec) { \
    if (vec == NULL) return; \
    free(vec->elements); \
    free(vec); \
} \
static inline u32 CAT(fn_prefix, _length)(const vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    return vec->length; \
} \
static inline bool CAT(fn_prefix, _empty)(const vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    return vec->length == 0; \
} \
static inline void CAT(fn_prefix, _resize)(vec_type* vec, u32 new_length) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (new_length == vec->length) return; \
    if (new_length <= vec->capacity) { \
        memset(vec->elements + vec->length, 0, (vec->capacity - vec->length) * sizeof(type)); \
        vec->length = new_length; \
        return; \
    } \
    if (new_length > vec->capacity) { \
        void *ptr = realloc(vec->elements, sizeof(type) * new_length); \
        if (vec == NULL) VEC_ERROR("realloc failed"); \
        vec->elements = ptr; \
        vec->capacity = new_length; \
        memset(vec->elements + vec->length, 0, (vec->capacity - vec->length) * sizeof(type)); \
        vec->length = new_length; \
        return; \
    } \
} \
static inline void CAT(fn_prefix, _add)(vec_type* vec, type element) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    CAT(fn_prefix, _resize)(vec, vec->length + 1); \
    vec->elements[vec->length - 1] = element; \
} \
static inline void CAT(fn_prefix, _push)(vec_type* vec, type element) { \
    CAT(fn_prefix, _add)(vec, element); \
} \
static inline type CAT(fn_prefix, _get)(const vec_type* vec, u32 idx) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0 || idx >= vec->length) VEC_ERROR("index %d not in bounds", idx); \
    return vec->elements[idx]; \
}\
static inline type CAT(fn_prefix, _peek)(const vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (vec->length == 0) VEC_ERROR("vec is empty"); \
    return vec->elements[vec->length - 1]; \
}\
static inline void CAT(fn_prefix, _set)(vec_type* vec, u32 idx, type element) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0) VEC_ERROR("index %d is not be bigger than 0", idx); \
    CAT(fn_prefix, _resize)(vec, fmax(vec->length, idx + 1)); \
    vec->elements[idx] = element; \
}\
static inline type CAT(fn_prefix, _remove)(vec_type* vec, u32 idx) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0 || idx >= vec->length) VEC_ERROR("index %d not in bounds", idx); \
    type r = vec->elements[idx]; \
    memmove(vec->elements + idx, vec->elements + idx + 1, (vec->length - idx - 1) * sizeof(type)); \
    CAT(fn_prefix, _resize)(vec, vec->length - 1); \
    return r; \
} \
static inline type CAT(fn_prefix, _pop)(vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (vec->length == 0) VEC_ERROR("vec is empty"); \
    CAT(fn_prefix, _resize)(vec, vec->length - 1); \
    return vec->elements[vec->length]; \
} \
static inline it_type CAT(fn_prefix, _iterator)(const vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    it_type iterator; \
    iterator.vec = vec; \
    iterator.idx = -1; \
    return iterator; \
} \
static inline bool CAT(fn_prefix, _has_next)(const it_type* iterator) { \
    if (iterator == NULL) VEC_ERROR("iterator must not be NULL"); \
    return iterator->idx + 1 < iterator->vec->length; \
} \
static inline type CAT(fn_prefix, _next)(it_type* iterator) { \
    if (iterator == NULL) VEC_ERROR("iterator must not be NULL"); \
    iterator->idx += 1; \
    return CAT(fn_prefix, _get)(iterator->vec, iterator->idx); \
} \

#define CREATE_VEC(type, name) CREATE_VEC_(type, name, CAT(vec_, name), CAT(CAT(vec_, name), _t), CAT(CAT(vec_, name), _iterator_t))

#endif //WASM_INTERPRETER_VEC_H
