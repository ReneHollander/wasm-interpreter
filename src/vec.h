#ifndef WASM_INTERPRETER_VEC_H
#define WASM_INTERPRETER_VEC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "util.h"
#include "value.h"
#include "exception.h"

typedef enum direction {
    IT_FORWARDS,
    IT_BACKWARDS,
} direction_t;

#define VEC_ERROR(args...) THROW_EXCEPTION_WITH_MSG(EXCEPTION_VECTOR_EXCEPTION, args)

#define CREATE_VEC_(type, name, fn_prefix, vec_type, it_type) \
typedef struct CAT(vec_, name) { \
    u32 _length; \
    u32 _capacity; \
    type* _elements; \
} vec_type; \
typedef struct CAT(CAT(vec_, name), _iterator) { \
    const vec_type* _vec; \
    u32 _idx; \
    s32 _dir; \
} it_type; \
static type CAT(empty_, type) = {0}; \
static inline vec_type* CAT(fn_prefix, _create)() { \
    vec_type* vec = calloc(1, sizeof(vec_type)); \
    if (vec == NULL) VEC_ERROR("calloc failed"); \
    vec->_length = 0; \
    vec->_capacity = 0; \
    vec->_elements = NULL; \
    return vec; \
} \
static inline void CAT(fn_prefix, _free)(vec_type* vec) { \
    if (vec == NULL) return; \
    free(vec->_elements); \
    free(vec); \
} \
static inline u32 CAT(fn_prefix, _length)(const vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    return vec->_length; \
} \
static inline bool CAT(fn_prefix, _empty)(const vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    return vec->_length == 0; \
} \
static inline void CAT(fn_prefix, _resize)(vec_type* vec, u32 new_length) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (new_length == vec->_length) return; \
    if (new_length <= vec->_capacity) { \
        memset(vec->_elements + vec->_length, 0, (vec->_capacity - vec->_length) * sizeof(type)); \
        vec->_length = new_length; \
        return; \
    } \
    if (new_length > vec->_capacity) { \
        void *ptr = realloc(vec->_elements, sizeof(type) * new_length); \
        if (vec == NULL) VEC_ERROR("realloc failed"); \
        vec->_elements = ptr; \
        vec->_capacity = new_length; \
        memset(vec->_elements + vec->_length, 0, (vec->_capacity - vec->_length) * sizeof(type)); \
        vec->_length = new_length; \
        return; \
    } \
} \
static inline type* CAT(fn_prefix, _add)(vec_type* vec, type element) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    CAT(fn_prefix, _resize)(vec, vec->_length + 1); \
    vec->_elements[vec->_length - 1] = element; \
    return &vec->_elements[vec->_length - 1]; \
} \
static inline type* CAT(fn_prefix, _push)(vec_type* vec, type element) { \
    return CAT(fn_prefix, _add)(vec, element); \
} \
static inline type CAT(fn_prefix, _get)(const vec_type* vec, u32 idx) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0 || idx >= vec->_length) VEC_ERROR("index %d not in bounds", idx); \
    return vec->_elements[idx]; \
}\
static inline type CAT(fn_prefix, _get_or)(const vec_type* vec, u32 idx, type def) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0 || idx >= vec->_length) return def; \
    return vec->_elements[idx]; \
}\
static inline type* CAT(fn_prefix, _getp)(const vec_type* vec, u32 idx) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0 || idx >= vec->_length) VEC_ERROR("index %d not in bounds", idx); \
    return &vec->_elements[idx]; \
}\
static inline type* CAT(fn_prefix, _getp_or)(const vec_type* vec, u32 idx, type* def) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0 || idx >= vec->_length) return def; \
    return &vec->_elements[idx]; \
}\
static inline type CAT(fn_prefix, _peek)(const vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (vec->_length == 0) VEC_ERROR("vec is empty"); \
    return vec->_elements[vec->_length - 1]; \
}\
static inline type CAT(fn_prefix, _peek_or)(const vec_type* vec, type def) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (vec->_length == 0) return def; \
    return vec->_elements[vec->_length - 1]; \
}\
static inline type* CAT(fn_prefix, _peekp)(const vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (vec->_length == 0) VEC_ERROR("vec is empty"); \
    return &vec->_elements[vec->_length - 1]; \
}\
static inline type* CAT(fn_prefix, _peekp_or)(const vec_type* vec, type* def) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (vec->_length == 0) return def; \
    return &vec->_elements[vec->_length - 1]; \
}\
static inline type* CAT(fn_prefix, _set)(vec_type* vec, u32 idx, type element) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0) VEC_ERROR("index %d is not positive", idx); \
    CAT(fn_prefix, _resize)(vec, fmax(vec->_length, idx + 1)); \
    vec->_elements[idx] = element; \
    return &vec->_elements[idx]; \
}\
static inline type* CAT(fn_prefix, _insert)(vec_type* vec, u32 idx, type element) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0) VEC_ERROR("index %d is not positive", idx); \
    CAT(fn_prefix, _resize)(vec, fmax(vec->_length + 1, idx + 1)); \
    for (u32 i = vec->_length - 1; i > idx; i--) { \
        vec->_elements[i] = vec->_elements[i - 1]; \
    }\
    vec->_elements[idx] = element; \
    return &vec->_elements[idx]; \
}\
static inline type CAT(fn_prefix, _remove)(vec_type* vec, u32 idx) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (idx < 0 || idx >= vec->_length) VEC_ERROR("index %d not in bounds", idx); \
    type r = vec->_elements[idx]; \
    memmove(vec->_elements + idx, vec->_elements + idx + 1, (vec->_length - idx - 1) * sizeof(type)); \
    CAT(fn_prefix, _resize)(vec, vec->_length - 1); \
    return r; \
} \
static inline type CAT(fn_prefix, _pop)(vec_type* vec) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (vec->_length == 0) VEC_ERROR("vec is empty"); \
    CAT(fn_prefix, _resize)(vec, vec->_length - 1); \
    return vec->_elements[vec->_length]; \
} \
static inline type CAT(fn_prefix, _pop_or)(vec_type* vec, type def) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    if (vec->_length == 0) return def; \
    CAT(fn_prefix, _resize)(vec, vec->_length - 1); \
    return vec->_elements[vec->_length]; \
} \
static inline it_type CAT(fn_prefix, _iterator)(const vec_type* vec, direction_t dir) { \
    if (vec == NULL) VEC_ERROR("vec must not be NULL"); \
    it_type iterator; \
    iterator._vec = vec; \
    if (dir == IT_FORWARDS) { \
        iterator._dir = 1; \
        iterator._idx = -1; \
    } else if (dir == IT_BACKWARDS) { \
        iterator._dir = -1; \
        iterator._idx = vec->_length; \
    } else { \
        VEC_ERROR("unknown iterator direction %d", dir); \
    } \
    return iterator; \
} \
static inline void CAT(fn_prefix, _set_direction)(it_type* iterator, direction_t dir) { \
    if (iterator == NULL) VEC_ERROR("iterator must not be NULL"); \
    if (dir == IT_FORWARDS) iterator->_dir = 1; \
    else if (dir == IT_BACKWARDS) iterator->_dir = -1; \
    else VEC_ERROR("unknown iterator direction %d", dir); \
} \
static inline bool CAT(fn_prefix, _has_next)(const it_type* iterator) { \
    if (iterator == NULL) VEC_ERROR("iterator must not be NULL"); \
    return iterator->_idx + iterator->_dir < iterator->_vec->_length && iterator->_idx + iterator->_dir >= 0; \
} \
static inline type CAT(fn_prefix, _next)(it_type* iterator) { \
    if (iterator == NULL) VEC_ERROR("iterator must not be NULL"); \
    iterator->_idx += iterator->_dir; \
    return CAT(fn_prefix, _get)(iterator->_vec, iterator->_idx); \
} \
static inline u32 CAT(fn_prefix, _get_iterator_index)(it_type* iterator) { \
    if (iterator == NULL) VEC_ERROR("iterator must not be NULL"); \
    return iterator->_idx; \
} \
static inline type* CAT(fn_prefix, _nextp)(it_type* iterator) { \
    if (iterator == NULL) VEC_ERROR("iterator must not be NULL"); \
    iterator->_idx += iterator->_dir; \
    return CAT(fn_prefix, _getp)(iterator->_vec, iterator->_idx); \
} \

#define CREATE_VEC(type, name) CREATE_VEC_(type, name, CAT(vec_, name), CAT(CAT(vec_, name), _t), CAT(CAT(vec_, name), _iterator_t))

#endif //WASM_INTERPRETER_VEC_H
