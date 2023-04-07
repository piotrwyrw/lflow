#include "include/util.h"

#include <stdlib.h>
#include <string.h>

char *repeat(char c, unsigned n) {
    char *str = malloc(n + 1);
    memset(str, 0, n + 1);
    for (unsigned i = 0; i < n; i ++)
        str[i] = c;
    return str;
}

char *indent(unsigned n) {
    char *str = repeat('\t', n);
    return str;
}