

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "semantic.h"



// Zpracovává výraz a ošetřuje chyby
void checkExpression(expr* root, htab_t* symtable, htab_pair_t* defaultVar, char* name){
    if(root == NULL){
        return;
    }
    // Zajišťuje aby byla proměnna předem deklarovaná 
    if(root->tokenKey == 1){
        htab_pair_t *isDeclared = htab_find(symtable, root->value);

        
        if(isDeclared == NULL){
            exit(5);
        }

    root->tokenKey = isDeclared->value;
    }

    
    // Zpracovává typovou kompatibilitu
    if(defaultVar != NULL){
    switch (root->tokenKey)
        {
        case INT:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = T_INT;
                break;
            case TN_VOID:
                defaultVar->value = T_INT;
                break;
            case T_VOID:
                defaultVar->value = T_INT;
                break;
            case T_INT:
                defaultVar->value = T_INT;
                break;
            case T_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case T_STR:
                exit(7);
                break;
            case TN_INT:
                defaultVar->value = T_INT;
                break;
            case TN_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case TN_STR:
                exit(7);
                break;
            default:
                break;
            }
        break; 
        case FLOAT:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = T_FLOAT;
                break;
            case TN_VOID:
                defaultVar->value = T_FLOAT;
                break;
            case T_VOID:
                defaultVar->value = T_FLOAT;
                break;
            case T_INT:
                defaultVar->value = T_FLOAT;
                break;
            case T_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case T_STR:
                exit(7);
                break;
            case TN_INT:
                defaultVar->value = T_FLOAT;
                break;
            case TN_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case TN_STR:
                exit(7);
                break;
            default:
                break;
            }
        break; 
        case STR:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = T_STR;
                break;
            case TN_VOID:
                defaultVar->value = T_STR;
                break;
            case T_VOID:
                defaultVar->value = T_STR;
                break;
            case T_INT:
                exit(7);
                break;
            case T_FLOAT:
                exit(7);
                break;
            case T_STR:
                defaultVar->value = T_STR;
                break;
            case TN_INT:
                exit(7);
                break;
            case TN_FLOAT:
                exit(7);
                break;
            case TN_STR:
                defaultVar->value = T_STR;
                break;
            default:
                break;
            }
        break; 
        case T_INT:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = T_INT;
                break;
            case TN_VOID:
                defaultVar->value = T_INT;
                break;
            case T_VOID:
                defaultVar->value = T_INT;
                break;
            case T_INT:
                defaultVar->value = T_INT;
                break;
            case T_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case T_STR:
                exit(7);
                break;
            case TN_INT:
                defaultVar->value = T_INT;
                break;
            case TN_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case TN_STR:
                exit(7);
                break;
            default:
                break;
            }
        break;
        case T_FLOAT:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = T_FLOAT;
                break;
            case TN_VOID:
                defaultVar->value = T_FLOAT;
                break;
            case T_VOID:
                defaultVar->value = T_FLOAT;
                break;
            case T_INT:
                defaultVar->value = T_FLOAT;
                break;
            case T_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case T_STR:
                exit(7);
                break;
            case TN_INT:
                defaultVar->value = T_FLOAT;
                break;
            case TN_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case TN_STR:
                exit(7);
                break;
            default:
                break;
            }
        break;
        case T_STR:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = T_STR;
                break;
            case TN_VOID:
                defaultVar->value = T_STR;
                break;
            case T_VOID:
                defaultVar->value = T_STR;
                break;
            case T_INT:
                exit(7);
                break;
            case T_FLOAT:
                exit(7);
                break;
            case T_STR:
                defaultVar->value = T_STR;
                break;
            case TN_INT:
                exit(7);
                break;
            case TN_FLOAT:
                exit(7);
                break;
            case TN_STR:
                defaultVar->value = T_STR;
                break;
            default:
                break;
            }
        break; 
        case TN_INT:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = TN_INT;
                break;
            case TN_VOID:
                defaultVar->value = TN_INT;
                break;
            case T_VOID:
                defaultVar->value = TN_INT;
                break;
            case T_INT:
                defaultVar->value = T_INT;
                break;
            case T_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case T_STR:
                exit(7);
                break;
            case TN_INT:
                defaultVar->value = T_INT;
                break;
            case TN_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case TN_STR:
                exit(7);
                break;
            default:
                break;
            }
        break;
        case TN_FLOAT:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = TN_FLOAT;
                break;
            case TN_VOID:
                defaultVar->value = TN_FLOAT;
                break;
            case T_VOID:
                defaultVar->value = TN_FLOAT;
                break;
            case T_INT:
                defaultVar->value = T_FLOAT;
                break;
            case T_FLOAT:
                defaultVar->value = T_FLOAT;
                break;
            case T_STR:
                exit(7);
                break;
            case TN_INT:
                defaultVar->value = TN_FLOAT;
                break;
            case TN_FLOAT:
                defaultVar->value = TN_FLOAT;
                break;
            case TN_STR:
                exit(7);
                break;
            default:
                break;
            }
        break;
        case TN_STR:
            switch (defaultVar->value)
            {
            case NIL:
                defaultVar->value = TN_STR;
                break;
            case TN_VOID:
                defaultVar->value = TN_STR;
                break;
            case T_VOID:
                defaultVar->value = TN_STR;
                break;
            case T_INT:
                exit(7);
                break;
            case T_FLOAT:
                exit(7);
                break;
            case T_STR:
                defaultVar->value = T_STR;
                break;
            case TN_INT:
                exit(7);
                break;
            case TN_FLOAT:
                exit(7);
                break;
            case TN_STR:
                defaultVar->value = TN_STR;
                break;
            default:
                break;
            }
        break;
        default:
            break;
        }
    
    }
        


    checkExpression(root->rPtr, symtable, defaultVar, name);
    checkExpression(root->lPtr, symtable, defaultVar, name);
}

// Zpracovává parametry funkce
void validateFunParams(pNode arg, htab_t* symtable, htab_t* localTable, char* name){
    while(arg != NULL){
        if(arg->cond == NULL)break;
        htab_pair_t * paramVar = htab_lookup_add(localTable, arg->cond->value);
        
        if(arg->cond->tokenKey == T_INT){
            paramVar->value = T_INT;
        }else if(arg->cond->tokenKey == T_FLOAT){
            paramVar->value = T_FLOAT;
        }else if(arg->cond->tokenKey == T_STR){
            paramVar->value = T_STR;
        }else if(arg->cond->tokenKey == T_VOID){
            paramVar->value = T_VOID;
        }else if(arg->cond->tokenKey == TN_INT){
            paramVar->value = TN_INT;
        }else if(arg->cond->tokenKey == TN_FLOAT){
            paramVar->value = TN_FLOAT;
        }else if(arg->cond->tokenKey == TN_STR){
            paramVar->value = TN_STR;
        }else if(arg->cond->tokenKey == TN_VOID){
            paramVar->value = TN_VOID;
        }
        arg = arg->next;
    }
}


// Ukládá do tabulky symbolů vestavěné funkce
void loadStaticFunctions(htab_t* symtable){
    htab_lookup_add(symtable, "reads");
    htab_lookup_add(symtable, "readi");
    htab_lookup_add(symtable, "readf");
    htab_lookup_add(symtable, "write");
}

// Načte všechny deklarované proměnné do tabulky symbolů
// parametry funkce ani tělo se zde nenačítají.. načtou se až při druhém průběhu do lokální tabulky
void load(pNode ast, htab_t *symtable){
    while(ast != NULL){
        //printf("\n%d", ast->key);
        switch(ast->key){
            case EXPRESSION:; // Ukladá výraz
                expr * leftVariable = ast->cond->lPtr;
                if(leftVariable != NULL){
                    //printf("Loaded : %s\n", leftVariable->value);
                    htab_pair_t * isFound = htab_find(symtable, leftVariable->value);
                    if(isFound && isFound->pointer != NULL){
                        ast->cond->lPtr = isFound->pointer;
                    }else{
                        htab_pair_t * defVar = htab_lookup_add(symtable, leftVariable->value);

                        defVar->pointer = leftVariable;
                    }
                }

                ast = ast->next;
            break;
            case IF: // Ukladá parametry if
                load(ast->body, symtable);
                load(ast->els, symtable);
                ast = ast->next;
            break;
            case WHILE: // Ukládá parametry While cyklu
                load(ast->body, symtable);
                ast = ast->next;
            break;
            case FUNCTIONCALL:; 
                ast = ast->next;
            break;
            case FUNCTION:; // Deklarace funkce
                htab_pair_t * functionTest = htab_find(symtable, ast->name);
                if(functionTest != NULL){
                    exit(3);
                }else{
                    htab_pair_t * functionObject = htab_lookup_add(symtable, ast->name);
                    pNode arg = ast->cond->funArg;
                    if(arg != NULL){
                        functionObject->arguments = arg;
                    }
                }
                load(ast->body, symtable);
                ast = ast->next;
            break;
        default:
            ast = ast->next;
            break;
        }
    }
}
//Ošetřuje parametry v If a While
void checkparamif(expr* root, htab_t *symtable){
    if(root == NULL){
        return;
    }
    //
    if(root->tokenKey == 1){
        htab_pair_t *isDeclared = htab_find(symtable, root->value);
        if(isDeclared == NULL){
            exit(5);
        }
    }

    checkparamif(root->rPtr, symtable);
    checkparamif(root->lPtr, symtable);
}


//Potvrzuje pokud jsou splněny pravidla pro chyby
//nahraje parametry funkce a návratový typ do lokální tabulky 
//za průchodu funcke načítá i deklarované proměnné v těle a ošetřuje je
//poté tabulku smaže a nechá jen info o funkci (počet parametrů, návratový typ atd..)
void validate(pNode ast, htab_t *symtable, htab_t *localTable, char* name){
    while(ast != NULL){
        switch(ast->key){
            case EXPRESSION:; // Potvrzuje správnost výrazu 
                expr * leftVariable = ast->cond->lPtr;
                if(leftVariable != NULL){
                    htab_pair_t * defVar = htab_find(symtable, leftVariable->value);
                    checkExpression(ast->cond, symtable, defVar, name);
                }
                ast = ast->next;
            break;
            case IF:; // Potvrzuje správnost if
                expr * leftVariableIf = ast->cond->lPtr;
                if(leftVariableIf != NULL){
                    checkparamif(ast->cond, symtable);
                    validate(ast->body, symtable, localTable, name);
                    validate(ast->els, symtable, localTable, name);
                }
                ast = ast->next;
            break;
            case WHILE:; // Potvrzuje správnost while cyklu
                expr * leftVariableWhile = ast->cond->lPtr;
                if(leftVariableWhile != NULL){
                    checkparamif(ast->cond, symtable);
                    validate(ast->body, symtable, localTable, name);
                }
                ast = ast->next;
            break;
            case FUNCTIONCALL: // Potvrzuje správnost volání funkce
                ast = ast->next;
            break;
            case FUNCTION: // Potvrzuje správnost deklarace funkce
                validateFunParams(ast->cond->funArg, symtable, localTable, ast->name);
                load(ast->body, localTable);
                validate(ast->body, localTable, NULL, ast->name);


                ast = ast->next;
            break;
        default:
            ast = ast->next;
            break;
        }
    }
}
//Spuští funkce obsluhy
int semanticCheck(pNode ast){
    htab_t *symtable = htab_init(200);
    htab_t *localTable = htab_init(200);
    loadStaticFunctions(symtable);
    load(ast, symtable); 
    validate(ast, symtable, localTable, NULL);
    htab_clear(symtable);
    htab_free(symtable);
    return 0;
}