#include <stdio.h>

#include "exception.h"

int smaller_or_equal_to_ten(int i) {
    if (i > 10) {
        THROW_EXCEPTION(90);
    } else {
        return 5;
    }
}

int main() {
    TRY_CATCH
    ({
         printf("5: %d\n", smaller_or_equal_to_ten(5));
         printf("15: %d\n", smaller_or_equal_to_ten(15));
     }, code, {
         printf("error occured: %d", code);
     })

    return 0;
}
