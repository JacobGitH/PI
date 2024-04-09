
#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"

//grammar bundles
typedef enum {
    EXPR_VAR, // využívané parserem a vnitřně ve výrazech
    EXPR_INT, // využívané parserem a jako TYP
    EXPR_STR, // využívané parserem a jako TYP
    EXPR_FLOAT, // využívané parserem a jako TYP
    EXPR_VOID, // využívané parserem a jako TYP
    EXPR_ADDSUBCONCAT, // využívané parserem a vnitřně ve výrazech
    EXPR_MULDIV, // využívané parserem a vnitřně ve výrazech
    EXPR_EQ, // využívané parserem a vnitřně ve výrazech
    EXPR_LESSGRATER, // využívané parserem a vnitřně ve výrazech
    EXPR_EQTNEQT, // využívané parserem a vnitřně ve výrazech
    EXPR_OPBRACE, // využívané parserem 
    EXPR_CLBRACE, // využívané parserem
    EXPR_OPCBRACE, // využívané parserem
    EXPR_CLCBRACE, // využívané parserem
    EXPR_INDENT,  // využívané parserem
    EXPR_$,  // využívané parserem
    EXPR_COMMA, // využívané parserem
    BODY, // využívané parserem
    IF, // dostupné z AST
    IFEMPTY, // využívané parserem
    IFARG, // využívané parserem
    ELSE, // využívané parserem
    WHILE, // dostupné z AST
    WHILEEMPTY, // využívané parserem
    WHILEARG, // využívané parserem
    FUNCTIONCALL, // dostupné z AST
    FUNCTION, // dostupné z AST
    FUNCTIONEMPTY, // využívané parserem
    FUNCTIONARG, // využívané parserem
    FUNCTIONNAME, // využívané parserem
    NONVERIFIEDFUNCTION, // využívané parserem
    RETURN,  // dostupné z AST
    NONVERIFIEDEXPRESSION, // EXPRESSION WITHOUT ;
    EXPRESSION, // dostupné z AST
    SEMICOLON, // využívané parserem
    NONVERIFIEDCALL, // využívané parserem a vnitřně ve výrazech
    RETURNTYPE, // využívané parserem a v ->els pro FUNCTION, pokud má funkce definovaný návratový typ, jeho ->cond obsahuje TYP
    CONDITIONEXPRESSION, // využívané parserem a vnitřně v argumentech funkce
    ARGUMENTS, // využívané parserem a vnitřně v argumentech funkce
    KEYWORD // využívané parserem
} parsType;

typedef enum {
    NIL,
    ID,
    INT,
    FLOAT,
    STR,           

    ADD,            // +
    SUB,            // - 
    MUL,            // *
    DIV,            // /
    CONCAT,         // .
    
    EQ,             // =
    IDENTICAL,      // ===
    NONIDENTICAL,   // !==
    GT,             // >
    LT,             // <
    GTE,            // >=
    LTE,            // <=

    T_INT,
    T_FLOAT,
    T_STR,
    T_VOID,

    TN_INT,
    TN_FLOAT,
    TN_STR,
    TN_VOID,

    COMA,
    CALL,   	    // Function Call

    UNDEFINED,
} exprType;

typedef struct stExpression {
    parsType key;
    exprType tokenKey;
    char* value;
    struct stExpression* lPtr;
    struct stExpression* rPtr;
    struct stParseNode* funArg;
    bool isDefined;
} expr;

typedef struct stParseNode{    
    parsType key;
    char* name;
    struct stParseNode* arg;
    struct stExpression* cond;
    struct stParseNode* body;
    struct stParseNode* els;
    struct stParseNode* next;
} *pNode;

typedef struct stItem {
    parsType type;
    tokenList val;
    expr *expression;
    bool braceFlag;
    struct stParseNode* argument;
} *eItem;

typedef struct stStack {
    int top;
    int cap;
    eItem *item;
} *eStack;

typedef struct stParseStack {
    int top;
    int cap;
    pNode *item;
} *pStack;

typedef struct stkStack {
    int top;
    int cap;
    eStack *item;
} *eStackStack;

typedef struct tuple{
    int o;
    int c;
} *Tuple;

typedef struct flStack {
    int top;
    int cap;
    Tuple *flags;
} *flagStack;


pNode parse();

void eStackStackPush (eStackStack, eStack);
void eFlagStackPush (flagStack, Tuple);
void freeTree (pNode);
void freeExpr (expr*);
void checkConnect (pStack);
void mainExpand (pStack, pNode);
void mainLeftOperation (pStack, pNode);
void rightOperation (eStack);
void leftOperation (eStack, eItem, int);
void convertToExpression (eItem);
void pStackInsert (pStack, pNode, int);
void eStackInsert (eStack, eItem, int);
void eStackRemoveAt (eStack, int);
void eStackPush (eStack, eItem);
void pStackPush (pStack, pNode);

Tuple eFlagStackPop(flagStack);

eStack eStackStackPop (eStackStack);

eItem resolveBrace (eStack);
eItem resolveExpression (eStack);
eItem eStackGetItem (eStack, int);
eItem eStackGetTop (eStack);
eItem eStackPop (eStack);

pNode resolveMainStack (eStack);
pNode connectTwo (pNode, pNode);
pNode pIndent ();
pNode createNode (eItem);
pNode pStackGetItem (pStack, int);
pNode pStackGetTop (pStack);
pNode pStackPop (pStack);

int getOperation (pNode, eItem);
int expression (parsType, parsType);
int pStackGetTerminalIndex (pStack);
int eStackGetTerminalIndex (eStack);

exprType tokenToEx (tokenType);

flagStack flagStackInit ();

eStackStack eStackStackInit ();

pStack pStackInit ();

eStack eStackInit ();

parsType wrapTokenTypes(tokenType);
#endif //__PARSER_H__