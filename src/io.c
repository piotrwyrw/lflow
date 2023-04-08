#include <stdio.h>
#include <stdlib.h>

#include "include/io.h"

char *read_file(char *path) {
    FILE *file = fopen(path, "r");
    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    unsigned len = ftell(file);
    rewind(file);

    char *buffer = calloc(len + 1, 1);
    fread(buffer, 1, len, file);
    buffer[len] = 0;

    fclose(file);

    return buffer;
}