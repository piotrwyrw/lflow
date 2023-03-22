#ifndef LFLOW_PARSE_H
#define LFLOW_PARSE_H

#include "tokenizer.h"

typedef struct {
    Tokenizer *tokenizer;
    Token *current;
    Token *next;
} Parser;

Parser *Parser_CreateParser(Tokenizer *);
void Parser_DestroyParser(Parser *);

void Parser_Consume(Parser *);

#endif