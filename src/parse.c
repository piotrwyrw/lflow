#include "include/parse.h"
#include "include/conv.h"

#include <stdlib.h>
#include <string.h>

Parser *Parser_CreateParser(Tokenizer *tokenizer) {
    Parser *parser = malloc(sizeof(Parser));
    parser->tokenizer = tokenizer;
    parser->current = NULL;
    parser->next = NULL;

    Parser_Consume(parser);
    Parser_Consume(parser);

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
    parser->next = Token_Dup(parser->tokenizer->current);
}

bool Parser_Compare(Parser *p, TokenDesignation td, TokenType tt, const char *str) {
    Token *tok = (td == CURRENT) ? p->current : p->next;
    if (!tok)
        return false;
    if (str != NULL)
        return (strcmp(tok->value, str) == 0) && tok->type == tt;
    else
        return tok->type == tt;
}

Node *Parser_ParseProgram(Parser *parser) {
    Array *arr = Array_Create();

    while (true) {
        if (!Tokenizer_HasNext(parser->tokenizer))
            break;

        Node *n = Parser_ParseNext(parser);

        if (n == NULL) {
            Array_DestroyCallBack(arr, (void *) Node_DestroyRecurse);
            return NULL;
        }

        Array_Push(arr, n);
    }

    return Node_CreateProgram(arr);
}

Node *Parser_ParseNext(Parser *parser) {
    Node *n = Parser_ParseExpression(parser);
    if (!Parser_Compare(parser, CURRENT, TT_SEMI, NULL)) {
        SYNTAX_ERR("Expected ';' after expression.\n");
        if (n)
            Node_DestroyRecurse(n);
        return NULL;
    }
    Parser_Consume(parser); // Skip ';'
    return n;
}

Node *Parser_ParseStringLiteral(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_LSTRING, NULL)) {
        SYNTAX_ERR("Expected string literal, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }
    Node *lit = Node_CreateStringLiteral(parser->current->value);
    Parser_Consume(parser); // Next token
    return lit;
}

Node *Parser_ParseIntegerLiteral(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_LINT, NULL)) {
        SYNTAX_ERR("Expected integer literal, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }
    Node *lit = Node_CreateIntegerLiteral(stoi(parser->current->value));
    Parser_Consume(parser); // Next token
    return lit;
}

Node *Parser_ParseRealLiteral(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_LFLOAT, NULL)) {
        SYNTAX_ERR("Expected float literal, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }
    Node *lit = Node_CreateFloatLiteral(stof(parser->current->value));
    Parser_Consume(parser); // Next token
    return lit;
}

// identifier "(" expression ["," expression] ... ")"
Node *Parser_ParseFunctionCall(Parser *parser) {

    Token *identifier;

    // identifier
    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected identifier, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }

    identifier = Token_Dup(parser->current);

    Parser_Consume(parser);

    // (
    if (!Parser_Compare(parser, CURRENT, TT_LPAREN, NULL)) {
        SYNTAX_ERR("Expected '(' after identifier \"\"");
        Token_Destroy(identifier);
        return NULL;
    }

    Parser_Consume(parser);

    Array *exprs = Array_Create();

    // Parameters (expr. seq.)
    while (!Parser_Compare(parser, CURRENT, TT_RPAREN, NULL) && parser->current != NULL) {
        // Expression
        Node *expr = Parser_ParseExpression(parser);

        Array_Push(exprs, expr);

//         ',' or ')'
        if (Parser_Compare(parser, CURRENT, TT_COMMA, NULL) && !Parser_Compare(parser, NEXT, TT_RPAREN, NULL)) {
            Parser_Consume(parser);
            continue;
        }

        if (Parser_Compare(parser, CURRENT, TT_RPAREN, NULL))
            continue;

        SYNTAX_ERR("Expected ')' or ',' and more expressions.\n");
        Token_Destroy(identifier);
        Array_DestroyCallBack(exprs, (void *) Node_DestroyRecurse);
        return NULL;

    }

    if (!Parser_Compare(parser, CURRENT, TT_RPAREN, NULL)) {
        SYNTAX_ERR("Expected ')' after the list of expressions. Got \"%s\"\n", parser->current->value);

        Token_Destroy(identifier);
        Array_DestroyCallBack(exprs, (void *) Node_DestroyRecurse);

        return NULL;
    }

    Parser_Consume(parser); // SKip ')'

    Node *fcall = Node_CreateFunctionCall(identifier, exprs);
    Token_Destroy(identifier);

    return fcall;
}

Node *Parser_ParseExpression(Parser *parser) {
    Node *left = Parser_ParseSecondDegree(parser);

    if (!left)
        return NULL;

    // Addition / Subtraction
    while (Parser_Compare(parser, CURRENT, TT_PLUS, NULL) || Parser_Compare(parser, CURRENT, TT_MINUS, NULL)) {
        BinaryType type = BinaryType_FromTokenType(parser->current->type);

        if (type == BIN_UNDEF) {
            SYNTAX_ERR("Unknown binary operation \"%s\".\n", parser->current->value);
            Node_DestroyRecurse(left);
            return NULL;
        }

        Parser_Consume(parser); // next token, skip operation

        Node *right = Parser_ParseExpression(parser);

        if (!right)
            return NULL;

        left = Node_CreateBinaryOperation(left, right, type);
    }

    return left;
}

Node *Parser_ParseSecondDegree(Parser *parser) {
    Node *left = Parser_ParseAtom(parser);

    if (!left)
        return NULL;

    // Multiplication / Division
    while (Parser_Compare(parser, CURRENT, TT_ASTERISK, NULL) || Parser_Compare(parser, CURRENT, TT_SLASH, NULL)) {
        BinaryType type = BinaryType_FromTokenType(parser->current->type);

        if (type == BIN_UNDEF) {
            SYNTAX_ERR("Unknown binary operation \"%s\".\n", parser->current->value);
            Node_DestroyRecurse(left);
            return NULL;
        }

        Parser_Consume(parser); // next token, skip operation

        Node *right = Parser_ParseSecondDegree(parser);

        if (!right)
            return NULL;

        left = Node_CreateBinaryOperation(left, right, type);
    }

    return left;
}

Node *Parser_ParseAtom(Parser *parser) {

    // Sub-expression
    if (Parser_Compare(parser, CURRENT, TT_LPAREN, NULL)) {
        Parser_Consume(parser); // SKip the '('
        Node *expr = Parser_ParseExpression(parser);
        if (!Parser_Compare(parser, CURRENT, TT_RPAREN, NULL)) {
            SYNTAX_ERR("Expected ')' after sub-expression.\n");
            Node_DestroyRecurse(expr);
            return NULL;
        }
        Parser_Consume(parser);
        return expr;
    }

    // Function call
    if (Parser_Compare(parser, CURRENT, TT_IDEN, NULL) && Parser_Compare(parser, NEXT, TT_LPAREN, NULL)) {
        return Parser_ParseFunctionCall(parser);
    }

    // Integer Literal
    if (Parser_Compare(parser, CURRENT, TT_LINT, NULL)) {
        return Parser_ParseIntegerLiteral(parser);
    }

    // Real Literal
    if (Parser_Compare(parser, CURRENT, TT_LFLOAT, NULL)) {
        return Parser_ParseRealLiteral(parser);
    }

    // String Literal
    if (Parser_Compare(parser, CURRENT, TT_LSTRING, NULL)) {
        return Parser_ParseStringLiteral(parser);
    }

    SYNTAX_ERR("Unknown atom starting with \"%s\".\n", parser->current->value);

    return NULL;
}
