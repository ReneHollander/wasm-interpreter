#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "parser/parser.h"
#include "parser/next.h"

void parse_error(char* msg) {
    fprintf(stderr, "Error parsing module: %s\n", msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Please specify exactly one input file!\n");
        exit(EXIT_FAILURE);
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        fprintf(stderr, "Error opening input file %s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    set_input(input);
    parse();

    fclose(input);
    fprintf(stdout, "Parsing successful\n");
    return 0;
}
