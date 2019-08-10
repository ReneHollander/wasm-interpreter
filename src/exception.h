#ifndef WASM_INTERPRETER_EXCEPTION_H
#define WASM_INTERPRETER_EXCEPTION_H

//#define DISABLE_EXCEPTION_HANDLING

#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef enum exception {
    NO_EXCEPTION = 0,

    EXCEPTION_VECTOR_EXCEPTION,

    EXCEPTION_PARSER_MEMORY_INDEX_NOT_ZERO,
    EXCEPTION_PARSER_UNEXPECTED_VALTYPE,
    EXCEPTION_PARSER_EOF_BEFORE_FINISHED,
    EXCEPTION_PARSER_LEB_OVERFLOW,
    EXCEPTION_PARSER_INVALID_TYPEIDX_FOR_CALL_INDIRECT,
    EXCEPTION_PARSER_UNKNOWN_OPCODE,
    EXCEPTION_PARSER_UNKNOWN_LIMIT_TYPE,
    EXCEPTION_PARSER_UNEXPECTED_OPCODE,
    EXCEPTION_PARSER_UNKNOWN_ELEMENT_TYPE,
    EXCEPTION_PARSER_UNKNOWN_MUTABILITY_TYPE,
    EXCEPTION_PARSER_UNKNOWN_IMPORTDESC_TYPE,
    EXCEPTION_PARSER_UNKNOWN_EXPORTDESC_TYPE,
    EXCEPTION_PARSER_INVALID_SECTION_ID,
    EXCEPTION_PARSER_UNKNOWN_SECTION_TYPE,
    EXCEPTION_PARSER_UNKNOWN_MAGIC_VALUE,
    EXCEPTION_PARSER_VERSION_NOT_SUPPORTED,
} exception_t;

#ifndef DISABLE_EXCEPTION_HANDLING

#include <setjmp.h>

typedef struct _exception_data {
    volatile bool active;
    volatile exception_t code;
    volatile char file[1024];
    volatile char function[1024];
    volatile int line;
    volatile char message[1024];
} _exception_data_t;

extern jmp_buf _exception_jump_buf;
extern volatile _exception_data_t _exception_data;

#define THROW_EXCEPTION_WITH_MSG(exception_code, args...) \
do { \
    if (!_exception_data.active) { \
        fprintf(stderr, "unhandled %s exception at %s(%s:%d): ", exception_code_to_string(exception_code), __FUNCTION__, __FILE__, __LINE__); \
        fprintf(stderr, args); \
        fprintf(stderr, "\n"); \
        fflush(stderr); \
        abort(); \
    } \
    _exception_data.active = false; \
    _exception_data.code = exception_code; \
    strncpy((char *) _exception_data.file, __FILE__, 1024); \
    strncpy((char *) _exception_data.function, __FUNCTION__, 1024); \
    _exception_data.line = __LINE__; \
    snprintf((char *) _exception_data.message, 1024, args); \
    longjmp(_exception_jump_buf, 1); \
} while (0)

#define TRY_CATCH(try_clause, catch_clause) \
{\
    if (_exception_data.active == true) { \
        fprintf(stderr, "ABORT - try already active"); \
        abort(); \
    } \
    if (!setjmp(_exception_jump_buf)) { \
        _exception_data.active = true; \
        {try_clause} \
        _exception_data.active = false; \
    } else { \
        _exception_data.active = false; \
        int exception = _exception_data.code; \
        char* file = (char*) _exception_data.file; \
        char* function = (char*) _exception_data.function; \
        int line = _exception_data.line; \
        {catch_clause} \
    } \
}
#endif // not DISABLE_EXCEPTION_HANDLING

#ifdef DISABLE_EXCEPTION_HANDLING
#define THROW_EXCEPTION_WITH_MSG(exception_code, args...) \
do { \
    fprintf(stderr, "unhandled %s exception at %s(%s:%d): ", exception_code_to_string(exception_code), __FUNCTION__, __FILE__, __LINE__); \
    fprintf(stderr, args); \
    fprintf(stderr, "\n"); \
    fflush(stderr); \
    abort(); \
} while (0)

#define TRY_CATCH(try_clause, catch_clause) {try_clause}
#endif // DISABLE_EXCEPTION_HANDLING

#define THROW_EXCEPTION(exception_code) THROW_EXCEPTION_WITH_MSG(exception_code, "%s", exception_code_to_string(exception_code))

const char *exception_code_to_string(exception_t ex);

#endif // WASM_INTERPRETER_EXCEPTION_H
