#include "include/parse.h"

#include <stdlib.h>
#include <string.h>

Parser *Parser_CreateParser(Tokenizer *tokenizer) {
    Parser *parser = malloc(sizeof(Parser));
    parser->tokenizer = tokenizer;

    Tokenizer_Next(tokenizer);
    parser->current = Token_Dup(tokenizer->current);
    Tokenizer_Next(tokenizer);
    parser->next = Token_Dup(tokenizer->current);

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

Node *Parser_ParseStringLiteral(Parser *parser) {
    if (parser->current->type != TT_LINT) {
        SYNTAX_ERR("Expected string literal, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }
    return Node_CreateStringLiteral(strdup(parser->current->value));
}