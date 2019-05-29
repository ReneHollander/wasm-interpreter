#ifndef PARSER_NEXT_H
#define PARSER_NEXT_H

#include <string.h>
#include <assert.h>
#include "util.h"
#include "../type.h"

extern FILE *input;
extern size_t consumed_counter;

extern void parse_error(char *msg);

#define TRACK_CONSUMED_BYTES(size_variable, result_variable, op) \
size_t size_variable = consumed_counter; \
result_variable = op; \
size_variable = consumed_counter - size_variable

static byte next_byte() {
    int value = fgetc(input);
    if (value == EOF) {
        parse_error("reached eof while parsing");
    }
    consumed_counter++;
    return value;
}

static void advance(u32 count) {
    for (u32 i = 0; i < count; i++) {
        next_byte();
    }
}

static inline u8 next_u8() {
    u8 b = next_byte();
    return b;
}

static inline u16 next_u16() {
    u16 n = next_byte();
    if (n < 128) {
        return n;
    }
    u16 m = next_u8();
    return 128 * m + (n - 128);
}

static inline u24 next_u24() {
    u24 n = next_byte();
    if (n < 128) {
        return n;
    }
    u24 m = next_u16();
    return 128 * m + (n - 128);
}

static inline u32 next_u32() {
    u32 n = next_byte();
    if (n < 128) {
        return n;
    }
    u32 m = next_u24();
    return 128 * m + (n - 128);
}

MAKE_NEXT_VEC(vec_byte_t, byte, next_byte, next_vec_byte)

static inline char *next_name() {
    vec_byte_t *vec = next_vec_byte();
    char *name = malloc(sizeof(char) * (vec->length + 1));
    memcpy(name, vec->values, sizeof(char) * vec->length);
    name[vec->length] = '\0';
    free(vec);
    return name;
}

#endif // PARSER_NEXT_H
