//
// Created by pwpio on 08/04/2023.
//

#ifndef LFLOW_TYPE_H
#define LFLOW_TYPE_H

#include "token.h"

typedef enum {
    TYPE_BUILTIN,
    TYPE_DEFINED,
    TYPE_PLACEHOLDER,
} TypeType;

typedef enum {
    BT_REAL,
    BT_INTEGER,
    BT_VOID
} BuiltinType;

const char *BuiltinType_ToString(BuiltinType);

typedef struct {
    TypeType kind;
    union {

        struct {
            BuiltinType type;
        } builtin;

        struct {
            Token *lookupId;
        } defined;

        struct {
            Token *id;
        } placeholder;

    } type;
} Type;

Type *Type_CreateBuiltin(BuiltinType);
Type *Type_CreateDefined(Token *);
Type *Type_CreatePlaceholder(Token *);
char *Type_Identifier(Type *);
void Type_DestroyType(Type *);

#endif //LFLOW_TYPE_H
