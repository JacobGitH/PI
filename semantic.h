

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include "symtable.h"

enum funcReturns{
    FUN_NULL_RETURN,
    FUN_INT,
    FUN_FLOAT,
    FUN_STRING
};

enum varTypes{
    VAR_NULL,
    VAR_INT,
    VAR_FLOAT,
    VAR_STRING
};

int semanticCheck(pNode ast);
void validate(pNode ast, htab_t *symtable, htab_t *localTable, char* name);
void load(pNode ast, htab_t *symtable);
void loadStaticFunctions(htab_t* symtable);
void validateFunctionCall(pNode ast,htab_t* symtable, char* name);
void validateFunParams(pNode arg, htab_t* symtable, htab_t* localTable, char* name);
void checkExpression(expr* root, htab_t* symtable, htab_pair_t* defaultVar, char* name);
#endif