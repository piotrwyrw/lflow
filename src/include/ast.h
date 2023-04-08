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
    NODE_FUNCTION_CALL,
    NODE_VARIABLE_REFERENCE,
    NODE_BLOCK,
    NODE_FUNCTION_DEFINITION,
    NODE_RETURN,
    NODE_CHECK
} NodeType;

const char *NodeType_ToString(NodeType);

typedef enum {

    // Arithmetics
    BIN_ADD,
    BIN_SUB,
    BIN_MUL,
    BIN_DIV,

    // Logical boolean oeprations
    BIN_OR,
    BIN_AND,
    BIN_EQUAL,
    BIN_LGREATER,
    BIN_RGREATER,

    BIN_UNDEF
} BinaryType;

const char *BinaryType_ToString(BinaryType);
BinaryType BinaryType_FromTokenType(TokenType);

typedef enum {
    MQ_CONST,
    MQ_VARYING,
    MQ_UNDEF
} ModificationQualifier;

ModificationQualifier ModificationQualifier_FromTokenType(TokenType);
const char *ModificationQualifier_String(ModificationQualifier);

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

        // Var  Declaration
        struct {
            Token *id;
            Token *type;
            bool defined;
            Node *value;
            ModificationQualifier mutable;
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

        // Variable reference
        struct {
            Token *id;
        } var_ref;

        // Compound / Block statement
        struct {
            Array *nodes;
        } block;

        // Function definition
        struct {
            Token *id;
            Token *type;
            Array *params;
            Node *block;
        } func_def;

        // Return statement
        struct {
            Node *expr;
        } ret;

        // Check statement
        struct {
            Node *expr;
            Node *block;
            Node *sub;
        } check;

    } node;
};

Node *Node_CreateBase(NodeType);

void Node_DestroyBase(Node *);

Node *Node_CreateProgram(Array *);

Node *Node_CreateStringLiteral(char *);

Node *Node_CreateIntegerLiteral(int);

Node *Node_CreateFloatLiteral(float);

Node *Node_CreateVariableDeclaration(Token *, Node *, Token *, ModificationQualifier);

Node *Node_CreateVariableAssignment(Token *, Node *);

Node *Node_CreateBinaryOperation(Node *, Node *, BinaryType);

Node *Node_CreateFunctionCall(Token *, Array *);

Node *Node_CreateVariableReference(Token *);

Node *Node_CreateBlock(Array *);

Node *Node_CreateFunctionDefinition(Token *, Token *, Array *, Node *);

Node *Node_CreateReturn(Node *);

Node *Node_CreateCheck(Node *, Node *, Node *);

void Node_DestroyRecurse(Node *);

void Node_Print(unsigned, Node *);

#endif