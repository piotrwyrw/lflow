#include "include/util.h"

#include <stdlib.h>
#include <string.h>

char *repeat(char c, unsigned n) {
    char *str = malloc(n + 1);
    for (unsigned i = 0; i < n; i ++)
        str[i] = c;
    return str;
}

const char *indent(unsigned n) {
    char *str = repeat('\t', n);
    char *ss = stack(str);
    free(str);
    return ss;
}

const char *stack(char *s) {
    unsigned le = strlen(s);
    char str[le + 1];
    memset(str, 0, le + 1);
    strcpy(str, s);
    return str;
}