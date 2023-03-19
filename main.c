#include <stdio.h>
#include <stdlib.h>

#include "src/include/tokenizer.h"
#include "src/include/io.h"

int main(void) {
    char *str = read_file("main.flow");

    if (!str) {
        printf("Failed to read file.\n");
        return 0;
    }

    char *primed = Tokenizer_Prime(str);
    free(str);

    Tokenizer *tokenizer = Tokenizer_Create(primed);

    while (Tokenizer_HasNext(tokenizer)) {
        if (!Tokenizer_Next(tokenizer))
            break;
        printf("[%s] %s\n", TokenType_String(tokenizer->current->type), tokenizer->current->value);
    }

    Tokenizer_Destroy(tokenizer);

    free(primed);

    return 0;
}