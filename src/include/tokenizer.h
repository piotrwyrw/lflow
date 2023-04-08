#ifndef LFLOW_TOKENIZER_H
#define LFLOW_TOKENIZER_H

#include "token.h"
#include "status.h"

typedef struct {
    char *input;
    unsigned int length;

    unsigned int ix;

    Token *current;
} Tokenizer;

Tokenizer *Tokenizer_Create(char *);
void Tokenizer_Destroy(Tokenizer *);

int Tokenizer_HasNext(Tokenizer *);
Status Tokenizer_Next(Tokenizer *);

char *Tokenizer_Prime(char *);

#endif