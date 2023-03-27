#ifndef LFLOW_AST_H
#define LFLOW_AST_H

#include "arr.h"
#include "bool.h"
#include "token.h"

typedef enum {
    NODE_PROGRAM,
    NODE_STRING_LITERAL,
    NODE_INTEGER_LITERAL,
    NODE_FLOAT_LITERAL,
    NODE_VARIABLE_DECLARATION,
    NODE_VARIABLE_ASSIGNMENT
} NodeType;

const char *NodeType_ToString(NodeType);

typedef struct Node Node;

struct Node {
    NodeType type;
    union {
        struct {
            Array *nodes;
        } program;

        // Literals
        struct {
            char *str;
        } str_lit;

        struct {
            int n;
        } int_lit;

        struct {
            float f;
        } float_lit;

        // Declarations
        struct {
            Token *id;
            bool defined;
            Node *value;
        } var_decl;

        // Assignment
        struct {
            Token *id;
            Node *value;
        } var_assign;

    } node;
};

Node *Node_CreateBase(NodeType);

void Node_DestroyBase(Node *);

Node *Node_CreateProgram(Array *);

Node *Node_CreateStringLiteral(char *);

Node *Node_CreateIntegerLiteral(int);

Node *Node_CreateFloatLiteral(float);

Node *Node_createVariableDeclaration(Token *, Node *);

void Node_Destroy(Node *);

void Node_Print(unsigned, Node *);

#endif