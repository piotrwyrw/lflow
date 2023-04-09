//
// Created by pwpio on 08/04/2023.
//

#include "include/type.h"

#include <stdlib.h>

ComplexField *ComplexField_Create(Token *id, Type *type) {
    ComplexField *field = malloc(sizeof(ComplexField));
    field->type = type;
    field->id = Token_Dup(id);
    return field;
}

void ComplexField_Destroy(ComplexField *cx) {
    Token_Destroy(cx->id);
    free(cx);
}

ComplexType *ComplexType_Create(Token *id, Array *fields) {
    ComplexType *ct = malloc(sizeof(ComplexType));
    ct->id = Token_Dup(id);
    ct->fields = fields;
    return ct;
}

void ComplexType_Destroy(ComplexType *ct) {
    Token_Destroy(ct->id);
    free(ct);
}