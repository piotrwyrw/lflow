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
    NODE_VARIABLE_ASSIGNMENT,
    NODE_BINARY_EXPRESSION,
    NODE_FUNCTION_CALL
} NodeType;

const char *NodeType_ToString(NodeType);

typedef enum {
    BIN_ADD,
    BIN_SUB,
    BIN_MUL,
    BIN_DIV,
    BIN_UNDEF
} BinaryType;

const char *BinaryType_ToString(BinaryType);
BinaryType BinaryType_FromTokenType(TokenType);

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

        // Declaration
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

        // Binary operation
        struct {
            Node *left;
            Node *right;
            BinaryType op;
        } binary;

        // Function call
        struct {
            Token *id;
            Array *exprs;
        } fcall;

    } node;
};

Node *Node_CreateBase(NodeType);

void Node_DestroyBase(Node *);

Node *Node_CreateProgram(Array *);

Node *Node_CreateStringLiteral(char *);

Node *Node_CreateIntegerLiteral(int);

Node *Node_CreateFloatLiteral(float);

Node *Node_CreateVariableDeclaration(Token *, Node *);

Node *Node_CreateVariableAssignment(Token *, Node *);

Node *Node_CreateBinaryOperation(Node *, Node *, BinaryType);

Node *Node_CreateFunctionCall(Token *, Array *);

void Node_DestroyRecurse(Node *);

void Node_Print(unsigned, Node *);

#endif