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

    Node *n = Parser_ParseProgram(parser);

    if (n) {
        Node_Print(0, n);
        Node_DestroyRecurse(n);
    }

    Parser_DestroyParser(parser);

    Tokenizer_Destroy(tokenizer);

    free(primed);

    return 0;
}