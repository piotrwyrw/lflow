#include "include/token.h"

#include <stdlib.h>
#include <string.h>

#define AUTO_CASE(e) \
    case e: \
        return #e;

char *TokenType_String(TokenType type) {
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

        AUTO_CASE(TT_POINT_RIGHT)
        AUTO_CASE(TT_POINT_LEFT)
        AUTO_CASE(TT_DOUBLE_EQUALS)
        AUTO_CASE(TT_AND_AND)
        AUTO_CASE(TT_OR_OR)

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
    if (!tok)
        return;
    if (tok->value)
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
        default:
            return TT_UNKNOWN;
    }
}

#undef BIND