#include <stdio.h>
#include <stdlib.h>

#include "src/include/tokenizer.h"
#include "src/include/io.h"
#include "src/include/parse.h"

int main(void) {
    char *str = read_file("main.flow");

    if (!str) {
        printf("Failed to read file.\n");
        return 0;
    }

    char *primed = Tokenizer_Prime(str);
    free(str);

    Tokenizer *tokenizer = Tokenizer_Create(primed);

    Parser *parser = Parser_CreateParser(tokenizer);

    Parser_ParseStringLiteral(parser);

    Parser_DestroyParser(parser);

    free(primed);

    return 0;
}