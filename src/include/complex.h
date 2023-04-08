//
// Created by pwpio on 08/04/2023.
//

#ifndef LFLOW_COMPLEX_H
#define LFLOW_COMPLEX_H

#include "token.h"
#include "arr.h"
#include "type.h"

typedef struct {
    Token *id;
    Type *type;
} ComplexField;

typedef struct {
    Token *id;
    Array *fields;
} ComplexType;

#endif //LFLOW_COMPLEX_H
