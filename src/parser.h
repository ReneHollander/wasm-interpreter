#ifndef PARSER_H
#define PARSER_H

#include "stdio.h"
#include "module.h"
#include "exception.h"

typedef void (*parse_error_f)(char *msg);

exception_t parse(FILE *input_file, module_t **module);

#endif // PARSER_H
