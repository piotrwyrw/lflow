#include "include/token.h"

#include <stdlib.h>
#include <string.h>

#define AUTO_CASE(e) \
    case e: \
        return #e;

const char *TokenType_String(TokenType type) {
    switch (type) {
        AUTO_CASE(TT_UNKNOWN)
        AUTO_CASE(TT_IDEN)

        AUTO_CASE(TT_LSTRING)
        AUTO_CASE(TT_LINT)
        AUTO_CASE(TT_LFLOAT)

        AUTO_CASE(TT_LPAREN)
        AUTO_CASE(TT_RPAREN)
        AUTO_CASE(TT_LSBRACKET)
        AUTO_CASE(TT_RSBRACKET)
        AUTO_CASE(TT_LBRACKET)
        AUTO_CASE(TT_RBRACKET)
        AUTO_CASE(TT_EQUALS)
        AUTO_CASE(TT_PLUS)
        AUTO_CASE(TT_MINUS)
        AUTO_CASE(TT_ASTERISK)
        AUTO_CASE(TT_SLASH)
        AUTO_CASE(TT_COLON)
        AUTO_CASE(TT_SEMI)
        AUTO_CASE(TT_BACKSLASH)
        AUTO_CASE(TT_AMPERSAND)
        AUTO_CASE(TT_VERTBAR)
        AUTO_CASE(TT_LGREATER)
        AUTO_CASE(TT_RGREATER)
        AUTO_CASE(TT_COMMA)
        AUTO_CASE(TT_DOLLAR_SIGN)

        AUTO_CASE(TT_POINT_RIGHT)
        AUTO_CASE(TT_POINT_LEFT)
        AUTO_CASE(TT_DOUBLE_EQUALS)
        AUTO_CASE(TT_NOT_EQUALS)
        AUTO_CASE(TT_AND_AND)
        AUTO_CASE(TT_OR_OR)

        AUTO_CASE(TT_KW_PROCEDURE)
        AUTO_CASE(TT_KW_CHECK)
        AUTO_CASE(TT_KW_VARYING)
        AUTO_CASE(TT_KW_CONSTANT)
        AUTO_CASE(TT_KW_JMP)
        AUTO_CASE(TT_KW_OTHERWISE)
        AUTO_CASE(TT_KW_RETURN)
        AUTO_CASE(TT_KW_SIZE)
        AUTO_CASE(TT_KW_EXTERN)

        default:
            return "(Unknown type)";
    }
}

#undef AUTO_CASE

Token *Token_Create(char *val, TokenType type) {
    Token *tok = malloc(sizeof(Token));
    tok->value = malloc(strlen(val) + 1);
    strcpy(tok->value, val);
    tok->length = strlen(val);
    tok->type = type;
    return tok;
}

void Token_Destroy(Token *tok) {
    free(tok->value);
    free(tok);
}

#define BIND(c, t) case c: return t;

TokenType TokenType_Leading(char c) {
    switch (c) {
        BIND('(', TT_LPAREN)
        BIND(')', TT_RPAREN)
        BIND('[', TT_LSBRACKET)
        BIND(']', TT_RSBRACKET)
        BIND('{', TT_LBRACKET)
        BIND('}', TT_RBRACKET)
        BIND('=', TT_EQUALS)
        BIND('+', TT_PLUS)
        BIND('-', TT_MINUS)
        BIND('*', TT_ASTERISK)
        BIND('/', TT_SLASH)
        BIND(':', TT_COLON)
        BIND(';', TT_SEMI)
        BIND('\\', TT_BACKSLASH)
        BIND('&', TT_AMPERSAND)
        BIND('|', TT_VERTBAR)
        BIND('>', TT_LGREATER)
        BIND('<', TT_RGREATER)
        BIND(',', TT_COMMA)
        BIND('$', TT_DOLLAR_SIGN)
        default:
            return TT_UNKNOWN;
    }
}

#undef BIND

Token *Token_Dup(Token *token) {
    return Token_Create(token->value, token->type);
}

bool Token_Cmp(Token *a, Token *b) {
    if (!a || !b)
        return false;
    return strcmp(a->value, b->value) == 0;
}