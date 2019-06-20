#ifndef PARSER_H
#define PARSER_H

#include "stdio.h"
#include "module.h"

typedef void (*parse_error_f)(char *msg);

module_t *parse(FILE *input, parse_error_f parse_error);

#endif // PARSER_H
