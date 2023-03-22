#ifndef LFLOW_PARSE_H
#define LFLOW_PARSE_H

#include <stdio.h>

#include "tokenizer.h"
#include "ast.h"

#define SYNTAX_ERR(...) \
        printf("Syntax Error: "); \
        printf(__VA_ARGS__);

typedef struct {
    Tokenizer *tokenizer;
    Token *current;
    Token *next;
} Parser;

Parser *Parser_CreateParser(Tokenizer *);
void Parser_DestroyParser(Parser *);

void Parser_Consume(Parser *);

Node *Parser_ParseStringLiteral(Parser *);

#endif