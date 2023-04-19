#ifndef LFLOW_PARSE_H
#define LFLOW_PARSE_H

#include <stdio.h>

#include "tokenizer.h"
#include "ast.h"

#define SYNTAX_ERR(...) \
        printf("Natron -> "); \
        printf(__VA_ARGS__);

#define WARN(...) \
        printf("Warning: "); \
        printf(__VA_ARGS__);

typedef struct {
    Tokenizer *tokenizer;
    Token *current;
    Token *next;

    Node *lastBlock;
    Node *rootBlock;
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
Node *Parser_ParseVariableReference(Parser *);

Node *Parser_ParseFunctionCall(Parser *);
Node *Parser_ParseVariableDeclaration(Parser *);
Node *Parser_ParseVariableAssignment(Parser *);

Node *Parser_ParseSubExpression(Parser *);
Node *Parser_ParseExpression(Parser *);
Node *Parser_ParseSecondDegree(Parser *);
Node *Parser_ParseAtom(Parser *);
Node *Parser_ParseBlock(Parser *);
Node *Parser_ParseFunctionDefinition(Parser *);
Node *Parser_ParseReturn(Parser *);
Node *Parser_ParseCheck(Parser *);
Node *Parser_ParseExternalReference(Parser *);
Node *Parser_ParseSize(Parser *);

#endif