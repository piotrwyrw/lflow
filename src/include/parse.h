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

typedef enum {
    CURRENT,
    NEXT
} TokenDesignation;

Parser *Parser_CreateParser(Tokenizer *);
void Parser_DestroyParser(Parser *);

void Parser_Consume(Parser *);

bool Parser_Compare(Parser *, TokenDesignation, TokenType, const char *);

Node *Parser_ParseProgram(Parser *);
Node *Parser_ParseNext(Parser *);

Node *Parser_ParseStringLiteral(Parser *);
Node *Parser_ParseIntegerLiteral(Parser *);
Node *Parser_ParseRealLiteral(Parser *);

Node *Parser_ParseFunctionCall(Parser *);
Node *Parser_ParseVariableDeclaration(Parser *);

Node *Parser_ParseExpression(Parser *);
Node *Parser_ParseSecondDegree(Parser *);
Node *Parser_ParseAtom(Parser *);

#endif