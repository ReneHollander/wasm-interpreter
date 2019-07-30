#ifndef WASM_INTERPRETER_EXCEPTION_H
#define WASM_INTERPRETER_EXCEPTION_H

#include <stdbool.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdlib.h>

extern jmp_buf _exception_jump_buf;
extern bool _exception_active;
extern volatile int _exception_code;

#define THROW_EXCEPTION(code) \
do { \
    if (!_exception_active) { \
        write(2, (char *) "ABORT - not in try catch", sizeof("ABORT - not in try catch") - 1); \
        abort(); \
    } \
    _exception_code = code; \
    longjmp(_exception_jump_buf, 1); \
} while (0)

#define TRY_CATCH(try_clause, data, catch_clause) \
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
        volatile int code = _exception_code; \
        {catch_clause} \
    } \
}\

#endif // WASM_INTERPRETER_EXCEPTION_H
