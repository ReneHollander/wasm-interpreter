#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "parser.h"
#include "interpreter.h"

void parse_error(char *msg) {
    fprintf(stderr, "Error parsing module: %s\n", msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Please specify exactly one input file and optionally one function name!\n");
        exit(EXIT_FAILURE);
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Error opening input file %s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    module_t *module = parse(input);
    fclose(input);

    if (argc == 3) {
        interpret_function(module, argv[2]);
    } else {
        interpret(module);
    }


    return 0;
}
