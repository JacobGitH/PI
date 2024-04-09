
#include "generator.h"
#include "parser.h"
   // escape sekvence kódem 000-032, 035 a 092
void rectifyString(char* value){
    char c;
    char seq[10];
    value = (value+(1*sizeof(char)));
    for(int i = 0; i < strlen(value)-1;i++){
        c = *(value+(i*sizeof(char)));
        if(c <= 9 && c >= 0){
            sprintf(seq,"%d",(int)c);
            printf("\\00%s",seq);
        }
        else if(c <= 32 || c == 35 || c == 92){
            sprintf(seq,"%d",(int)c);
            printf("\\0%s",seq);
        }
        else
            printf("%c",c);
    }
    return;
}

void genSymbol(expr* cond) {
    switch (cond->tokenKey)
    {
    case NIL:
        printf("nil@nil");
        break;
    
    case ID:
        printf("LF@%s_%li", cond->value, (long)cond);
        break;

    case INT:
        printf("int@%s", cond->value);
        break;

    case FLOAT:
        printf("float@0x%s", cond->value);
        break;

    case STR:
        printf("string@");
        rectifyString(cond->value);
        break;
    
    default:
        break;
    }
}

//nop
int genExpression(pNode node){
    switch (node->cond->tokenKey) {
        case EQ:
            if (!node->cond->isDefined) {
                printf("DEFVAR LF@%s_%li\n",node->cond->lPtr->value, (long)node->cond->lPtr);
                node->cond->isDefined = true;
            }
            if(node->cond->rPtr->tokenKey > 4){
                genCondition(node->cond->rPtr);
                printf("POPS LF@%s_%li\n",node->cond->lPtr->value, (long)node->cond->lPtr);
            }
            else{
                printf("MOVE LF@%s_%li ",node->cond->lPtr->value, (long)node->cond->lPtr);
                genSymbol(node->cond->rPtr);
                printf("\n");
            }
            return branchTopLevel(node->next);

        case UNDEFINED:
        default:
            return 1; // Error klíč
    }
}



int genFunctioncall(pNode node){
    pNode funArg;
    int i = 0;
    if(node->cond->funArg != NULL)
        funArg = node->cond->funArg;
    else
        funArg = NULL;
    if(strcmp(node->name,"write")==0){
        if(funArg->cond->tokenKey == ID){
            printf("WRITE LF@%s_%li",funArg->cond->value, (long)funArg->cond);
        }else{
            printf("WRITE ");
            genSymbol(funArg->cond);
        }
        printf("\n");
        return branchTopLevel(node->next);
    }
    else if(strcmp(node->name,"readi")==0){
        printf("READ TF@retval int\n");
        return branchTopLevel(node->next);
    }
    else if(strcmp(node->name,"reads")==0){
        printf("READ TF@retval string\n");
        return branchTopLevel(node->next);
    }
    else if(strcmp(node->name,"readf")==0){
        printf("READ TF@retval float\n");
        return branchTopLevel(node->next);
    }
    printf("CREATEFRAME\n");
    //předej argumenty do TF
    while(funArg != NULL){
        printf("DEFVAR TF@%%%d\n",i);
        printf("MOVE TF@%%%d ",i);
        genSymbol(funArg->cond);
        printf("\n");
        funArg = funArg->next;
        i++;
    }
    printf("DEFVAR TF@retval\n");
    printf("CALL %s\n",node->name);
    return branchTopLevel(node->next);
}

int genInlineCall(expr* node){
    pNode funArg;
    int i = 0;
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@retval\n");
    if(node->funArg != NULL)
        funArg = node->funArg;
    else
        funArg = NULL;
    if(strcmp(node->value,"write")==0){
        if(funArg->cond->tokenKey == ID){
            printf("WRITE LF@%s_%li",funArg->cond->value, (long)funArg->cond);
        }else{
            printf("WRITE ");
            genSymbol(funArg->cond);
        }
        printf("\n");
        return 0;
    }
    else if(strcmp(node->value,"readi")==0){
        printf("READ TF@retval int\n");
    }
    else if(strcmp(node->value,"reads")==0){
        printf("READ TF@retval string\n");
    }
    else if(strcmp(node->value,"readf")==0){
        printf("READ TF@retval float\n");
    }
    else {
       //předej argumenty do TF
        while(funArg != NULL){
            printf("DEFVAR TF@%%%d\n",i);
            printf("MOVE TF@%%%d ",i);
            genSymbol(funArg->cond);
            printf("\n");
            funArg = funArg->next;
            i++;
        } 
        printf("CALL %s\n",node->value);
    }

    printf("PUSHS TF@retval\n");
    return 0;
}

int genReturntype(pNode node){
    
    return 0;
}

int genCondition(expr* cond) {
   if(cond == NULL)
        return 0;
   genCondition(cond->lPtr);
   genCondition(cond->rPtr);
   
   switch (cond->tokenKey) {
        case ADD:  // +
            printf("ADDS\n");
            break;
        case CALL:
            genInlineCall(cond);
        break;

        case SUB:  // -
            printf("SUBS\n");
            break;
        case MUL: // *
            printf("MULS\n");
            break;
        case DIV: // /
            printf("CREATEFRAME\n");
            printf("PUSHFRAME\n");
            printf("DEFVAR LF@a\n");
            printf("DEFVAR LF@b\n");
            printf("DEFVAR LF@type\n");
            printf("DEFVAR LF@result\n");
            printf("POPS LF@b\n");
            printf("POPS LF@a\n");
            printf("TYPE LF@type LF@a\n");
            printf("JUMPIFEQ %%intdiv LF@type string@int\n");
            printf("DIV LF@result LF@a LF@b\n");
            printf("JUMP %%divend\n");
            printf("LABEL %%intdiv\n");
            printf("IDIV LF@result LF@a LF@b\n");
            printf("LABEL %%divend\n");
            printf("PUSHS LF@result\n");
            printf("POPFRAME\n");
            break;
        case CONCAT: // .
            printf("CREATEFRAME\n");
            printf("PUSHFRAME\n");
            printf("DEFVAR LF@a\n");
            printf("DEFVAR LF@b\n");
            printf("DEFVAR LF@result\n");
            printf("POPS LF@b\n");
            printf("POPS LF@a\n");
            printf("CONCAT LF@result LF@a LF@b\n");
            printf("PUSHS LF@result\n");
            printf("POPFRAME\n");
            break;
        case IDENTICAL:  // ===
            printf("EQS\n");
            break;
        case NONIDENTICAL:  // !==
            printf("EQS\n");
            printf("NOTS\n");
            break;
        case GT:  // >
            printf("GTS\n");
            break;
        case LT:  // <
            printf("LTS\n");
            break;
        case GTE:  // >=
            printf("GTS\n");
            genCondition(cond->lPtr);
            genCondition(cond->rPtr);
            printf("EQS\n");
            printf("ORS\n");
            break;
        case LTE:  // <=
            printf("LTS\n");
            genCondition(cond->lPtr);
            genCondition(cond->rPtr);
            printf("EQS\n");
            printf("ORS\n");
            break;
        default:
            if (cond->tokenKey <= 4)
            {
                printf("PUSHS ");
                genSymbol(cond);
                printf("\n");
            }
            break;
   }
   
   return 0;
}

int genIf(pNode node){
    genCondition(node->cond);
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS jump_%li\n",(long)node);
    if(node->body){
        branchTopLevel(node->body);
        printf("JUMP jump2_%li\n",(long)node);
    }
    printf("LABEL jump_%li\n",(long)node);
    if(node->els){
        branchTopLevel(node->els);
    }
    printf("LABEL jump2_%li\n",(long)node);
    return branchTopLevel(node->next);
}


int genWhile(pNode node){
    printf("LABEL while_%li\n", (long)node);
    genCondition(node->cond);
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS end_%li\n", (long)node);
    if(node->body){
        branchTopLevel(node->body);
    }
    printf("JUMP while_%li\n", (long)node);
    printf("LABEL end_%li\n", (long)node);
    return branchTopLevel(node->next);
}

//vygenerování hlavičky funkce -- jméno návěští, vytvoření rámce
int genFunction(pNode node){
    printf("JUMP funcdef_%li\n", (long)node);
    printf("LABEL %s\n",node->name);
    printf("PUSHFRAME\n");
    branchTopLevel(node->body);
    printf("LABEL funcdef_%li\n", (long)node);
    return branchTopLevel(node->next);
}

// bral jsem to i jako vygenerování konce funkce (return), pokud nemá návratovou hodnotu, hodnota je nil@nil
int genReturn(pNode node){
    if (node->body) {
        branchTopLevel(node->body); 
        printf("POPS LF@retval\n");
    }
    else {
        printf("MOVE LF@retval nil@nil\n");
    }
    printf("POPFRAME\n");
    printf("RETURN\n");
    return branchTopLevel(node->next);
}


int branchTopLevel(pNode node){
    if(node == NULL)
        return 1;
    switch (node->key) {
        case EXPRESSION:
            return genExpression(node);
        case FUNCTIONCALL:
            return genFunctioncall(node);
        case RETURNTYPE:
            return genReturntype(node);
        case RETURN:
            return genReturn(node);
        case IF:
            return genIf(node);
        case WHILE:
            return genWhile(node);
        case FUNCTION:
            return genFunction(node);
        default:
            return 1; // neznámý klíč
    }
}


int generate(pNode node){
    printf(".IFJcode22\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    int ret = 0;
    
    while(ret == 0){
        if(node == NULL) break;
        ret = branchTopLevel(node);
        node = node->next;
    }

    return ret;
}