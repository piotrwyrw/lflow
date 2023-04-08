//
// Created by pwpio on 08/04/2023.
//

#ifndef LFLOW_PARAM_H
#define LFLOW_PARAM_H

#include "token.h"

typedef struct {
    Token *id;
    Token *type;
} FunctionParameter;

FunctionParameter *FunctionParameter_Create(Token *, Token *);
void FunctionParameter_Destroy(FunctionParameter *);

#endif //LFLOW_PARAM_H
