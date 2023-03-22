#include "include/arr.h"

#include <stdlib.h>

Array *Array_Create() {
    Array *arr = malloc(sizeof(Array));
    arr->base = malloc(0);
    arr->length = 0;
    return arr;
}

void Array_Destroy(Array *arr) {
    free(arr->base);
    free(arr);
}

void Array_Push(Array *arr, void *ptr) {
    realloc(arr->base, (++ arr->length) * sizeof(void *));
    arr->base[arr->length - 1] = ptr;
}

void *Array_At(Array *arr, int i) {
    if (i >= arr->length)
        return NULL;

    return arr->base[i];
}