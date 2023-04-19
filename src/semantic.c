#include "include/semantic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

SemanticAnalysis *SemanticAnalysis_Create(Node *program) {
    SemanticAnalysis *sa = malloc(sizeof(SemanticAnalysis));
    sa->program = program;
    sa->types = Array_Create();

    // Add the primitive types to the array
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_BYTE));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_WORD));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_DWORD));
    Array_Push(sa->types, Type_CreatePrimitive(PRIMITIVE_QWORD));

    return sa;
}

Type *SemanticAnalysis_ResolveType(SemanticAnalysis *sa, Type *type, Node *n) {
    if (type->type != TYPE_PLACEHOLDER)
        return NULL;

    if (n->type != NODE_BLOCK)
        return NULL;

    Type *t = SemanticAnalysis_FindType(sa, type->content.placeholder.id);
    if (t)
        return t;

    return NULL;
}

void SemanticAnalysis_Destroy(SemanticAnalysis *analysis) {
    Array_DestroyCallBack(analysis->types, (void *) Type_Destroy);
    free(analysis);
}

PrimitiveType PrimitiveType_FitInteger(int n) {
    int storage = ceil(log2(n)) + 1;
    if (storage <= 8)
        return PRIMITIVE_BYTE;
    else if (storage <= 16)
        return PRIMITIVE_WORD;
    else if (storage <= 32)
        return PRIMITIVE_DWORD;
    else if (storage <= 64)
        return PRIMITIVE_QWORD;
    else {
        SEMANTIC_PRINT("Integer exceeds QWORD bounds: %u\n", n);
        return -1;
    }
}

Status SemanticAnalysis_AnalyseNode(SemanticAnalysis *, Node *);

Type *SemanticAnalysis_AnalyseExpression(SemanticAnalysis *analysis, Node *expr) {
    if (expr->type == NODE_BINARY_EXPRESSION) {
        Type *left = SemanticAnalysis_AnalyseExpression(analysis, expr->node.binary.left);
        if (!left)
            return NULL;

        Type *right = SemanticAnalysis_AnalyseExpression(analysis, expr->node.binary.right);
        if (!right)
            return NULL;

        if (left->type == TYPE_VOID || right->type == TYPE_VOID) {
            SEMANTIC_PRINT("Cannot perform binary operation on void types.\n");
            return NULL;
        }

        if (!Type_Compare(left, right)) {
            Type *new = Type_Larger(left, right);
            SEMANTIC_PRINT(
                    "Cannot perform binary operation on conflicting types \"%s\" and \"%s\". Performing type cast to '%s'\n",
                    Type_Identifier(left),
                    Type_Identifier(right), Type_Identifier(new));
            return new;
        }

        return left;
    }

    if (expr->type == NODE_INTEGER_LITERAL) {
        PrimitiveType fitting = PrimitiveType_FitInteger(expr->node.int_lit.n);
        if (fitting == -1) {
            SEMANTIC_PRINT("Integer automatic classification failed. Maybe try explicit typing?\n");
            return NULL;
        }
        Token *tok = Token_Create((char *) PrimitiveType_String(fitting), TT_IDEN);
        Type *t = SemanticAnalysis_FindType(analysis, tok);
        SEMANTIC_PRINT("Classified integer %d (%s)\n", expr->node.int_lit.n, tok->value);
        Token_Destroy(tok);
        return t;
    }

    // Floats are stored as QWORDS
    if (expr->type == NODE_FLOAT_LITERAL) {
        Token *t = Token_Create("qword", TT_IDEN);
        Type *qw = SemanticAnalysis_FindType(analysis, t);
        Token_Destroy(t);
        if (!qw) {
            SEMANTIC_PRINT("Could not find the QWORD type.\n");
            return NULL;
        }
        SEMANTIC_PRINT("Classified float %f (qword)\n", expr->node.float_lit.f);
        return qw;
    }

    if (expr->type == NODE_VARIABLE_REFERENCE) {
        // Check whether the variable is accessible
        Element e = Block_FindElement(expr->super, expr->node.var_ref.id);

        if (e.type != ELEMENT_VARIABLE) {
            SEMANTIC_PRINT("The referenced variable '%s' is not a variable.\n", expr->node.var_ref.id->value);
            return NULL;
        }

        if (e.n)
            return e.n->node.var_decl.type;

        SEMANTIC_PRINT("The variable '%s' is undefined.\n", expr->node.var_ref.id->value);
        return NULL;
    }

    return NULL;
}

Status SemanticAnalysis_AnalyseVariableDeclaration(SemanticAnalysis *analysis, Node *n) {
    if (n->type != NODE_VARIABLE_DECLARATION) {
        SEMANTIC_PRINT("Internal error: Wrong node type passed to %s", __FUNCTION__);
        return STATUS_FAIL;
    }

    // Resolve the type
    if (n->node.var_decl.type->type == TYPE_PLACEHOLDER) {
        Type *resv = SemanticAnalysis_ResolveType(analysis, n->node.var_decl.type, n->super);
        if (!resv) {
            SEMANTIC_PRINT("Unresolved type '%s' in variable declaration '%s'.\n",
                           n->node.var_decl.type->content.placeholder.id->value, n->node.var_decl.id->value);
            return STATUS_FAIL;
        }
        n->node.var_decl.type = resv;
    }

    // Check for identifier conflicts
    Element e = Block_FindElement(n->super, n->node.var_decl.id);

    if (e.n) {
        SEMANTIC_PRINT("The identifier '%s' is already taken. Attempted redefinition as variable of type '%s'.\n",
                       n->node.var_decl.id->value, Type_Identifier(n->node.var_decl.type));
        return STATUS_FAIL;
    }

    if (n->node.var_decl.value) {
        Type *t = SemanticAnalysis_AnalyseExpression(analysis, n->node.var_decl.value);

        // Check for type conflicts
        if (!Type_Compare(t, n->node.var_decl.type)) {
            SEMANTIC_PRINT(
                    "The variable '%s' of type '%s' cannot be initialized with an expression of effective type '%s'.\n",
                    n->node.var_decl.id->value, Type_Identifier(n->node.var_decl.type), Type_Identifier(t));
            return STATUS_FAIL;
        }

        // Push the declaration onto the array
        Array_Push(n->super->node.block.declarations, n);

        return STATUS_OK;
    }

    return STATUS_OK;
}

Status SemanticAnalysis_AnalyseNode(SemanticAnalysis *analysis, Node *n) {
    if (!n) {
        SEMANTIC_PRINT("Encountered a null node.\n");
        return STATUS_FAIL;
    }

    if (n->type == NODE_BLOCK) {
        Status stat = STATUS_OK;
        for (unsigned i = 0; i < n->node.block.nodes->length; i++) {
            if (!SemanticAnalysis_AnalyseNode(analysis, Array_At(n->node.block.nodes, i))) {
                stat = STATUS_FAIL;
                break;
            }
        }
        return stat;
    }

    if (n->type == NODE_INTEGER_LITERAL || n->type == NODE_BINARY_EXPRESSION || n->type == NODE_FUNCTION_CALL ||
        n->type == NODE_VARIABLE_REFERENCE) {
        return SemanticAnalysis_AnalyseExpression(analysis, n) != NULL;
    }

    if (n->type == NODE_VARIABLE_DECLARATION) {
        return SemanticAnalysis_AnalyseVariableDeclaration(analysis, n);
    }

    if (n->type == NODE_FUNCTION_DEFINITION) {
        // TODO Handle function definition
    }

    return STATUS_OK;
}

Status SemanticAnalysis_RunAnalysis(SemanticAnalysis *analysis) {
    return SemanticAnalysis_AnalyseNode(analysis, analysis->program->node.program.nodes);
}

Type *SemanticAnalysis_FindType(SemanticAnalysis *analysis, Token *id) {
    for (unsigned i = 0; i < analysis->types->length; i++) {
        Type *t = Array_At(analysis->types, i);
        if (strcmp(Type_Identifier(t), id->value) == 0)
            return t;
    }
    return NULL;
}