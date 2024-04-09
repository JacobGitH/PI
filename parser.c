
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "parser.h"


parsType wrapTokenTypes(tokenType type){
    switch (type)
    {
    case TYPE_ID:
        return KEYWORD;
    case TYPE_ID_DOLLAR:
    case TYPE_INT_VALUE:
    case TYPE_FLOAT_VALUE:
    case TYPE_STRING_VALUE:
    case TYPE_NULL:
        return EXPR_VAR;
    case TYPE_INT:
        return EXPR_INT;
    case TYPE_STRING:
        return EXPR_STR;
    case TYPE_FLOAT:
        return EXPR_FLOAT;
    case TYPE_VOID:
        return EXPR_VOID;
    case TYPE_PLUS:
    case TYPE_MINUS:
    case TYPE_CONCAT:
        return EXPR_ADDSUBCONCAT;
    case TYPE_MULTIPLY:
    case TYPE_DIVISION:
        return EXPR_MULDIV;
    case TYPE_BRACKET_LEFT:
        return EXPR_OPBRACE;
    case TYPE_BRACKET_RIGHT:
        return EXPR_CLBRACE;   
    case TYPE_CURLY_BRACKET_LEFT:
        return EXPR_OPCBRACE;
    case TYPE_CURLY_BRACKET_RIGHT:
        return EXPR_CLCBRACE;  
    case TYPE_LOWER:
    case TYPE_HIGHER:
    case TYPE_LOWER_EQUAL:
    case TYPE_HIGHER_EQUAL:
        return EXPR_LESSGRATER;  
    case TYPE_COMPARE_STR:
    case TYPE_COMPARE_STR_NEG:
        return EXPR_EQTNEQT;  
    case TYPE_EQUAL:
        return EXPR_EQ;    
    case TYPE_IF:
        return IFEMPTY;
    case TYPE_WHILE:
        return WHILEEMPTY;
    case TYPE_FUNCTION:
        return FUNCTIONEMPTY;
    case TYPE_SEMICOLON:
        return SEMICOLON;
    case TYPE_COMMA:
        return EXPR_COMMA;        
    case TYPE_RETURN:
        return RETURN;
    case TYPE_ELSE:
        return ELSE;
    default:
        exit(2);
    }
}

eStack eStackInit (){
    eStack stack = malloc(sizeof(struct stStack));

    stack->cap = 100;
    stack->top = -1;
    stack->item = (eItem*)malloc(sizeof(eItem)*100);
    return stack;
}

pStack pStackInit (){
    pStack stack = malloc(sizeof(struct stParseStack));

    stack->cap = 100;
    stack->top = -1;
    stack->item = (pNode*)malloc(sizeof(pNode)*100);
    return stack;
}

eStackStack eStackStackInit (){
    eStackStack stack = malloc(sizeof(struct stkStack));

    stack->cap = 100;
    stack->top = -1;
    stack->item = (eStack*)malloc(sizeof(eStack)*100);
    return stack;
}

flagStack flagStackInit (){
    flagStack stack = malloc(sizeof(struct flStack));

    stack->cap = 100;
    stack->top = -1;
    stack->flags = (Tuple*)malloc(sizeof(Tuple)*100);
    return stack;
}

void pStackPush (pStack stack, pNode item){
    if(stack->top+1>= stack->cap){
        if(realloc(stack->item, (stack->cap=stack->cap*2))==NULL)
            exit(99);
    }
    
    stack->top++;
    stack->item[stack->top] = item;
}

void eStackPush (eStack stack, eItem item){
    if(stack->top+1>= stack->cap){
        if(realloc(stack->item, (stack->cap=stack->cap*2))==NULL)
            exit(99);
    }
    
    stack->top++;
    stack->item[stack->top] = item;
}

void eStackRemoveAt (eStack stack, int index){
    if (index > stack->top){
        exit(99);
    }
    for(int i = index; i <= stack->top; i++){
        stack->item[i] = stack->item[i+1];
    }
    stack->top--;
}

void eStackInsert (eStack stack, eItem item, int index){
    if (index > stack->top){
        exit(99);
    }
    for(int i = stack->top; i >= index; i--){
        stack->item[i + 1] = stack->item[i];
    }
    stack->item[index] = item;
    stack->top++;
}

void pStackInsert (pStack stack, pNode item, int index){
    if (index > stack->top){
        exit(99);
    }
    for(int i = stack->top; i >= index; i--){
        stack->item[i + 1] = stack->item[i];
    }
    stack->item[index] = item;
    stack->top++;
}

eItem eStackPop (eStack stack){
    if(stack->top < 0){
        return NULL;
    }
    
    eItem item = stack->item[stack->top];
    stack->item[stack->top--] = NULL;
    return item;
}

pNode pStackPop (pStack stack){
    if(stack->top < 0){
        return NULL;
    }
    return stack->item[stack->top--];
}

eItem eStackGetTop (eStack stack){
    if(stack->top < 0){
        return NULL;
    }
    return stack->item[stack->top];
}

pNode pStackGetTop (pStack stack){
    if(stack->top < 0){
        return NULL;
    }
    return stack->item[stack->top];
}

int eStackGetTerminalIndex (eStack stack){
    int index = stack->top;
    if(index < 0){
        return -1;
    }

    while(1){
        if(index < 0){
            return -1;
        }
        if(stack->item[index]->expression == NULL || (stack->item[index]->type == NONVERIFIEDCALL)) break;

        index--;
    }

    return index;
}

/*
 * Funkce na nalezení neterminálního symbolu
 */
int pStackGetTerminalIndex (pStack stack){
    int index = stack->top;
    if(index < 0){
        return -1;
    }

    while(1){
        if(index < 0){
            return -1;
        }
        if(stack->item[index]->key != EXPRESSION && stack->item[index]->key != NONVERIFIEDEXPRESSION && stack->item[index]->key != IF && stack->item[index]->key != WHILE && stack->item[index]->key != FUNCTION  && stack->item[index]->key != FUNCTIONCALL  && stack->item[index]->key != RETURN && stack->item[index]->key != CONDITIONEXPRESSION && stack->item[index]->key != ARGUMENTS ) break;
        index--;
    }

    return index;
}

eItem eStackGetItem (eStack stack, int index){
    if(index > stack->top){
        exit(99);
    }

    return stack->item[index];
}

pNode pStackGetItem (pStack stack, int index){
    if(index > stack->top){
        exit(99);
    }

    return stack->item[index];
}

/*
 * Funkce na získání typu operace na základě typu symbolu na vrcholu stacku a typu příchozího symbolu
 * <  >  = [ ]
 * 1  2  3  4
 */
int expression(parsType stack, parsType input){
    switch (stack)
    {
    case EXPR_EQ:
        if(input == EXPR_INT || input == EXPR_STR || input == EXPR_FLOAT || input == EXPR_VOID)
            exit(2);
        if (input == EXPR_CLBRACE){
            return 2; // >
        }
        return 1; // <    
    case EXPR_LESSGRATER:
        if (input == EXPR_EQTNEQT || input == EXPR_CLBRACE){
            return 2; // >   
        }
        if (input == EXPR_LESSGRATER || input == EXPR_INT || input == EXPR_STR || input == EXPR_FLOAT || input == EXPR_VOID){
            exit(2);
        }
        return 1; // <    
    case EXPR_EQTNEQT:
        if (input == EXPR_EQTNEQT || input == EXPR_INT || input == EXPR_STR || input == EXPR_FLOAT || input == EXPR_VOID){
            exit(2); //error
        }
        if (input == EXPR_CLBRACE){
            return 2; // >   
        }
        return 1; // <    
    case EXPR_ADDSUBCONCAT:
        if (input == EXPR_EQTNEQT || input == EXPR_INT || input == EXPR_STR || input == EXPR_FLOAT || input == EXPR_VOID){
            exit(2); //error
        }
        if (input == EXPR_ADDSUBCONCAT || input == EXPR_CLBRACE || input == EXPR_LESSGRATER || input == EXPR_EQTNEQT){
            return 2; // >
        }
        return 1; // <
    case EXPR_MULDIV:
        if (input == EXPR_EQTNEQT || input == EXPR_INT || input == EXPR_STR || input == EXPR_FLOAT || input == EXPR_VOID){
            exit(2); //error
        }
        if (input == EXPR_VAR || input == NONVERIFIEDCALL || input == FUNCTIONCALL || input == EXPR_OPBRACE){
            return 1; // <
        }
        return 2; // >
    case NONVERIFIEDCALL:
    case EXPR_VAR:
        if (input == EXPR_VAR || input == NONVERIFIEDCALL || input == FUNCTIONCALL || input == EXPR_OPBRACE || input == EXPR_INT || input == EXPR_STR || input == EXPR_FLOAT || input == EXPR_VOID){
            exit(2);
        }
        return 2; // >
    case EXPR_OPBRACE:
        if (input == EXPR_CLBRACE){
            return 3; // =
        } 
        return 1; // <
    case EXPR_CLBRACE:
        if (input == EXPR_OPBRACE || input == EXPR_VAR || input == NONVERIFIEDCALL || input == FUNCTIONCALL || input == EXPR_INT || input == EXPR_STR || input == EXPR_FLOAT || input == EXPR_VOID){
            exit(2); // =
        }
        return 2; // >
    default:
        exit(2);
    }
}

/*
 * Vytvoří ze struktury eItem strukturu pNode
 * V případě volání funkce nastaví jméno funkce
 */
pNode createNode (eItem item){
    pNode node = NULL;

    if (item->argument != NULL){
        node = item->argument;
    }else{
        node = malloc(sizeof(struct stParseNode)); 
        
        if(item->type == NONVERIFIEDCALL || item->type == FUNCTIONNAME){
            node->name = item->val->tokenString;
        }

        node->key = item->type;
        node->cond = item->expression;
        node->arg = NULL;
        node->body = NULL;
        node->els = NULL;
        node->next = NULL;
    }
    free(item);
    
    return node;
}

/*
 * Funkce na převedení typu tokenu ze Skeneru na pomocný typ parseru, využívaný v jiných modulech programu
 * Rozdílnost od parsType je v jednoznačnosti, parsType využívá shlukované typy (př. ADDSUB), zde nabývá jednoznačnost ADD nebo SUB
 */
exprType tokenToEx(tokenType type){
    switch (type)
    {
    case TYPE_NULL:
        return NIL;
    case TYPE_ID_DOLLAR:
        return ID;
    case TYPE_INT_VALUE:
        return INT;
    case TYPE_FLOAT_VALUE:
        return FLOAT;
    case TYPE_STRING_VALUE:
        return STR;
    case TYPE_PLUS:
        return ADD;
    case TYPE_MINUS:
        return SUB;
    case TYPE_MULTIPLY:
        return MUL;
    case TYPE_DIVISION:
        return DIV;
    case TYPE_CONCAT:
        return CONCAT;
    case TYPE_EQUAL:
        return EQ;
    case TYPE_COMPARE_STR:
        return IDENTICAL;
    case TYPE_COMPARE_STR_NEG:
        return NONIDENTICAL;
    case TYPE_HIGHER:
        return GT;
    case TYPE_LOWER:
        return LT;
    case TYPE_HIGHER_EQUAL:
        return GTE;
    case TYPE_LOWER_EQUAL:
        return LTE;
    case TYPE_INT:
        return T_INT;
    case TYPE_FLOAT:
        return T_FLOAT;
    case TYPE_STRING:
        return T_STR;
    case TYPE_VOID:
        return T_VOID;
    case TYPE_COMMA:
        return COMA;
    default:
            return UNDEFINED;
    }
}

/*
 * Inicializuje strukturu 'expression' pro strukturu eItem, aby byla dostupná na úpravu a čtení
 * Dále do ní uloží základní hodnoty z původního převzatého tokenu
 */
void convertToExpression(eItem item){
    expr *e = malloc(sizeof(expr));
    
    e->lPtr = NULL;
    e->rPtr = NULL;
    e->key = item->type;
    e->value = item->val->tokenString;
    e->tokenKey = tokenToEx(item->val->tokenType);
    e->isDefined = false;

    item->expression = e;
    
}

/*
 * Operace < nad zásobníkem výrazů
 * Přidává znak '<' používaný pro oddělování a následně i přidávaný výraz
 * Znak '<' je přidáván k nejbližšímu terminálnímu symbolu, tedy může docházet k posunutí celého zásobníku díky vložení vně
 */
void leftOperation(eStack stack, eItem item, int index){
    // Tvorba znaku <
    eItem indent = malloc(sizeof(struct stItem));
            indent->val = NULL;
            indent->type = EXPR_INDENT;
            indent->expression = NULL;

    // Vkládání znaku na správnou pozici
    if(stack->top == -1 || stack->top == index){
        eStackPush(stack, indent);
    }else{
        eStackInsert(stack, indent, ++index);
    }
    
     // Vkládání výrazu za znak '<'
    eStackPush(stack, item);
}

/*
 * Operace > nad zásobníkem výrazů
 * Redukuje výrazy od vrcholu zásobníku po znak '<' nebo v případě, kdy se žádný '<' znak nenachází na zásobníku, tak po jeho začátek, tedy NULL
 */
void rightOperation(eStack stack){
    int count = 0;
    // Získání počtu výrazů pro určení operace nad zásobníkem
    eStack list = eStackInit();
    while(1){ 
        eItem popped =  eStackPop(stack);
        if (popped->type == EXPR_INDENT) {
            free(popped);
            break;
        }
        count++;
        eStackPush(list, popped);
    }
    // < {x}
    if(count == 1){
        eItem top = eStackPop(list);
        
        if (top->expression != NULL){   // <E
            if(top->type == NONVERIFIEDCALL)
                top->type = FUNCTIONCALL;

            eStackPush(stack, top);
        }else{  // <i
            if(top->type == NONVERIFIEDCALL)
                top->type = FUNCTIONCALL;

            convertToExpression(top);
            eStackPush(stack, top);
        } 
    }else if (count == 3){
        eItem top = eStackPop(list);
        eItem op = eStackPop(list);
        eItem bottom = eStackPop(list);
        
        // ( E )
        if (top->type == EXPR_OPBRACE && bottom->type == EXPR_CLBRACE){
            free(top);
            free(bottom);
            op->braceFlag = true;
            eStackPush(stack, op);
        }else if (op->type == EXPR_MULDIV || op->type == EXPR_ADDSUBCONCAT || op->type == EXPR_EQ || op->type == EXPR_LESSGRATER || op->type == EXPR_EQTNEQT){ //<E */ E
            if((top->type == EXPR_INT ||top->type == EXPR_STR ||top->type == EXPR_FLOAT || top->type == EXPR_VOID || bottom->type == EXPR_INT ||bottom->type == EXPR_STR ||bottom->type == EXPR_FLOAT || bottom->type == EXPR_VOID)){
                exit(2);
            }else if(op->type == EXPR_EQ && !(top->expression->tokenKey == ID || top->expression->tokenKey == CALL))
                exit(2);
            // E (operátor) E
            // kontrola možných chyb a převod na výraz
            convertToExpression(op);

            op->expression->lPtr = top->expression;
            op->expression->rPtr = bottom->expression;

            eStackPush(stack, op);
        }else{
            exit(2);
        }
    }else{
        // Nedefinovaný Počet
        exit(2);
    }

    free(list->item);
    free(list);
    return; 
}

/*
 * Funkce na zpracování zásobníku a vytvoření výsledného výrazu pomocí precedenční tabulky
 */
eItem resolveExpression (eStack expressionStack){
    eStack inputStack = eStackInit();
    int stackTop = expressionStack->top;
    for (int i = 0; i <= stackTop; i++){
        eStackPush(inputStack, eStackPop(expressionStack));
    }
    eStack finalStack = eStackInit();

    while (inputStack->top > -1){
        eItem input = eStackGetTop(inputStack);
        eItem onStack;
        
        int index = eStackGetTerminalIndex(finalStack);

        if(index>=0){ 
            onStack = eStackGetItem(finalStack, index);
        }else{        
            onStack = NULL;
        }

        // Pokud je zásobník prázdný tak je zavolána vkládací operace (<)
        if(onStack == NULL){
            if(input->type == EXPR_CLBRACE){
                // wrong expression -> $)
                exit(2);
            }else{
                // <
                leftOperation(finalStack, eStackPop(inputStack), index);
            }
            continue;
        }

        int op = expression(onStack->type,input->type);
        // Volání operace na základě typů
        if(op == 1){
            leftOperation(finalStack, eStackPop(inputStack), index);
        }else if (op == 2)
        {
            rightOperation(finalStack);
        }else if (op == 3){
            eStackPush(finalStack, eStackPop(inputStack));
            rightOperation(finalStack);
        }else if (op == 4){
            exit(2);
        }
    }
    
    while(finalStack->top !=0){
        rightOperation(finalStack); 
    }

    eItem returned = eStackGetTop(finalStack);

    free(inputStack->item);
    free(finalStack->item);
    free(inputStack);
    free(finalStack);
    
    return returned;
}

/*
 * Funkce ná získání operace na základě přícházející struktury 'pNode' a struktury 'pNode' na vrcholu zásobníku
 */
int getOperation(pNode stack, eItem input){
    if(stack == NULL){
        if(input->type == RETURN)   
            return 3;
        return 1;
    }
    parsType key = stack->key;
    
    switch (input->type)
    {
    case IFEMPTY:
    case WHILEEMPTY:
        if(key == EXPR_INDENT || key == RETURNTYPE)
            return 3;
        exit(2);
    case FUNCTIONEMPTY:
        if(key == NONVERIFIEDFUNCTION || key == NONVERIFIEDCALL){
            return 3;
        }
        exit(2);
    case NONVERIFIEDEXPRESSION:
        if(key == SEMICOLON){
            return 3;
        }else if(key == BODY){
            return 1;
        }
        exit(2); 
    case SEMICOLON:
        return 1; 
    case RETURN:
        return 3;
    case EXPR_OPCBRACE:
        if(key == EXPR_CLCBRACE)
            return 3;
        exit(2); 
    case EXPR_CLCBRACE:
        return 1; 
    case NONVERIFIEDCALL:
        if(key == SEMICOLON || key == RETURNTYPE){
            return 3;
        }
        if(key == BODY){
            return 1;
        }
        exit(2);
    case ELSE:
        if(key == EXPR_INDENT || key == BODY){
            return 3;
        }
        exit(2);
    case RETURNTYPE:
        if(key == EXPR_INDENT){
            return 3;
        }else if(key == BODY){
            return 1;
        }
        exit(2);
    default:
        exit(2);
    }
}

/*
 * Funkce pro vytvoření znaku '<'
 */
pNode pIndent(){
    pNode indent = malloc(sizeof(struct stParseNode));
            indent->key = EXPR_INDENT;
            indent->cond = NULL;
            indent->arg = NULL;
            indent->body = NULL;
            indent->els = NULL;
            indent->next = NULL;
    return indent;
}

/*
 * Funkce pro vložení objektu na hlavní zásobník na základě operace
 */
void mainLeftOperation(pStack stack, pNode item){
    pStackPush(stack, pIndent());
    pStackPush(stack, item);
}

/*
 * Funkce pro redukování hlavního zásobníku. Funkcionálně podobné a založené na funkci 'rightOperation'.
 */
void mainExpand(pStack stack, pNode item){
    int count = 0; 
    pStack list = pStackInit();
    while(1){
        if(stack->top < 0) break; 
        pNode popped =  pStackPop(stack);
        if (popped->key == EXPR_INDENT) {
            free(popped);
            break;
        };
        count++;
        pStackPush(list, popped);
    }

    if(count == 1){
        //;E -> E
        pNode e = pStackPop(list);

        if(item->key == RETURN){
            if(e->key == EXPRESSION || e->key == FUNCTIONCALL || e->key == SEMICOLON){
                if(e->key!=SEMICOLON){
                    item->body = e;
                }
                if(e->cond != NULL && (e->cond->tokenKey == T_INT ||e->cond->tokenKey == T_FLOAT ||e->cond->tokenKey == T_STR ||e->cond->tokenKey == T_VOID || e->cond->tokenKey == TN_INT ||e->cond->tokenKey == TN_FLOAT ||e->cond->tokenKey == TN_STR ||e->cond->tokenKey == TN_VOID))
                    exit(2);
                pStackPush(stack, item);
            }else{
                exit(2);
            }
        }else if(e->key == SEMICOLON){
            free(e);
            if(item->key == NONVERIFIEDEXPRESSION){
                item->key=EXPRESSION;
                pStackPush(stack, item);
            }else if(item->key == NONVERIFIEDCALL){
                item->key = FUNCTIONCALL;
                pStackPush(stack, item);
            }else{
                exit(2);
            }
        }else if(e->key == NONVERIFIEDEXPRESSION || e->key == CONDITIONEXPRESSION || e->key == ARGUMENTS)
        {
            if(item->key == IFEMPTY && e->key == CONDITIONEXPRESSION){
                free(item);
                e->key = IFARG;
                mainExpand(stack, e);
            }else if(item->key == WHILEEMPTY && e->key == CONDITIONEXPRESSION){
                free(item);
                e->key = WHILEARG;
                mainExpand(stack, e);
            }else{
                exit(2); 
            }
        }else if(e->key == BODY)
        {   
            if(item->key == RETURNTYPE){
                e->els = item;
                pStackPush(stack, e);
            }else if(item->key == IFARG){
                e->key = IF;
                e->cond = item->cond;
                if(e->cond == NULL)
                    exit(2);
                pStackPush(stack, e);
            }else if(item->key == WHILEARG){
                e->key = WHILE;
                e->cond = item->cond;
                if(e->cond == NULL)
                    exit(2);
                pStackPush(stack, e);
            }else if(item->key == FUNCTIONARG){
                item->key = NONVERIFIEDFUNCTION;
                item->body = e->body;
                pStackPush(stack, pIndent());
                pStackPush(stack, item);
            }else if(item->key == ELSE){
                item->body = e->body;
                pStackPush(stack, item);
            }else{
                exit(2); 
            }
        }else if(e->key == EXPR_CLCBRACE){
            if(item->key == EXPR_OPCBRACE){
                pStackPush(stack, pIndent());
                e->key = BODY;
                e->body = NULL;
                pStackPush(stack, e);
            }else{
                exit(2);
            }
        }else if(e->key == NONVERIFIEDCALL){
            if(item->key == FUNCTIONEMPTY){
                pNode checkedArg = e->cond->funArg;
                while (checkedArg != NULL)
                {
                    if(!(checkedArg->cond->tokenKey == ID || checkedArg->cond->tokenKey == CALL ||
                            checkedArg->cond->tokenKey == T_INT || checkedArg->cond->tokenKey == T_FLOAT || 
                            checkedArg->cond->tokenKey == T_STR || checkedArg->cond->tokenKey == T_VOID ||
                            checkedArg->cond->tokenKey == TN_INT || checkedArg->cond->tokenKey == TN_FLOAT || 
                            checkedArg->cond->tokenKey == TN_STR || checkedArg->cond->tokenKey == TN_VOID))
                        exit(2);
                    
                    checkedArg = checkedArg->next;
                }

                e->key = FUNCTION;
                pNode bodyPopped = pStackPop(stack);
                
                if(pStackGetTop(stack)->key == EXPR_INDENT) pStackPop(stack);

                if(bodyPopped->key == BODY){
                    e->body = bodyPopped->body;
                }else{
                    exit(2);
                }
                 pStackPush(stack, e);
            }else{
                exit(2);
            }
        }else if(e->key == RETURNTYPE){
            if(item->key == NONVERIFIEDCALL){
                pNode checkedArg = item->cond->funArg;
                while (checkedArg != NULL)
                {
                    if(!(checkedArg->cond->tokenKey == ID || checkedArg->cond->tokenKey == CALL ||
                            checkedArg->cond->tokenKey == T_INT || checkedArg->cond->tokenKey == T_FLOAT || 
                            checkedArg->cond->tokenKey == T_STR || checkedArg->cond->tokenKey == T_VOID ||
                            checkedArg->cond->tokenKey == TN_INT || checkedArg->cond->tokenKey == TN_FLOAT || 
                            checkedArg->cond->tokenKey == TN_STR || checkedArg->cond->tokenKey == TN_VOID))
                        exit(2);
                    
                    checkedArg = checkedArg->next;
                }
            
                item->els = e;
                item->key = FUNCTIONARG;
                mainExpand(stack, item);
            }else{
                exit(2);
            }
        }else if(e->key == NONVERIFIEDFUNCTION){
            if(item->key == FUNCTIONEMPTY){
                e->key = FUNCTION;
                pStackPush(stack, e);
            }else{
                exit(2);
            }
        }else{
            exit(2);
        }   
    }else if (count == 2){
        pNode check = pStackPop(list);
        pNode e = pStackPop(list);

        if((item->key == RETURN && item->body == NULL) && (e->key == EXPRESSION || e->key == FUNCTIONCALL)){
            item->body = e;

            if(check->key == EXPR_CLCBRACE)
            {
                pStackPush(stack, pIndent());
                pStackPush(stack, check);  
            }
                
            pStackPush(stack, item);
        }else if(check->key == EXPR_CLCBRACE){
            // <{body}
            pStackPush(stack, pIndent());
            check->key  = BODY;
            check->body = e;

            pStackPush(stack, check);
        }else if(item->key == FUNCTIONNAME && check->key == BODY && e->key == RETURNTYPE){
            item->key = FUNCTIONARG;
            item->arg = e->arg;
            item->els = e->els;
            item->body = check->body;
            pStackPush(stack, item);
            
        }else{
            exit(2);
        }   
    }else{
        exit(2);
    }
    
    // Pokud je zásobník nenulový, program se pokusí o slepení zásobníku
    checkConnect(stack);

    free(list->item);
    free(list);
}

/*
 * Funkce pro přímé slepení příchozích struktur 'pNode'
 */
pNode connectTwo(pNode first, pNode second){
    second->next = first;
    return second;
}

/*
 * Funkce pro slepování funkcí a příkazů.
 * Je kontrolována velikost zásobníku a správné pořadí slepovaných struktur.
 */
void checkConnect(pStack stack){
    if(stack->top>0){
        pNode first = pStackGetItem(stack, stack->top);
        pNode second = pStackGetItem(stack, stack->top-1);

        if(first->key == IF && second->key == ELSE){
            pNode ifNode = pStackPop(stack);
            pNode elseNode = pStackPop(stack);
            
            ifNode->els = elseNode->body;
            pStackPush(stack, ifNode);
            return;
        }
        
        if( (first->key == EXPRESSION || first->key == IF || first->key == WHILE || first->key == FUNCTION || first->key == FUNCTIONCALL || first->key == RETURN) &&
        (second->key == EXPRESSION || second->key == IF || second->key == WHILE || second->key == FUNCTION || second->key == FUNCTIONCALL || second->key == RETURN))
        {   
            pNode result = connectTwo(pStackPop(stack), pStackPop(stack));
            pStackPush(stack, result);
        }
        if(first->key == NONVERIFIEDCALL || first->key == NONVERIFIEDEXPRESSION || second->key == NONVERIFIEDCALL || second->key == NONVERIFIEDEXPRESSION || second->key == ELSE )
            exit(2);
    }
}

/*
 * Funkce pro práci s hlavním zásobníkem programu. 
 * Tento zásobník vyhodnocuje funkce a příkazy, které jsou postupně slepovány do jednoho konečného stromu, který je zpracováván dalšími moduly programu.
 */
pNode resolveMainStack(eStack stack){
    pStack main = pStackInit();

    while(stack->top > -1){
        eItem input = eStackGetTop(stack);

        // Slepování funkcí
        if(main->top > -1 && pStackGetTop(main)->key == FUNCTIONARG){
            if(input->type == FUNCTIONEMPTY){
                free(eStackPop(stack));
                pStackGetTop(main)->key = FUNCTION;
                pStackGetTop(main)->arg->name = pStackGetTop(main)->name;
                checkConnect(main);
                continue;
            }else{
                exit(2);
            }
        }

        pNode onStack;
        
        int index = pStackGetTerminalIndex(main);

        if(index>=0){ 
            onStack = pStackGetItem(main, index);
        }else{      
            onStack = NULL;
        }

        int op = getOperation(onStack, input);

        if(op == 1){
            mainLeftOperation(main, createNode(eStackPop(stack)));
        }else if (op == 3){
            mainExpand(main, createNode(eStackPop(stack)));
        }else{
            exit(2);
        }
    }

    // Detekce zacyklení, v případě, že se zásobník neredukuje a zůstává stejný je tento případ odchycen aby nedošlo ke zbytečnému cyklení
    int antiLoop = main->top;
    while(main->top != 0){
        checkConnect(main);

        if(antiLoop == main->top)
            exit(2);
    }

    pNode returned = pStackPop(main);

    free(main->item);
    free(main);
    return returned;
}

void freeExpr(expr* root)
{
    if (root == NULL)
        return;
    freeExpr(root->rPtr);
    freeExpr(root->lPtr);
    free(root);
}

void freeTree(pNode ast){
    while(1){
        if(ast == NULL) break;
        pNode current = ast;
        ast = ast->next;
        freeTree(current->arg);
        freeExpr(current->cond);
        freeTree(current->body);
        freeTree(current->els);
        free(current);
    }
}

/*
 * Funkce pro řešení závorek mimo výraz (př. Argumenty funkce)
 * V případě argumentů slepuje výrazy jako struktury 'pNode' do jedné výsledné
 */
eItem resolveBrace(eStack stack){
    bool comaFlag = false;
    pNode tmpItem = createNode(eStackPop(stack));
    pNode item = createNode(eStackPop(stack));

    while(item->key != EXPR_OPBRACE){
        if(item->key != EXPR_COMMA){
            item->next = tmpItem;
            tmpItem = item;
        }else{
            free(item);
            item = NULL;
            comaFlag = true;
        }
        item = createNode(eStackPop(stack));
    }
    free(item);
    if(comaFlag){
        tmpItem->key = ARGUMENTS;
    }else{
        tmpItem->key = CONDITIONEXPRESSION;
    }
    
    eItem finalItem = malloc(sizeof(struct stItem));
                finalItem->val = NULL;
                finalItem->type = NONVERIFIEDEXPRESSION;
                finalItem->expression = NULL;
                finalItem->braceFlag = true;
                finalItem->argument = tmpItem;

    return finalItem;
}

eStack eStackStackPop(eStackStack stack){
    if(stack->top < 0){
        return NULL;
    }
    
    eStack item = stack->item[stack->top];
    stack->item[stack->top--] = NULL;
    return item;
}

void eFlagStackPush(flagStack stack, Tuple item){
    if(stack->top+1>= stack->cap){
        if(realloc(stack->flags, (stack->cap=stack->cap*2))==NULL)
            exit(99);
    }

    stack->top++;
    stack->flags[stack->top] = item;
}

Tuple eFlagStackPop(flagStack stack){
    if(stack->top < 0){
        return NULL;
    }
    
    Tuple item = stack->flags[stack->top];
    stack->flags[stack->top--] = NULL;
    return item;
}

void eStackStackPush(eStackStack stack, eStack item){
    if(stack->top+1>= stack->cap){
        if(realloc(stack->item, (stack->cap=stack->cap*2))==NULL)
            exit(99);
    }

    stack->top++;
    stack->item[stack->top] = item;
}

/*
 * Hlavní funkce programu
 * Získává tokeny z lexikální analýzy, které jsou následně zpravovávány a jejich posloupnosti vyhodnocovány a slepovány jako výrazy nebo struktury výsledného programu
 * Výstupem je struktura 'pNode' ukazující na celý program jako na Abstraktní Syntaktický Strom
 */
pNode parse (){
    eStack stack = eStackInit();
    eStack program = eStackInit();
    eStackStack funStack = eStackStackInit();
    flagStack fStack = flagStackInit();

    tokenList tokens;

    // Statická kontrola declare(strict_type=1);
    if( strcmp(getToken()->tokenString, "declare") || strcmp(getToken()->tokenString, "(") || 
        strcmp(getToken()->tokenString, "strict_types") || strcmp(getToken()->tokenString, "=") || 
        strcmp(getToken()->tokenString, "1") || strcmp(getToken()->tokenString, ")") || 
        strcmp(getToken()->tokenString, ";")){
        exit(2);
    }

    // Získání prvního tokenu
    tokens = getToken();

    bool semicolonFlag = false;
    bool returnType = false;
    bool braceFlag = false;
    bool keywordFlag = false;
    bool isType = false;

    int wasBrace = 0;
    int nestedFlag = false;
    int braceBuffer = 0;

    int clB = 0;
    int opB = 0;

    // Cyklus do konce souboru nebo konce programu (?>)
    while(true){
        if(tokens->tokenType == TYPE_EOF || tokens->tokenType == TYPE_EOP)
            break;

        if(semicolonFlag && tokens->tokenType == TYPE_SEMICOLON){
            tokens = getToken();
            continue;
        }
        if(tokens->tokenType == TYPE_COLON){
            returnType = true;
            tokens = getToken();
            continue;
        }
        
        eItem item = malloc(sizeof(struct stItem));
            item->val = tokens;
            item->type = wrapTokenTypes(tokens->tokenType);
            item->expression = NULL;
            item->braceFlag = false;
            item->argument = NULL;

        if(item->type == EXPR_OPBRACE){
            wasBrace++;
        }else{
            wasBrace = 0;
        }
        
        if(item->type == EXPR_OPCBRACE){
            nestedFlag++;
        }else if(item->type == EXPR_CLCBRACE){
            nestedFlag--;
        }

        /*
         * Kontrola vnořených funkcí 
         */
        if(item->type == FUNCTIONEMPTY && nestedFlag != 0){
            exit(2);
        }

        // Kontrola Typů
        if( item->type == EXPR_INT ||
            item->type == EXPR_STR ||
            item->type == EXPR_FLOAT ||
            item->type == EXPR_VOID ){
                if(isType)
                    exit(2);
                isType = true;
            }else{
                isType = false;
            }
        
        // Pokud je token možnou součástí výrazu, je ukládán na zásobník výrazů
        if( (item->type == EXPR_VAR ||
            item->type == EXPR_ADDSUBCONCAT ||
            item->type == EXPR_MULDIV ||
            item->type == EXPR_EQ ||
            ((item->type == EXPR_OPBRACE && stack->top > -1) || (item->type == EXPR_OPBRACE && wasBrace > 1)) ||
            (item->type == EXPR_CLBRACE && stack->top > -1) ||
            item->type == EXPR_LESSGRATER ||
            item->type == EXPR_EQTNEQT) && clB <= opB ){
                if(item->type == EXPR_OPBRACE)
                    opB++;
                if(item->type == EXPR_CLBRACE)
                    clB++;
                eStackPush(stack, item);
        }else if(item->type == KEYWORD){
            // Ukončení chybou 2, pokud přichází více klíčových slov za sebou
            if(keywordFlag)
                exit(2);
            item->type = NONVERIFIEDCALL;

            keywordFlag = true;

            eStackPush(stack, item);
            eStackStackPush(funStack, stack);

            Tuple t = malloc(sizeof(Tuple));
            t->o = opB;
            t->c = clB;
            eFlagStackPush(fStack, t);

            opB = 0;
            clB = 0;
            
            stack = NULL;
            stack = eStackInit();
        }else{
            if(keywordFlag){
                // klíčové slovo není následováno závorkami, není volání funkce -> chyba 
                if(item->type != EXPR_OPBRACE)
                    exit(2);
                keywordFlag = false;
            }
      
            if(item->type == EXPR_OPBRACE){
                if(braceFlag)
                    braceBuffer++;
                braceFlag = true;  
            }
            
            // Redukce zásobníku výrazů v případě jeho zaplnění
            if(stack->top > -1){ 
                eItem lastBrace = NULL;

                if(braceFlag && item->type != EXPR_COMMA){
                    
                    lastBrace = eStackPop(stack);

                    if(lastBrace->type != EXPR_CLBRACE)
                    {
                      exit(2);
                    }
                }

                eItem e = resolveExpression(stack);

                if( program->top > -1 && (eStackGetTop(program)->type == EXPR_INT ||
                    eStackGetTop(program)->type == EXPR_STR ||
                    eStackGetTop(program)->type == EXPR_FLOAT ||
                    eStackGetTop(program)->type == EXPR_VOID )){
                        if(!(e->expression != NULL && e->expression->tokenKey == ID)){
                            exit(2);
                        }
                        eItem typeItem = eStackPop(program);
                        exprType id;
                        if(typeItem->type == EXPR_INT){
                            if(typeItem->val->tokenString[0] == '?'){
                                id = TN_INT;
                            }else{
                                id = T_INT;
                            }
                        } else if(typeItem->type == EXPR_STR){
                            if(typeItem->val->tokenString[0] == '?'){
                                id = TN_STR;
                            }else{
                                id = T_STR;
                            }
                        } else if(typeItem->type == EXPR_FLOAT){
                            if(typeItem->val->tokenString[0] == '?'){
                                id = TN_FLOAT;
                            }else{
                                id = T_FLOAT;
                            }
                        } else if(typeItem->type == EXPR_VOID){
                            if(typeItem->val->tokenString[0] == '?'){
                                id = TN_VOID;
                            }else{
                                id = T_VOID;
                            }
                        }  
                        e->type = typeItem->type;
                        e->expression->tokenKey = id;
                    }
                    

                if(e->type == FUNCTIONCALL)
                    e->type = NONVERIFIEDCALL;
 
                eItem newItem = malloc(sizeof(struct stItem));
                newItem->val = e->val;

                if(e->type == NONVERIFIEDCALL){
                    newItem->type = NONVERIFIEDCALL;
                }else{
                    newItem->type = NONVERIFIEDEXPRESSION;
                }
                
                newItem->expression = e->expression;
                eStackPush(program, newItem); 
                
                if(braceFlag && item->type != EXPR_COMMA){
                    if(braceBuffer > 0){
                        braceBuffer--;
                    }else{
                       braceFlag = false; 
                    }
                    
                    eItem resolvedBrace = resolveBrace(program);

                    if (funStack->top != -1){
                        stack = eStackStackPop(funStack);

                        expr *ex = malloc(sizeof(expr));
                            ex->funArg = createNode(resolvedBrace);
                            ex->tokenKey = CALL;
                            ex->value = eStackGetTop(stack)->val->tokenString;
                            ex->lPtr = NULL;
                            ex->rPtr = NULL;

                        Tuple t = eFlagStackPop(fStack);
                        opB = t->o;
                        clB = t->c;

                        eStackGetTop(stack)->expression = ex;
                        if(isType)
                            isType = false;
                         
                        if(item->type == EXPR_OPCBRACE){
                            nestedFlag--;
                        }else if(item->type == EXPR_CLCBRACE){
                            nestedFlag++;
                        }
                        continue;
                    }else{
                        opB = 0;
                        clB = 0;
            
                        eStackPush(program, resolvedBrace);
                    }
                }
            }
            if(braceFlag && item->type == EXPR_CLBRACE){
                // chyba pokud nejde o funkci, přiřazení (E) 
                if(funStack->top == -1)
                    exit(2);
                // odstranění (
                eStackPop(program);
                stack = eStackStackPop(funStack);

                convertToExpression(eStackGetTop(stack));
                eStackGetTop(stack)->expression->tokenKey = CALL;
                
                Tuple t = eFlagStackPop(fStack);
                opB = t->o;
                clB = t->c;

                if(braceBuffer > 0){
                    braceBuffer--;
                }else{
                    braceFlag = false; 
                }
            }else{
                if(returnType){
                    if(item->type == EXPR_VAR ||
                        item->type == EXPR_INT ||
                        item->type == EXPR_STR  ||
                        item->type == EXPR_FLOAT  ||
                        item->type == EXPR_VOID){
                            item->type = RETURNTYPE;
                            convertToExpression(item);
                            eStackPush(program, item);
                            returnType = false;
                        }else{
                            exit(2);
                        }
                }else{
                    eStackPush(program, item);
                }
            }
        } 
        semicolonFlag = (tokens->tokenType == TYPE_SEMICOLON);

        tokens = getToken();
    }

    // V případě zaplnění zásobníku výrazů a následném ukončení čtení tokenů je zásobník vyhodnocen
    if(stack->top > -1){ 
        eItem e = resolveExpression(stack);
        if(e->type == NONVERIFIEDCALL){
            eStackPush(program, e); 
        }else{
            eItem eItem = malloc(sizeof(struct stItem));
            eItem->val = NULL;
            eItem->type = NONVERIFIEDEXPRESSION;
            eItem->expression = e->expression;
            eItem->braceFlag = false;

            eStackPush(program, eItem); 
        }   
    }

    /**
     * Od vrcholu zásobníku (Od spoda programu nahoru) slepuje prvky do jednoho ukazatele na celý program
     * Následně vrací ukazatel do main.c kde může být zpracován sémantikou a generátorem kódu
     */
    pNode ast = resolveMainStack(program);

    free(stack->item);
    free(program->item);
    free(stack);
    free(program);
    
    return ast;
}