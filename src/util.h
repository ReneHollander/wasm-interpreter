#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <limits.h>

#define CAT(x, y) CAT_(x, y)
#define CAT_(x, y) x ## y

#define BIT_MASK(__TYPE__, __ONE_COUNT__) \
    (((__TYPE__) (-((__ONE_COUNT__) != 0))) \
    & (((__TYPE__) -1) >> ((sizeof(__TYPE__) * CHAR_BIT) - (__ONE_COUNT__))))

#ifdef __GNUC__
#define SILENCE_UNUSED __attribute__ ((unused))
#else
#define SILENCE_UNUSED
#endif

#endif // UTIL_H
