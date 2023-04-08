//
// Created by pwpio on 08/04/2023.
//

#include "include/type.h"

#include <stdlib.h>

#define CASE(a) case a: return #a;

const char *BuiltinType_ToString(BuiltinType type) {
    switch (type) {
        CASE(BT_INTEGER)
        CASE(BT_REAL)
        CASE(BT_VOID)
    }
}

#undef CASE

Type *Type_CreateBuiltin(BuiltinType bt) {
    Type *t = malloc(sizeof(Type));
    t->kind = TYPE_BUILTIN;
    t->type.builtin.type = bt;
    return t;
}

Type *Type_CreateDefined(Token *token) {
    Type *t = malloc(sizeof(Type));
    t->kind = TYPE_DEFINED;
    t->type.defined.lookupId = Token_Dup(token);
    return t;
}

Type *Type_CreatePlaceholder(Token *type) {
    Type *t = malloc(sizeof(Type));
    t->kind = TYPE_PLACEHOLDER;
    t->type.placeholder.id = Token_Dup(type);
    return t;
}

char *Type_Identifier(Type *type) {
    switch (type->kind) {
        case TYPE_PLACEHOLDER:
            return type->type.placeholder.id->value;
        case TYPE_DEFINED:
            return type->type.defined.lookupId->value;
        case TYPE_BUILTIN:
            return BuiltinType_ToString(type->type.builtin.type);
    }
}

void Type_DestroyType(Type *type) {
    if (type->kind == TYPE_DEFINED)
        Token_Destroy(type->type.defined.lookupId);
    if (type->kind == TYPE_PLACEHOLDER)
        Token_Destroy(type->type.placeholder.id);
    free(type);
}