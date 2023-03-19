#include "include/xstring.h"

#include <stdlib.h>
#include <string.h>

XString *XString_Create() {
    XString *xs = malloc(sizeof(XString));
    xs->length = 0;
    xs->str = malloc(1);
    (xs->str)[0] = 0;
    return xs;
}

void XString_Destroy(XString *xs) {
    free(xs->str);
    free(xs);
}

void XString_Append(XString *xs, char c) {
    char *ptr = malloc(xs->length + 2);
    strcpy(ptr, xs->str);
    ptr[xs->length] = c;
    ptr[xs->length + 1] = 0;
    free(xs->str);
    xs->str = ptr;
    xs->length ++;
}

char XString_Last(XString *xs) {
    if (xs->length >= 1)
        return xs->str[xs->length - 1];
    return 0;
}