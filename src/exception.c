#include "exception.h"

#ifndef DISABLE_EXCEPTION_HANDLING
jmp_buf _exception_jump_buf;
volatile _exception_data_t _exception_data = {
        .active = false,
        .code = 0,
        .file = "",
        .function = "",
        .line = 0,
        .message = "",
};
#endif // DISABLE_EXCEPTION_HANDLING

const char *exception_to_string[] = {
        [NO_EXCEPTION] = "",

        [EXCEPTION_VECTOR_EXCEPTION] = "vector exception",

        [EXCEPTION_PARSER_MEMORY_INDEX_NOT_ZERO] = "parser memory index not zero",
        [EXCEPTION_PARSER_UNEXPECTED_VALTYPE] = "parser unexpected valtype",
        [EXCEPTION_PARSER_EOF_BEFORE_FINISHED] = "parser reached eof before finishing",
        [EXCEPTION_PARSER_LEB_OVERFLOW] = "parser leb overflow",
        [EXCEPTION_PARSER_INVALID_TYPEIDX_FOR_CALL_INDIRECT] = "parser invalid typeidx for call indirect",
        [EXCEPTION_PARSER_UNKNOWN_OPCODE] = "parser unknown opcode",
        [EXCEPTION_PARSER_UNKNOWN_LIMIT_TYPE] = "parser unknown limit type",
        [EXCEPTION_PARSER_UNEXPECTED_OPCODE] = "parser unexpected opcode",
        [EXCEPTION_PARSER_UNKNOWN_ELEMENT_TYPE] = "parser unknown element type",
        [EXCEPTION_PARSER_UNKNOWN_MUTABILITY_TYPE] = "parser unknown mutability type",
        [EXCEPTION_PARSER_UNKNOWN_IMPORTDESC_TYPE] = "parser unknown importdesc type",
        [EXCEPTION_PARSER_UNKNOWN_EXPORTDESC_TYPE] = "parser unknown exportdesc type",
        [EXCEPTION_PARSER_INVALID_SECTION_ID] = "parser invalid section id",
        [EXCEPTION_PARSER_UNKNOWN_SECTION_TYPE] = "parser unknown section type",
        [EXCEPTION_PARSER_UNKNOWN_MAGIC_VALUE] = "parser unknown magic value",
        [EXCEPTION_PARSER_VERSION_NOT_SUPPORTED] = "parser version not supported",
};

const char *exception_code_to_string(exception_t ex) {
    return exception_to_string[ex];
}