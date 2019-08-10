#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "parser.h"
#include "interpreter.h"
#include "memory.h"
#include "variable.h"

char *prg_name;

void parse_error(char *msg) {
    fprintf(stderr, "Error parsing module: %s\n", msg);
    exit(EXIT_FAILURE);
}

void usage() {
    fprintf(stderr, "Usage: %s <-p name> <-f name> [-a type:value]...\n", prg_name);
    exit(EXIT_FAILURE);
}

void parse_arg(char *str, parameter_value_t *param) {
    char *type_str = strtok(str, ":");
    char *value_str = strtok(NULL, ":");
    if (strcmp(type_str, "i32") == 0) {
        param->type = VALTYPE_I32;
        param->val.i32 = (i32) strtoul(value_str, NULL, 10);
    } else if (strcmp(type_str, "i64") == 0) {
        param->type = VALTYPE_I64;
        param->val.i64 = (i64) strtoul(value_str, NULL, 10);
    } else if (strcmp(type_str, "f32") == 0) {
        param->type = VALTYPE_F32;
        param->val.i32 = strtoul(value_str, NULL, 10);
    } else if (strcmp(type_str, "f64") == 0) {
        param->type = VALTYPE_F64;
        param->val.i64 = strtoul(value_str, NULL, 10);
    } else {
        fprintf(stderr, "Unknown parameter type %s\n", type_str);
    }

}

int main(int argc, char *argv[]) {
    prg_name = argv[0];

    parameter_value_t a;
    vec_parameter_value_t *parameters = vec_parameter_value_create();

    char *module_path = NULL;
    char *function = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "p:f:a:")) != -1) {
        switch (opt) {
            case 'p':
                if (module_path != NULL) {
                    usage();
                }
                module_path = strdup(optarg);
                break;
            case 'f':
                if (function != NULL) {
                    usage();
                }
                function = strdup(optarg);
                break;
            case 'a':
                parse_arg(strdup(optarg), &a);
                vec_parameter_value_add(parameters, a);
                break;
            default: /* '?' */
                usage();
        }
    }

    if (optind < argc) {
        usage();
    }

    FILE *input = fopen(module_path, "r");
    if (input == NULL) {
        fprintf(stderr, "Error opening input file %s: %s", module_path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    module_t *module = NULL;
    exception_t ex = parse(input, &module);
    if (ex > 0) {
        fprintf(stderr, "error parsing file: %s", exception_code_to_string(ex));
        exit(1);
    }
    fclose(input);

    eval_state_t *interpreter = create_interpreter();
    interpreter->module = module;
    init_interpreter(interpreter);

    return_value_t ret;
    ex = interpret_function(interpreter, function, parameters, &ret);
    if (ex > 0) {
        fprintf(stderr, "error interpreting: %s", exception_code_to_string(ex));
        exit(1);
    }

    if (ret.is_void) {
        printf("void");
    } else {
        switch (ret.type) {
            case VALTYPE_I32:
                printf("i32:%u", ret.val.i32);
                break;
            case VALTYPE_I64:
                printf("i64:%lu", ret.val.i64);
                break;
            case VALTYPE_F32:
                printf("f32:%u", ret.val.i32);
                break;
            case VALTYPE_F64:
                printf("f64:%lu", ret.val.i64);
                break;
            default:
                fprintf(stderr, "unknown return valtype");
                interpreter_exit(NULL);
        }
    }

    free_interpreter(interpreter);

    return 0;
}
