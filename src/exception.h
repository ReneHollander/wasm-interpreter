#ifndef WASM_INTERPRETER_EXCEPTION_H
#define WASM_INTERPRETER_EXCEPTION_H

#include <stdbool.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

typedef enum exception {
    NO_EXCEPTION = 0,
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

typedef struct _exception_data {
    exception_t code;
    char file[1024];
    char function[1024];
    int line;
} _exception_data_t;

extern jmp_buf _exception_jump_buf;
extern bool _exception_active;
extern volatile _exception_data_t _exception_data;

#define THROW_EXCEPTION(exception_code) \
do { \
    if (!_exception_active) { \
        write(2, (char *) "unhandled exception", sizeof("unhandled exception") - 1); \
        abort(); \
    } \
    _exception_data.code = exception_code; \
    strncpy((char *) _exception_data.file, __FILE__, sizeof(_exception_data.file)); \
    strncpy((char *) _exception_data.function, __FUNCTION__, sizeof(_exception_data.function)); \
    _exception_data.line = __LINE__; \
    longjmp(_exception_jump_buf, 1); \
} while (0)

#define TRY_CATCH(try_clause, catch_clause) \
{\
    if (_exception_active == true) { \
        write(2, (char *) "ABORT - try already active", sizeof("ABORT - try already active") - 1); \
        abort(); \
    } \
    if (!setjmp(_exception_jump_buf)) { \
        _exception_active = true; \
        {try_clause} \
    } else { \
        _exception_active = false; \
        volatile int exception = _exception_data.code; \
        volatile char* file = _exception_data.file; \
        volatile char* function = _exception_data.function; \
        volatile int line = _exception_data.line; \
        {catch_clause} \
    } \
}\


char *exception_code_to_string(exception_t ex);

#endif // WASM_INTERPRETER_EXCEPTION_H
