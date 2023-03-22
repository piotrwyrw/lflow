#include "include/parse.h"

#include <stdlib.h>

Parser *Parser_CreateParser(Tokenizer *tokenizer) {
    Parser *parser = malloc(sizeof(Parser));
    parser->tokenizer = tokenizer;

    Tokenizer_Next(tokenizer);
    parser->current = tokenizer->current;
    Tokenizer_Next(tokenizer);
    parser->next = tokenizer->current;

    return parser;
}

void Parser_DestroyParser(Parser *parser) {
    if (parser->current)
        Token_Destroy(parser->current);

    if (parser->next)
        Token_Destroy(parser->next);

    free(parser);
}

void Parser_Consume(Parser *parser) {
    if (parser->current)
        Token_Destroy(parser->current);
    parser->current = parser->next;
    Tokenizer_Next(parser->tokenizer);
    parser->next = parser->tokenizer->current;
}