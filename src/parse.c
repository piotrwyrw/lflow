#include "include/parse.h"
#include "include/conv.h"
#include "include/param.h"

#include <stdlib.h>
#include <string.h>

Parser *Parser_CreateParser(Tokenizer *tokenizer) {
    Parser *parser = malloc(sizeof(Parser));
    parser->tokenizer = tokenizer;
    parser->current = NULL;
    parser->next = NULL;
    parser->lastBlock = NULL;

    Parser_Consume(parser);
    Parser_Consume(parser);

    return parser;
}

void Parser_DestroyParser(Parser *parser) {
    if (parser->current)
        Token_Destroy(parser->current);

    if (parser->next)
        Token_Destroy(parser->next);

    free(parser);
}

void Parser_Consume(Parser *parser) {
    if (parser->current)
        Token_Destroy(parser->current);
    parser->current = parser->next;
    Tokenizer_Next(parser->tokenizer);
    parser->next = Token_Dup(parser->tokenizer->current);
}

bool Parser_Compare(Parser *p, TokenDesignation td, TokenType tt, const char *str) {
    Token *tok = (td == CURRENT) ? p->current : p->next;
    if (!tok)
        return false;
    if (str != NULL)
        return (strcmp(tok->value, str) == 0) && tok->type == tt;
    else
        return tok->type == tt;
}

Node *Parser_ParseProgram(Parser *parser) {
    Array *arr = Array_Create();
    Node *blk = Node_CreateBlock(arr, NULL);

    parser->lastBlock = blk;
    parser->rootBlock = blk;

    while (true) {
        if (Parser_Compare(parser, CURRENT, TT_UNKNOWN, NULL))
            break;

        Node *n = Parser_ParseNext(parser);

        if (n == NULL) {
            Array_DestroyCallBack(arr, (void *) Node_DestroyRecurse);
            Node_DestroyRecurse(blk);
            return NULL;
        }

        Array_Push(arr, n);
    }

    return Node_CreateProgram(blk);
}

Node *Parser_ParseNext(Parser *parser) {

    if (Parser_Compare(parser, CURRENT, TT_KW_VARYING, NULL) || Parser_Compare(parser, CURRENT, TT_KW_CONSTANT, NULL))
        return Parser_ParseVariableDeclaration(parser);

    if (Parser_Compare(parser, CURRENT, TT_IDEN, NULL) && Parser_Compare(parser, NEXT, TT_EQUALS, NULL))
        return Parser_ParseVariableAssignment(parser);

    if (Parser_Compare(parser, CURRENT, TT_KW_PROCEDURE, NULL))
        return Parser_ParseFunctionDefinition(parser);

    if (Parser_Compare(parser, CURRENT, TT_LBRACKET, NULL))
        return Parser_ParseBlock(parser);

    if (Parser_Compare(parser, CURRENT, TT_KW_RETURN, NULL))
        return Parser_ParseReturn(parser);

    if (Parser_Compare(parser, CURRENT, TT_KW_CHECK, NULL))
        return Parser_ParseCheck(parser);

    // Last resort
    Node *n = Parser_ParseExpression(parser);

    if (!n)
        return NULL;

    if (!Parser_Compare(parser, CURRENT, TT_SEMI, NULL)) {
        SYNTAX_ERR("Expected ';' after expression.\n");
        Node_DestroyRecurse(n);
        return NULL;
    }

    Parser_Consume(parser); // Skip ';'
    return n;
}

Node *Parser_ParseStringLiteral(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_LSTRING, NULL)) {
        SYNTAX_ERR("Expected string literal, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }
    Node *lit = Node_CreateStringLiteral(parser->current->value);
    Parser_Consume(parser); // Next token
    return lit;
}

Node *Parser_ParseIntegerLiteral(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_LINT, NULL)) {
        SYNTAX_ERR("Expected integer literal, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }
    Node *lit = Node_CreateIntegerLiteral(stoi(parser->current->value));
    Parser_Consume(parser); // Next token
    return lit;
}

Node *Parser_ParseRealLiteral(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_LFLOAT, NULL)) {
        SYNTAX_ERR("Expected float literal, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }
    Node *lit = Node_CreateFloatLiteral(stof(parser->current->value));
    Parser_Consume(parser); // Next token
    return lit;
}

Node *Parser_ParseVariableReference(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected identifier, got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }
    Node *ref = Node_CreateVariableReference(parser->current, parser->lastBlock);
    Parser_Consume(parser); // SKip to the next token
    return ref;
}

Node *Parser_ParseVariableAssignment(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected identifier. got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }

    Token *id = Token_Dup(parser->current);

    Parser_Consume(parser);

    if (!Parser_Compare(parser, CURRENT, TT_EQUALS, NULL)) {
        SYNTAX_ERR("Expected '=' after identifier \"%s\".\n", parser->current->value);
        return NULL;
    }

    Parser_Consume(parser);

    Node *expr = Parser_ParseExpression(parser);

    if (!expr) {
        Token_Destroy(id);
        return NULL;
    }

    if (!Parser_Compare(parser, CURRENT, TT_SEMI, NULL)) {
        SYNTAX_ERR("Expected ';' after target expression, got \"%s\".\n", parser->current->value);
        Token_Destroy(id);
        Node_DestroyRecurse(expr);
        return NULL;
    }

    Parser_Consume(parser); // Skip ';'

    Node *n = Node_CreateVariableAssignment(id, expr, parser->lastBlock);
    Token_Destroy(id);
    return n;
}

// identifier "(" expression ["," expression] ... ")"
Node *Parser_ParseFunctionCall(Parser *parser) {

    Token *identifier;

    // identifier
    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected identifier, got %s\n", TokenType_String(parser->current->type));
        return NULL;
    }

    identifier = Token_Dup(parser->current);

    Parser_Consume(parser);

    // (
    if (!Parser_Compare(parser, CURRENT, TT_LPAREN, NULL)) {
        SYNTAX_ERR("Expected '(' after identifier \"\"");
        Token_Destroy(identifier);
        return NULL;
    }

    Parser_Consume(parser);

    Array *exprs = Array_Create();

    // Parameters (expr. seq.)
    while (!Parser_Compare(parser, CURRENT, TT_RPAREN, NULL) && parser->current != NULL) {
        // Expression
        Node *expr = Parser_ParseExpression(parser);

        Array_Push(exprs, expr);

//         ',' or ')'
        if (Parser_Compare(parser, CURRENT, TT_COMMA, NULL) && !Parser_Compare(parser, NEXT, TT_RPAREN, NULL)) {
            Parser_Consume(parser);
            continue;
        }

        if (Parser_Compare(parser, CURRENT, TT_RPAREN, NULL))
            continue;

        SYNTAX_ERR("Expected ')' or ',' and more expressions.\n");
        Token_Destroy(identifier);
        Array_DestroyCallBack(exprs, (void *) Node_DestroyRecurse);
        return NULL;

    }

    if (!Parser_Compare(parser, CURRENT, TT_RPAREN, NULL)) {
        SYNTAX_ERR("Expected ')' after the list of expressions. Got \"%s\"\n", parser->current->value);

        Token_Destroy(identifier);
        Array_DestroyCallBack(exprs, (void *) Node_DestroyRecurse);

        return NULL;
    }

    Parser_Consume(parser); // SKip ')'

    Node *fcall = Node_CreateFunctionCall(identifier, exprs, parser->lastBlock);
    Token_Destroy(identifier);

    return fcall;
}

// "varying" | "const" identifier ":" type "=" expression ";"
Node *Parser_ParseVariableDeclaration(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_KW_VARYING, NULL) &&
        !Parser_Compare(parser, CURRENT, TT_KW_CONSTANT, NULL)) {
        SYNTAX_ERR("Expected modification qualifier ('varying' or 'const'). Got \"%s\" instead.\n",
                   parser->current->value);
        return NULL;
    }

    ModificationQualifier modQua = ModificationQualifier_FromTokenType(parser->current->type);

    Parser_Consume(parser); // Skip the qualifier

    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected variable name (identifier). Got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }

    Token *id = Token_Dup(parser->current);

    Parser_Consume(parser); // Skip the identifier

    if (!Parser_Compare(parser, CURRENT, TT_COLON, NULL)) {
        SYNTAX_ERR("Expected colon ':' after identifier, got \"%s\".\n", parser->current->value);
        return NULL;
    }

    Parser_Consume(parser); // Skip the colon

    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected type (identifier) after ':', got %s.\n", TokenType_String(parser->current->type));
        Token_Destroy(id);
        return NULL;
    }

    Token *type = Token_Dup(parser->current);

    Parser_Consume(parser); // Skip the type identifier

    // No initial value
    if (Parser_Compare(parser, CURRENT, TT_SEMI, NULL)) {

        if (modQua == MQ_CONST) {
            SYNTAX_ERR(
                    "Variables with the 'constant' qualifier may not be left unassigned upon declaration. Missing initial value for const \"%s\".\n",
                    id->value);
            Token_Destroy(id);
            Token_Destroy(type);
            return NULL;
        }

        Parser_Consume(parser); // Skip ';'

        Node *n = Node_CreateVariableDeclaration(id, NULL, type, modQua, parser->lastBlock);

        Token_Destroy(id);
        Token_Destroy(type);

        return n;
    }

    // Initial value
    if (!Parser_Compare(parser, CURRENT, TT_EQUALS, NULL)) {
        SYNTAX_ERR("Expected ';' or '=' and an initial value for the variable \"%s\", got %s.\n", id->value,
                   TokenType_String(parser->current->type));
        Token_Destroy(id);
        Token_Destroy(type);
        return NULL;
    }

    Parser_Consume(parser); // SKip '='

    Node *expr = Parser_ParseExpression(parser);

    if (expr == NULL) {
        Token_Destroy(id);
        Token_Destroy(type);
        return NULL;
    }

    if (!Parser_Compare(parser, CURRENT, TT_SEMI, NULL)) {
        SYNTAX_ERR("Expected ';' after the expression, got %s.\n", TokenType_String(parser->current->type));
        Token_Destroy(id);
        Token_Destroy(type);
        Node_DestroyRecurse(expr);
        return NULL;
    }

    Parser_Consume(parser); // Skip ';'

    Node *n = Node_CreateVariableDeclaration(id, expr, type, modQua, parser->lastBlock);

    Token_Destroy(id);
    Token_Destroy(type);

    return n;
}

Node *Parser_ParseSubExpression(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_LPAREN, NULL)) {
        SYNTAX_ERR("Expected '(' at the start of a subexpression, got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }
    Parser_Consume(parser); // SKip the '('
    Node *expr = Parser_ParseExpression(parser);
    if (!Parser_Compare(parser, CURRENT, TT_RPAREN, NULL)) {
        SYNTAX_ERR("Expected ')' after sub-expression.\n");
        Node_DestroyRecurse(expr);
        return NULL;
    }
    Parser_Consume(parser);
    return expr;
}

Node *Parser_ParseExpression(Parser *parser) {
    Node *left = Parser_ParseSecondDegree(parser);

    if (!left)
        return NULL;

    // Addition / Subtraction / And / Or
    while (Parser_Compare(parser, CURRENT, TT_PLUS, NULL) || Parser_Compare(parser, CURRENT, TT_MINUS, NULL) ||
           Parser_Compare(parser, CURRENT, TT_AND_AND, NULL) || Parser_Compare(parser, CURRENT, TT_OR_OR, NULL)) {
        BinaryType type = BinaryType_FromTokenType(parser->current->type);

        if (type == BIN_UNDEF) {
            SYNTAX_ERR("Unknown binary operation \"%s\".\n", parser->current->value);
            Node_DestroyRecurse(left);
            return NULL;
        }

        Parser_Consume(parser); // next token, skip operation

        Node *right = Parser_ParseExpression(parser);

        if (!right)
            return NULL;

        left = Node_CreateBinaryOperation(left, right, type, parser->lastBlock);
    }

    return left;
}

Node *Parser_ParseSecondDegree(Parser *parser) {
    Node *left = Parser_ParseAtom(parser);

    if (!left)
        return NULL;

    // Multiplication / Division / > / < / == / !=
    while (Parser_Compare(parser, CURRENT, TT_ASTERISK, NULL) || Parser_Compare(parser, CURRENT, TT_SLASH, NULL) ||
           Parser_Compare(parser, CURRENT, TT_DOUBLE_EQUALS, NULL) ||
           Parser_Compare(parser, CURRENT, TT_LGREATER, NULL) ||
           Parser_Compare(parser, CURRENT, TT_RGREATER, NULL) || Parser_Compare(parser, CURRENT, TT_NOT_EQUALS, NULL)) {
        BinaryType type = BinaryType_FromTokenType(parser->current->type);

        if (type == BIN_UNDEF) {
            SYNTAX_ERR("Unknown binary operation \"%s\".\n", parser->current->value);
            Node_DestroyRecurse(left);
            return NULL;
        }

        Parser_Consume(parser); // next token, skip operation

        Node *right = Parser_ParseSecondDegree(parser);

        if (!right)
            return NULL;

        left = Node_CreateBinaryOperation(left, right, type, parser->lastBlock);
    }

    return left;
}

Node *Parser_ParseAtom(Parser *parser) {

    // Size directive
    if (Parser_Compare(parser, CURRENT, TT_KW_SIZE, NULL)) {
        return Parser_ParseSize(parser);
    }

    // Sub-expression
    if (Parser_Compare(parser, CURRENT, TT_LPAREN, NULL)) {
        return Parser_ParseSubExpression(parser);
    }

    // Function call
    if (Parser_Compare(parser, CURRENT, TT_IDEN, NULL) && Parser_Compare(parser, NEXT, TT_LPAREN, NULL)) {
        if (Parser_Compare(parser, CURRENT, TT_IDEN, "if")) {
            WARN("if is not a conditional statement, did you mean to use 'check' ?\n");
        }
        return Parser_ParseFunctionCall(parser);
    }

    // Integer Literal
    if (Parser_Compare(parser, CURRENT, TT_LINT, NULL)) {
        return Parser_ParseIntegerLiteral(parser);
    }

    // Real Literal
    if (Parser_Compare(parser, CURRENT, TT_LFLOAT, NULL)) {
        return Parser_ParseRealLiteral(parser);
    }

    // String Literal
    if (Parser_Compare(parser, CURRENT, TT_LSTRING, NULL)) {
        return Parser_ParseStringLiteral(parser);
    }

    // Variable reference
    if (Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        return Parser_ParseVariableReference(parser);
    }

    SYNTAX_ERR("Unknown atom starting with \"%s\".\n", parser->current->value);

    return NULL;
}

Node *Parser_ParseBlock(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_LBRACKET, NULL)) {
        SYNTAX_ERR("Block statements should start with a '{', got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }

    Parser_Consume(parser); // Skip '{'

    Array *blk = Array_Create();

    while (!Parser_Compare(parser, CURRENT, TT_RBRACKET, NULL) && !Parser_Compare(parser, CURRENT, TT_UNKNOWN, NULL)) {
        Node *n = Parser_ParseNext(parser);

        if (!n) {
            Array_DestroyCallBack(blk, (void *) Node_DestroyRecurse);
            return NULL;
        }

        Array_Push(blk, n);
    }

    if (!Parser_Compare(parser, CURRENT, TT_RBRACKET, NULL)) {
        SYNTAX_ERR("Reached end of file while parsing block statement. Missing closing bracket '}'.\n");
        Array_DestroyCallBack(blk, (void *) Node_DestroyRecurse);
        return NULL;
    }

    Parser_Consume(parser); // Skip closing bracket

    Node *block = Node_CreateBlock(blk, parser->lastBlock);

    parser->lastBlock = block;

    return block;
}

// "procedure" identifier "(" (identifier ":" type [","]) ")" ":" type block
Node *Parser_ParseFunctionDefinition(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_KW_PROCEDURE, NULL)) {
        SYNTAX_ERR("Expected 'procedure' keyword, got \"%s\".\n", parser->current->value);
        return NULL;
    }

    Parser_Consume(parser);

    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected procedure identifier after 'procedure' keyword, got %s.\n",
                   TokenType_String(parser->current->type));
        return NULL;
    }

    Token *id = Token_Dup(parser->current);

    Parser_Consume(parser); // Skip identifier

    if (!Parser_Compare(parser, CURRENT, TT_LPAREN, NULL)) {
        SYNTAX_ERR("Expected '(' after procedure identifier \"%s\", got %s.\n", id->value,
                   TokenType_String(parser->current->type));
        Token_Destroy(id);
        return NULL;
    }

    Parser_Consume(parser); // SKip '('

    Array *params = Array_Create();

    while (!Parser_Compare(parser, CURRENT, TT_RPAREN, NULL) && !Parser_Compare(parser, CURRENT, TT_UNKNOWN, NULL)) {
        if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
            SYNTAX_ERR("Expected parameter identifier, got %s.\n", TokenType_String(parser->current->type));
            Token_Destroy(id);
            Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
            return NULL;
        }

        Token *param_id = Token_Dup(parser->current);

        Parser_Consume(parser); // Skip the identifier

        if (!Parser_Compare(parser, CURRENT, TT_COLON, NULL)) {
            SYNTAX_ERR("Expected ':' after parameter identifier, got %s.\n", TokenType_String(parser->current->type));
            Token_Destroy(id);
            Token_Destroy(param_id);
            Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
            return NULL;
        }

        Parser_Consume(parser); // Skip ':'

        if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
            SYNTAX_ERR("Expected type identifier after ':' for parameter \"%s\", got %s.\n", param_id->value,
                       TokenType_String(parser->current->type));
            Token_Destroy(id);
            Token_Destroy(param_id);
            Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
            return NULL;
        }

        Token *param_type = Token_Dup(parser->current);

        Parser_Consume(parser); // Skip type identifier

        if (!Parser_Compare(parser, CURRENT, TT_COMMA, NULL) && !Parser_Compare(parser, CURRENT, TT_RPAREN, NULL)) {
            SYNTAX_ERR("Expected ')' or ',' and more parameters, got %s.\n", TokenType_String(parser->current->type));
            Token_Destroy(id);
            Token_Destroy(param_id);
            Token_Destroy(param_type);
            Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
            return NULL;
        }

        FunctionParameter *param = FunctionParameter_Create(param_id, param_type);
        Token_Destroy(param_id);
        Token_Destroy(param_type);

        Array_Push(params, param);

        if (Parser_Compare(parser, CURRENT, TT_COMMA, NULL)) {
            Parser_Consume(parser);
        }
    }

    if (!Parser_Compare(parser, CURRENT, TT_RPAREN, NULL)) {
        SYNTAX_ERR(
                "Missing closing parentheses ')' after parameter list. Reached end of file while parsing function signature for \"%s\".\n",
                id->value);
        Token_Destroy(id);
        Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
        return NULL;
    }

    Parser_Consume(parser); // Skip ')'

    if (!Parser_Compare(parser, CURRENT, TT_COLON, NULL)) {
        SYNTAX_ERR("Expected ':' after ')', got %s in definition of function \"%s\".\n",
                   TokenType_String(parser->current->type), id->value);
        Token_Destroy(id);
        Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
        return NULL;
    }

    Parser_Consume(parser); // SKip ':'

    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected type identifier after ':' in definition of function \"%s\", got %s.\n", id->value,
                   TokenType_String(parser->current->type));
        Token_Destroy(id);
        Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
        return NULL;
    }

    Token *type = Token_Dup(parser->current);

    Parser_Consume(parser); // SKip type identifier

    if (!Parser_Compare(parser, CURRENT, TT_LBRACKET, NULL)) {
        SYNTAX_ERR("Expected '{' after type identifier for function \"%s\", got %s.\n", id->value,
                   TokenType_String(parser->current->type));
        Token_Destroy(id);
        Token_Destroy(type);
        Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
        return NULL;
    }

    Node *blk = Parser_ParseBlock(parser); // Parse the block statement

    if (!blk) {
        Token_Destroy(id);
        Token_Destroy(type);
        Array_DestroyCallBack(params, (void *) FunctionParameter_Destroy);
        return NULL;
    }

    Node *fdef = Node_CreateFunctionDefinition(id, type, params, blk, parser->lastBlock);
    Token_Destroy(id);
    Token_Destroy(type);

    return fdef;
}

Node *Parser_ParseReturn(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_KW_RETURN, NULL)) {
        SYNTAX_ERR("Expected 'return', got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }

    Parser_Consume(parser); // SKip 'return'

    if (Parser_Compare(parser, CURRENT, TT_SEMI, NULL)) {
        Parser_Consume(parser); // SKip ';'
        return Node_CreateReturn(NULL, parser->lastBlock);
    }

    Node *expr = Parser_ParseExpression(parser);

    if (!expr)
        return NULL;

    if (!Parser_Compare(parser, CURRENT, TT_SEMI, NULL)) {
        SYNTAX_ERR("Expected ';' after return expression, got %s.\n", TokenType_String(parser->current->type));
        Node_DestroyRecurse(expr);
        return NULL;
    }

    Parser_Consume(parser); // Skip ';'

    return Node_CreateReturn(expr, parser->lastBlock);
}

Node *Parser_ParseCheck(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_KW_CHECK, NULL)) {
        SYNTAX_ERR("Expected 'check' keyword at the start of a check statement, got \"%s\".\n", parser->current->value);
        return NULL;
    }

    Parser_Consume(parser); // Skip 'check'

    Node *chk = Parser_ParseSubExpression(parser);

    if (!chk)
        return NULL;

    if (!Parser_Compare(parser, CURRENT, TT_LBRACKET, NULL)) {
        SYNTAX_ERR("Expected '{' after check expression, got %s.\n", TokenType_String(parser->current->type));
        Node_DestroyRecurse(chk);
        return NULL;
    }

    Node *blk = Parser_ParseBlock(parser);

    if (!blk) {
        Node_DestroyRecurse(chk);
        return NULL;
    }

    Node *check = Node_CreateCheck(chk, blk, NULL, parser->lastBlock);
    Node *root = check;

    bool flag = true; // false - The final 'otherwise' had already been commited

    while (Parser_Compare(parser, CURRENT, TT_KW_OTHERWISE, NULL)) {

        if (!flag) {
            SYNTAX_ERR("No more checks allowed after a last-resort 'otherwise'.\n");
            Node_DestroyRecurse(root);
            return NULL;
        }

        Parser_Consume(parser); // Skip 'otherwise'

        Node *expr = NULL;

        // otherwise check
        if (Parser_Compare(parser, CURRENT, TT_KW_CHECK, NULL)) {
            Parser_Consume(parser); // SKip 'check'

            if (!Parser_Compare(parser, CURRENT, TT_LPAREN, NULL)) {
                SYNTAX_ERR("Expected '(' after 'otherwise check', got %s.\n", TokenType_String(parser->current->type));
                Node_DestroyRecurse(chk);
                Node_DestroyRecurse(blk);
                return NULL;
            }

            expr = Parser_ParseSubExpression(parser);

            if (!expr) {
                Node_DestroyRecurse(chk);
                Node_DestroyRecurse(blk);
                return NULL;
            }
        }

        if (!expr)
            flag = false;

        if (!Parser_Compare(parser, CURRENT, TT_LBRACKET, NULL)) {
            if (!expr) {
                SYNTAX_ERR("Expected '{' after 'otherwise', got %s.\n", TokenType_String(parser->current->type));
            } else {
                SYNTAX_ERR("Expected '{' after otherwise-check expression, got %s.\n",
                           TokenType_String(parser->current->type));
            }
            Node_DestroyRecurse(blk);
            Node_DestroyRecurse(chk);
            if (expr)
                Node_DestroyRecurse(expr);
            return NULL;
        }

        Node *otherwise_blk = Parser_ParseBlock(parser);

        if (!otherwise_blk) {
            Node_DestroyRecurse(blk);
            Node_DestroyRecurse(chk);
            if (expr)
                Node_DestroyRecurse(expr);
            return NULL;
        }

        // Create the sub-check
        Node *subchk = Node_CreateCheck(expr, otherwise_blk, NULL, parser->lastBlock);
        check->node.check.sub = subchk;
        check = subchk;
    }

    return root;
}

Node *Parser_ParseSize(Parser *parser) {
    if (!Parser_Compare(parser, CURRENT, TT_KW_SIZE, NULL)) {
        SYNTAX_ERR("Expected 'size' keyword at the beginning of a size directive, got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }

    Parser_Consume(parser);

    if (!Parser_Compare(parser, CURRENT, TT_LSBRACKET, NULL)) {
        SYNTAX_ERR("Expected '[' after 'size' keyword, got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }

    Parser_Consume(parser);

    if (!Parser_Compare(parser, CURRENT, TT_IDEN, NULL)) {
        SYNTAX_ERR("Expected size identifier after '[', got %s.\n", TokenType_String(parser->current->type));
        return NULL;
    }

    Type *t = Type_CreatePlaceholder(parser->current);

    Parser_Consume(parser);

    if (!Parser_Compare(parser, CURRENT, TT_RSBRACKET, NULL)) {
        SYNTAX_ERR("Expected ']' after type identifier '%s', got %s.\n", Type_Identifier(t), TokenType_String(parser->current->type));
        return NULL;
    }

    Parser_Consume(parser);

    return Node_CreateSize(t, parser->lastBlock);
}