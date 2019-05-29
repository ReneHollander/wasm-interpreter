#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define CAT(x, y) CAT_(x, y)
#define CAT_(x, y) x ## y

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

#endif // UTIL_H
