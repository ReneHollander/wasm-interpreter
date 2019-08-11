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

        [EXCEPTION_INTERPRETER_ONLY_ONE_RETURN_VALUE_ALLOWED] = "interpreter only one return value allowed",
        [EXCEPTION_INTERPRETER_OPERAND_STACK_NOT_EMPTY] = "interpreter operand stack not empty",
        [EXCEPTION_INTERPRETER_INVALID_SECTION] = "interpreter invalid section",
        [EXCEPTION_INTERPRETER_NO_MEMORY] = "interpreter no memory",
        [EXCEPTION_INTERPRETER_INVALID_MEMORY] = "interpreter invalid memory",
        [EXCEPTION_INTERPRETER_INVALID_IMPORT] = "interpreter invalid import",
        [EXCEPTION_INTERPRETER_INVALID_INSTRUCTION] = "interpreter invalid instruction",
        [EXCEPTION_INTERPRETER_REACHED_OP_UNREACHABLE] = "interpreter reached op unreachable",
        [EXCEPTION_INTERPRETER_UNINITIALIZED] = "interpreter uninitialized",
        [EXCEPTION_INTERPRETER_MEMORY_ACCESS_OUT_OF_BOUNDS] = "interpreter memory access out of bounds",
        [EXCEPTION_INTERPRETER_NOT_FOUND] = "interpreter not found",
};

const char *exception_code_to_string(exception_t ex) {
    return exception_to_string[ex];
}