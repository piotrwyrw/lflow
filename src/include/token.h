#ifndef LFLOW_TOKEN_H
#define LFLOW_TOKEN_H

typedef enum {
    // Core types
    TT_UNKNOWN,
    TT_IDEN,

    // Literals
    TT_LSTRING,
    TT_LINT,
    TT_LFLOAT,

    // Symbols
    TT_LPAREN,          // (
    TT_RPAREN,          // )
    TT_LSBRACKET,       // [
    TT_RSBRACKET,       // ]
    TT_LBRACKET,        // {
    TT_RBRACKET,        // }
    TT_EQUALS,          // =
    TT_PLUS,            // +
    TT_MINUS,           // -
    TT_ASTERISK,        // *
    TT_SLASH,           // /
    TT_COLON,           // :
    TT_SEMI,            // ;
    TT_BACKSLASH,       // '\'
    TT_AMPERSAND,       // &
    TT_VERTBAR,         // |
    TT_LGREATER,        // >
    TT_RGREATER,        // <

    // Complex tokens
    TT_POINT_RIGHT,     // ->
    TT_POINT_LEFT,      // <-
    TT_DOUBLE_EQUALS,   // ==
    TT_AND_AND,         // &&
    TT_OR_OR            // ||

} TokenType;

const char *TokenType_String(TokenType);
TokenType TokenType_Leading(char);

typedef struct {
    char *value;
    unsigned int length;
    TokenType type;
} Token;

Token *Token_Create(char *, TokenType);
void Token_Destroy(Token *);

#endif