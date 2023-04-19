//
// Created by pwpio on 08/04/2023.
//

#ifndef LFLOW_SEMANTIC_H
#define LFLOW_SEMANTIC_H

#include "complex.h"
#include "ast.h"
#include "status.h"

#define SEMANTIC_PRINT(...) \
        printf("Notamide -> "); \
        printf(__VA_ARGS__);

typedef struct {
    Array *types;
    Node *program;
    Node *currentBlock;
} SemanticAnalysis;

Type *SemanticAnalysis_ResolveType(SemanticAnalysis *, Type *, Node *);

SemanticAnalysis *SemanticAnalysis_Create(Node *);
void SemanticAnalysis_Destroy(SemanticAnalysis *);

PrimitiveType PrimitiveType_FitInteger(int);

Status SemanticAnalysis_RunAnalysis(SemanticAnalysis *);

Type *SemanticAnalysis_FindType(SemanticAnalysis *, Token *);

#endif //LFLOW_SEMANTIC_H
