#include <stdio.h>
#include <stdlib.h>

#include "src/include/tokenizer.h"
#include "src/include/io.h"
#include "src/include/parse.h"
#include "src/include/semantic.h"

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

    Tokenizer_Destroy(tokenizer);

    if (n) {
        printf("Natron -> Syntactic analysis successful.\n");
        Node_Print(0, n);
        SemanticAnalysis *sa = SemanticAnalysis_Create(n);
        if (SemanticAnalysis_RunAnalysis(sa) == STATUS_FAIL) {
            printf("Notamide -> Semantic analysis failed.\n");
        } else {
            printf("Notamide -> Semantic analysis OK.\n");
        }

        Node_DestroyRecurse(n);
        SemanticAnalysis_Destroy(sa);
    } else {
        printf("Natron -> Parsing failed.\n");
    }

    Parser_DestroyParser(parser);

    free(primed);

    return 0;
}