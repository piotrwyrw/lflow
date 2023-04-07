#include "include/tokenizer.h"
#include "include/xstring.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Tokenizer *Tokenizer_Create(char *input) {
    Tokenizer *tokenizer = malloc(sizeof(Tokenizer));
    tokenizer->input = malloc(strlen(input) + 1);
    strcpy(tokenizer->input, input);
    tokenizer->length = strlen(input);
    tokenizer->ix = 0;
    tokenizer->current = Token_Create("", TT_UNKNOWN);
    return tokenizer;
}

void Tokenizer_Destroy(Tokenizer *tokenizer) {
    free(tokenizer->input);
    if (tokenizer->current)
        Token_Destroy(tokenizer->current);
    free(tokenizer);
}

#define LAST_IDX ((tokenizer->ix + 1 > tokenizer->length))
#define NCLASS ((type == TT_UNKNOWN))
#define LETTER(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
#define DIGIT(c) ((c >= '0' && c <= '9'))
#define FAIL \
        XString_Destroy(buffer); \
        return STATUS_FAIL;
#define SPACE(c) (c == ' ' || c == '\t' || c == '\n')

int Tokenizer_HasNext(Tokenizer *tokenizer) {
    if (LAST_IDX)
        return 0;
    return 1;
}

Status Tokenizer_Next(Tokenizer *tokenizer) {
    XString *buffer = XString_Create();
    TokenType type = TT_UNKNOWN;

    int str = 0;

    for (; tokenizer->ix < tokenizer->length; tokenizer->ix++) {

        char c = tokenizer->input[tokenizer->ix];
        char next = LAST_IDX ? -1 : tokenizer->input[tokenizer->ix + 1];

        if (c == '"') {
            type = TT_LSTRING;
            str = !str;
            if (!str) {
                tokenizer->ix++;
                break;
            }
            continue;
        }

        if (str) {
            XString_Append(buffer, c);
            if (LAST_IDX) {
                printf("Unclosed string literal.");
                FAIL;
            }
            continue;
        }

        // Is this a character that we need to skip?
        if (SPACE(c)) {
            // In this case, we might need to drop a token
            if (type != TT_UNKNOWN) {
                break;
            }

            // In other case, just skip it
            continue;
        }

        // --- Leading Character ---

        if (NCLASS) {
            XString_Append(buffer, c);

            if (LETTER(c)) {
                type = TT_IDEN;
            } else if (c >= '0' && c <= '9') {
                type = TT_LINT;
            } else {
                TokenType tt = TokenType_Leading(c);

                // If the classification has failed, just fail.
                if (tt == TT_UNKNOWN) {
                    printf("Failed to classify leading character '%c'.\n", c);
                    FAIL;
                }

                // Extra cases for complex tokens
                if (c == '-' && next == '>') {
                    XString_Append(buffer, next);
                    tokenizer->ix ++;
                    tt = TT_POINT_RIGHT;
                } else if (c == '<' && next == '-') {
                    XString_Append(buffer, next);
                    tokenizer->ix ++;
                    tt = TT_POINT_LEFT;
                } else if (c == '=' && next == '=') {
                    XString_Append(buffer, next);
                    tokenizer->ix ++;
                    tt = TT_DOUBLE_EQUALS;
                }else if (c == '&' && next == '&') {
                    XString_Append(buffer, next);
                    tokenizer->ix ++;
                    tt = TT_AND_AND;
                } else if (c == '|' && next == '|') {
                    XString_Append(buffer, next);
                    tokenizer->ix ++;
                    tt = TT_OR_OR;
                }

                // Otherwise utilize the flow of the tokenizer to return the token
                type = tt;
                tokenizer->ix++;
                break;
            }

            goto CONTINUE;

        }

        // --- Trailing Character(s) ---

        // Possible type deviation
        if (c == '.') {
            if (type == TT_LINT) {
                XString_Append(buffer, c);
                type = TT_LFLOAT;
                goto CONTINUE;
            }
        }

        // Check if the nature of the character has changed (letter, digit)
        if ((DIGIT(c) && type == TT_LINT || type == TT_LFLOAT) || ((LETTER(c) || DIGIT(c)) && type == TT_IDEN)) {
            // Everything is alright, append to the buffer
            XString_Append(buffer, c);
            goto CONTINUE;
        }

        // The nature of the character has changed: Token break;
        break;

        CONTINUE:
        if (!LAST_IDX)
            continue;

        tokenizer->ix++;

        if (XString_Last(buffer) == '.') {
            printf("Float literal must not end with a dot (.)\n");
            FAIL;
        }

        break;
    }

    Token_Destroy(tokenizer->current);
    tokenizer->current = Token_Create(buffer->str, type);
    XString_Destroy(buffer);

    return STATUS_OK;
}

#undef LAST_IDX
#undef NCLASS
#undef FAIL

char *Tokenizer_Prime(char *str) {
    XString *xs = XString_Create();

    int spaces = 0;
    int lead = 1;

    for (unsigned i = 0; i < strlen(str); i++) {
        char c = str[i];

        if (SPACE(c)) {
            spaces++;
            continue;
        }

        // A character we do care about

        if (spaces > 0 && !lead)
            XString_Append(xs, ' ');

        XString_Append(xs, c);

        lead = 0;
        spaces = 0;
    }

    char *s = malloc(xs->length + 1);
    strcpy(s, xs->str);
    XString_Destroy(xs);
    return s;
}

#undef SPACE
#undef LETTER
#undef DIGIT