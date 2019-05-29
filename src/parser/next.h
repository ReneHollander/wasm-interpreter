#ifndef PARSER_NEXT_H
#define PARSER_NEXT_H

#include <stdio.h>
#include <string.h>
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

static byte peek_byte() {
    int value = fgetc(input);
    if (value == EOF) {
        parse_error("reached eof while parsing");
    }
    ungetc(value, input);
    return value;
}

static void advance(u32 count) {
    for (u32 i = 0; i < count; i++) {
        next_byte();
    }
}

static inline uint64_t read_LEB(uint32_t maxbits, bool sign) {
    uint64_t result = 0;
    uint32_t shift = 0;
    uint32_t bcnt = 0;
    uint64_t byte;

    while (true) {
        byte = next_byte();
        result |= ((byte & 0x7f) << shift);
        shift += 7;
        if ((byte & 0x80) == 0) {
            break;
        }
        bcnt += 1;
        if (bcnt > (maxbits + 7 - 1) / 7) {
            parse_error("leb overflow");
        }
    }
    if (sign && (shift < maxbits) && (byte & 0x40)) {
        // Sign extend
        result |= -(1 << shift);
    }
    return result;
}

static inline u8 next_u8() {
    read_LEB(8, false);
}

static inline u16 next_u16() {
    read_LEB(16, false);
}

static inline u32 next_u32() {
    read_LEB(32, false);
}

static inline u64 next_u64() {
    read_LEB(64, false);
}

static inline s8 next_s8() {
    read_LEB(8, true);
}

static inline s16 next_s16() {
    read_LEB(16, true);
}

static inline s32 next_s32() {
    read_LEB(32, true);
}

static inline s64 next_s64() {
    read_LEB(64, true);
}

static inline i32 next_i32() {
    return next_s32();
}

static inline i64 next_i64() {
    return next_s64();
}

static inline f32 next_f32() {
    f32 output;
    *((byte *) (&output) + 3) = next_byte();
    *((byte *) (&output) + 2) = next_byte();
    *((byte *) (&output) + 1) = next_byte();
    *((byte *) (&output) + 0) = next_byte();
    return output;
}

static inline f64 next_f64() {
    f64 output;
    *((byte *) (&output) + 7) = next_byte();
    *((byte *) (&output) + 6) = next_byte();
    *((byte *) (&output) + 5) = next_byte();
    *((byte *) (&output) + 4) = next_byte();
    *((byte *) (&output) + 3) = next_byte();
    *((byte *) (&output) + 2) = next_byte();
    *((byte *) (&output) + 1) = next_byte();
    *((byte *) (&output) + 0) = next_byte();
    return output;
}

static inline typeidx next_typeidx() {
    return next_u32();
}

static inline funcidx next_funcidx() {
    return next_u32();
}

static inline tableidx next_tableidx() {
    return next_u32();
}

static inline memidx next_memidx() {
    return next_u32();
}

static inline globalidx next_globalidx() {
    return next_u32();
}

static inline localidx next_localidx() {
    return next_u32();
}

static inline labelidx next_labelidx() {
    return next_u32();
}

MAKE_NEXT_VEC(vec_byte_t, byte, next_byte, next_vec_byte)

MAKE_NEXT_VEC(vec_typeidx_t, typeidx, next_typeidx, next_vec_typeidx)

MAKE_NEXT_VEC(vec_labelidx_t, labelidx , next_labelidx, next_vec_labelidx)

static inline char *next_name() {
    vec_byte_t *vec = next_vec_byte();
    char *name = malloc(sizeof(char) * (vec->length + 1));
    memcpy(name, vec->values, sizeof(char) * vec->length);
    name[vec->length] = '\0';
    free(vec);
    return name;
}

#endif // PARSER_NEXT_H
