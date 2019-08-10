#include <stdio.h>

#include "exception.h"

int smaller_or_equal_to_ten(int i) {
    if (i > 10) {
        THROW_EXCEPTION(EXCEPTION_PARSER_VERSION_NOT_SUPPORTED);
    } else {
        return 5;
    }
}

int main() {
    TRY_CATCH
    ({
         printf("5: %d\n", smaller_or_equal_to_ten(5));
         printf("15: %d\n", smaller_or_equal_to_ten(15));
     }, {
         printf("%s exception at %s:%d in %s", exception_code_to_string(exception), file, line, function);
     })

    return 0;
}
