#include <assert.h>

#include "value.h"
#include "vec.h"

CREATE_VEC(u32, u32)

char *stringify(vec_u32_t *vec) {
    char *res = malloc(12);
    res[0] = '[';
    res[1] = '\0';
    for (int i = 0; i < vec_u32_length(vec); i++) {
        u32 x = vec_u32_get(vec, i);
        int length = snprintf(NULL, 0, "%s%d", res, x) + 2;
        res = realloc(res, length + 1);
        snprintf(res + strlen(res), length + 1, "%d", x);
        if (i == vec_u32_length(vec) - 1) {
            break;
        }
        strcat(res, ", ");
    }
    strcat(res, "]");
    return res;
}

void check(vec_u32_t *vec, char *str) {
    char *arrstr = stringify(vec);
    if (strcmp(arrstr, str) != 0) {
        fprintf(stderr, "Expected %s, got %s\n", str, arrstr);
        exit(1);
    }
    free(arrstr);
}

int main() {
    vec_u32_t *vec = vec_u32_create();
    assert(vec != NULL);
    check(vec, "[]");

    assert(vec_u32_empty(vec) == true);
    check(vec, "[]");

    vec_u32_add(vec, 15);
    check(vec, "[15]");
    vec_u32_add(vec, 32);
    check(vec, "[15, 32]");
    vec_u32_add(vec, 64);
    check(vec, "[15, 32, 64]");

    assert(vec_u32_empty(vec) == false);
    check(vec, "[15, 32, 64]");

    assert(vec_u32_peek(vec) == 64);
    check(vec, "[15, 32, 64]");

    assert(vec_u32_get(vec, 0) == 15);
    check(vec, "[15, 32, 64]");
    assert(vec_u32_get(vec, 1) == 32);
    check(vec, "[15, 32, 64]");
    assert(vec_u32_get(vec, 2) == 64);
    check(vec, "[15, 32, 64]");
    assert(vec_u32_length(vec) == 3);
    check(vec, "[15, 32, 64]");

    vec_u32_set(vec, 5, 123);
    check(vec, "[15, 32, 64, 0, 0, 123]");
    assert(vec_u32_get(vec, 5) == 123);
    check(vec, "[15, 32, 64, 0, 0, 123]");
    assert(vec_u32_length(vec) == 6);
    check(vec, "[15, 32, 64, 0, 0, 123]");

    vec_u32_set(vec, 3, 764);
    check(vec, "[15, 32, 64, 764, 0, 123]");
    assert(vec_u32_get(vec, 3) == 764);
    check(vec, "[15, 32, 64, 764, 0, 123]");
    assert(vec_u32_length(vec) == 6);
    check(vec, "[15, 32, 64, 764, 0, 123]");

    assert(vec_u32_pop(vec) == 123);
    check(vec, "[15, 32, 64, 764, 0]");
    assert(vec_u32_length(vec) == 5);
    check(vec, "[15, 32, 64, 764, 0]");

    assert(vec_u32_remove(vec, 1) == 32);
    check(vec, "[15, 64, 764, 0]");
    assert(vec_u32_length(vec) == 4);
    check(vec, "[15, 64, 764, 0]");

    assert(vec_u32_remove(vec, 3) == 0);
    check(vec, "[15, 64, 764]");
    assert(vec_u32_length(vec) == 3);
    check(vec, "[15, 64, 764]");

    vec_u32_iterator_t it = vec_u32_iterator(vec);
    assert(vec_u32_has_next(&it) == true);
    assert(vec_u32_next(&it) == 15);
    assert(vec_u32_has_next(&it) == true);
    assert(vec_u32_next(&it) == 64);
    assert(vec_u32_has_next(&it) == true);
    assert(vec_u32_next(&it) == 764);
    assert(vec_u32_has_next(&it) == false);
    check(vec, "[15, 64, 764]");

    vec_u32_insert(vec, 1, 8);
    check(vec, "[15, 8, 64, 764]");
    assert(vec_u32_length(vec) == 4);

    vec_u32_insert(vec, 0, 457);
    check(vec, "[457, 15, 8, 64, 764]");
    assert(vec_u32_length(vec) == 5);

    vec_u32_insert(vec, 4, 5);
    check(vec, "[457, 15, 8, 64, 5, 764]");
    assert(vec_u32_length(vec) == 6);

    vec_u32_insert(vec, 8, 542);
    check(vec, "[457, 15, 8, 64, 5, 764, 0, 0, 542]");
    assert(vec_u32_length(vec) == 9);

    vec_u32_free(vec);
}