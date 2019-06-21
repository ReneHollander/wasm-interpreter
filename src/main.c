#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "parser.h"
#include "interpreter.h"
#include "memory.h"

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

    memory_t *mem = create_memory(1); // Create one page of memory (65535 bytes).

    ((i32 *) mem->data)[0] = 5;

    use_memory(mem);

    module_t *module = parse(input, parse_error);
    fclose(input);

    if (argc == 3) {
        return_value_t ret = interpret_function(module, argv[2], NULL);

        if (ret.type == 0) {
            printf("void");
        } else {
            switch (ret.type) {
                case VALTYPE_I32:
                    printf("return value: i32:%d\n", ret.val.i32);
                    break;
                case VALTYPE_I64:
                    printf("return value: i64:%ld\n", ret.val.i64);
                    break;
                case VALTYPE_F32:
                    printf("return value: f32:%f\n", ret.val.f32);
                    break;
                case VALTYPE_F64:
                    printf("return value: f64:%f\n", ret.val.f64);
                    break;
                default:
                    fprintf(stderr, "unknown return valtype");
                    interpreter_exit(NULL);
            }
        }
    }

    i32 res0 = ((i32 *) mem->data)[0];
    i32 res1 = ((i32 *) mem->data)[1];

    printf("res0=%d, res1=%d\n", res0, res1);

    free_memory(mem);

    return 0;
}
