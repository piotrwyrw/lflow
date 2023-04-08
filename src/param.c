#include "include/param.h"

#include <stdlib.h>

FunctionParameter *FunctionParameter_Create(Token *id, Token *type) {
    FunctionParameter *fp = malloc(sizeof(FunctionParameter));
    fp->id = Token_Dup(id);
    fp->type = Token_Dup(type);
    return fp;
}

void FunctionParameter_Destroy(FunctionParameter *parameter) {
    Token_Destroy(parameter->id);
    Token_Destroy(parameter->type);
    free(parameter);
}