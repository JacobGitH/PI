
 */

#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#define nullptr NULL
#define ERROR_INTERNAL 99
#define ERROR_LEXICAL 1


/**
 * @brief Debug funkce pro výpis jednotlivých tokenů
 * 
 * @param tokens 
 * @param showInfo 
 */

tokenList FSM(){
    /**
     * @brief Řídící proměnná konečného stavového automatu
     */
    FSMState state  = MOVE_START;
    /**
     * @brief Dvousměrně vázaný lineární seznam pro ukládání tokenů
     */
    static tokenList token;
    //hlavička na začátek seznamu
    static tokenList head;
    int tokenStringSizeMax = 8; // proměnné pro kontrolu maximální délky 
                                 // stringu v paměti (dynamicky se zvedá při zaplnění na dvojnásobek)
    int tokenStringSizeCurrent = 0;
    bool eop = false;
    bool write = false; //povoluje zápis dat do stringu tokenu

    int counter = 0; // proměnná počítá počet opakování ve stavu (pouze u některých které to ke své funkci vyžadují)
    bool isDollar = false; // ternární operátor poté podle této proměnné kontroluje zda se jedná o identifikátor s dollarem či nikoliv
    char c; // právě čtený znak
    char swap[2]; //přenášení čteného znaku do pole abych mohl přidat EOS charakter a použít strcat

    token = malloc(sizeof(struct token));

    if(token == nullptr){
        exit(ERROR_INTERNAL);
    }
    char sequence[4]; // Proměnná pro převod escape sekvence

    // Inicializace prvního tokenu
    token->charLinePos = 0;
    token->linePos = 0;
    token->tokenString = calloc(tokenStringSizeMax,sizeof(char)); // Alokuje pamět pro string, viz tokenStringSizeMax
    token->tokenType = TYPE_UNDEF;
    head = token;
    //kontrola <?php
    bool header = false; //kontroluje header <?php tím, že se nastaví na true po tom, co je zpracován první token, pokud token není header, lex chyba
    c = fgetc(stdin); // Načte první znak
    while(true){
        //Pamět ve stringu není dostatečná
        if(tokenStringSizeMax == tokenStringSizeCurrent+1) 
            token->tokenString = realloc(token->tokenString, (tokenStringSizeMax=tokenStringSizeMax*2));
        if(token->tokenString == nullptr)
            exit(ERROR_INTERNAL);

        //povolení pro zápis dat do struktury pro token
        write = false;
        
        if(c == 10)
            token->linePos++;
        switch(state){
            case MOVE_START:
                // Identifikátor
                
                if(isspace(c)){
                    if(!header){
                        fprintf(stderr,"ERROR LEXICAL\n");
                        exit(ERROR_LEXICAL);
                    }
                    break;
                }
                else if(c == '$'){
                    write = true;
                    state = MOVE_ID; 
                }
                //a-z A-Z
                else if((c >= 65 && c <= 90) || (c >= 97 && c <=122) || c=='_'){
                    write = true;
                    state = MOVE_KEYWORD;
                }
                else if(c == '?'){
                    write = true;
                    state = MOVE_QUESTIONMARK;
                }
                else if(c == '('){
                    token->tokenType = TYPE_BRACKET_LEFT;
                    write = true;
                    state = MOVE_END;
                }
                else if(c == ')'){
                    token->tokenType = TYPE_BRACKET_RIGHT;
                    write = true;
                    state = MOVE_END;
                }
                else if(c >= 48 && c <= 57){
                    write = true;
                    state = MOVE_LITERAL;
                }
                else if(c == '"'){
                    write = true;
                    state = MOVE_STRING_CONTENT;
                }
                else if(c == '+'){
                    token->tokenType = TYPE_PLUS;
                    write = true;
                    state = MOVE_END;
                }
                else if(c == '*'){
                    token->tokenType = TYPE_MULTIPLY;
                    write = true;
                    state = MOVE_END;
                }
                else if(c == '-'){
                    token->tokenType = TYPE_MINUS;
                    write = true;
                    state = MOVE_END;
                }
                else if(c == '/'){
                    write = true;
                    state = MOVE_EXPECT_COMMENT;
                }
                else if(c == ';'){
                    token->tokenType = TYPE_SEMICOLON;
                    write = true;
                    state = MOVE_END;
                }
                else if(c == '.'){
                    token->tokenType = TYPE_CONCAT;
                    write = true;
                    state = MOVE_END;
                }
                else if(c == ':'){
                    token->tokenType = TYPE_COLON;
                    write = true;
                    state = MOVE_END;
                }
                else if(c == ','){
                    token->tokenType = TYPE_COMMA;
                    write = true;
                    state = MOVE_END;
                }
                else if(c == '='){
                    write = true;
                    state = MOVE_EXPECT_EQUAL;
                }
                else if(c == '!'){
                    write = true;
                    state = MOVE_EXPECT_EQUAL_NEG;
                }
                else if(c == '{'){
                    write = true;
                    state = MOVE_END;
                    token->tokenType = TYPE_CURLY_BRACKET_LEFT;
                }
                else if(c == '}'){
                    write = true;
                    state = MOVE_END;
                    token->tokenType = TYPE_CURLY_BRACKET_RIGHT;
                }
                else if(c == '>'){
                    write = true;
                    state = MOVE_HIGHER;
                }
                else if(c == '<'){
                    //byl už header <?php ?
                    if(!header){
                        state = MOVE_HEADER;
                    }
                    else{
                        write = true;
                        state = MOVE_LOWER;
                    }
                }
                else if(c == EOF){
                    token->tokenType = TYPE_EOF;
                    return head;
                }
                else{
                    
                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }
                break;


            case MOVE_QUESTIONMARK:
                //a-z A-Z
                if((c >= 65 && c <= 90) || (c >= 97 && c <=122)){
                    write = true;
                    state = MOVE_KEYWORD;
                }
                else if(c == '>'){
                    write = true;
                    token->tokenType = TYPE_EOP;
                    eop = true;
                    state = MOVE_END;
                }
                else{
                    fprintf(stderr, "ERROR LEXICAL \n");
                    exit(ERROR_LEXICAL);
                }
            break;

            // "<"
            case MOVE_HEADER:
                if (counter == 0 && c != '?')
                {

                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }

                if(counter == 1 && c != 'p')
                {

                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }

                if(counter == 2 && c != 'h')
                {

                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }

                if(counter == 3 && c == 'p')
                {
                    counter = 0;
                    header = true;
                    state = MOVE_START; 
                    break;
                }
                else 
                if(counter == 3 && c != 'p'){
                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }
                counter++;
                break;
            case MOVE_LOWER:
                if(c == '='){
                    write = true;
                    token->tokenType = TYPE_LOWER_EQUAL;
                    state = MOVE_END;
                }
                else{
                    write = true;
                    token->tokenType = TYPE_LOWER;
                    state = MOVE_END;
                    continue;
                }
                break;
            case MOVE_HIGHER:
                if(c == '='){
                    write = true;
                    token->tokenType = TYPE_HIGHER_EQUAL;
                    state = MOVE_END;
                }
                else{
                    write = true;
                    token->tokenType = TYPE_HIGHER;
                    state = MOVE_END;
                    continue;
                }
            case MOVE_EXPECT_EQUAL_NEG:
                if(c == '='){
                    counter++;
                    write = true;
                }
                else{
                    exit(ERROR_LEXICAL);
                }
                if(counter == 2){
                    token->tokenType = TYPE_COMPARE_STR_NEG;
                    state = MOVE_END;
                }
                break;
            case MOVE_EXPECT_EQUAL:
                if(c != '=' && counter == 0){
                    write = true;
                    token->tokenType = TYPE_EQUAL;
                    state = MOVE_END;
                    continue;
                }
                else if(c == '=' && counter == 0){
                    counter++;
                    write = true;
                }
                else if(c == '=' && counter == 1){
                    write = true;
                    token->tokenType = TYPE_COMPARE_STR;
                    state = MOVE_END;
                }
                else{
                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);                    
                }
                break;

            case MOVE_EXPECT_COMMENT:
                if(c != '/' && c != '*' && counter == 0){
                    token->tokenType = TYPE_DIVISION;
                    write = true;
                    state = MOVE_END;
                    continue;
                }
                if(counter == 1 && c != '\n'){
                    break;
                }
                else if(counter == 1 && c == '\n'){
                    token->tokenString[0] = '\0';
                    state = MOVE_START;
                    counter = 0;
                    break;
                }
                // je to komentář
                else if(c == '/'){
                    counter = 1;
                }
                else if(c == '*'){
                    token->tokenString[0] = '\0';
                    state = MOVE_NESTED_COMMENT;
                    break;
                }
                else{
                    token->tokenType = TYPE_DIVISION;
                    state = MOVE_END;
                    continue;
                }

            break;
            
            case MOVE_NESTED_COMMENT:
                if(c == '*')
                    counter = 1;
                else if(c == '/' && counter == 1){
                    state = MOVE_START;
                    counter = 0;
                    break;
                }
                else if(c == EOF){
                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }
                else
                    counter = 0;
                
            break;

            case MOVE_STRING_CONTENT:
                // escape sekvence
                if(c == '\\'){
                    sequence[counter] = c;
                    state = MOVE_ESCAPE_SEQ;
                    break;
                }
                if(c == '"'){
                    write = true;
                    token->tokenType = TYPE_STRING_VALUE;
                    state = MOVE_END;

                }
                else if(c > 31 && c != '$'){
                    write = true;
                    break;
                }
                else if(!isspace(c)){
                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }

            break;

            case MOVE_ESCAPE_SEQ:
                if(c == '"' || c == '\\' || c == '$'){
                    write = true;
                    token->tokenString[tokenStringSizeCurrent] = '\0';
                    tokenStringSizeCurrent--;
                    state = MOVE_STRING_CONTENT;
                    break;
                }
                if(c == 'n'){
                    c = '\n';
                    token->tokenString[tokenStringSizeCurrent] = '\0';
                    tokenStringSizeCurrent--;
                    write = true;
                    state = MOVE_STRING_CONTENT;
                    break;
                }
                if(c == 't'){
                    c = '\t';
                    token->tokenString[tokenStringSizeCurrent] = '\0';
                    tokenStringSizeCurrent--;
                    write = true;
                    state = MOVE_STRING_CONTENT;
                    break;
                }
                // 2 mistne hex cislo po x
                if(c == 'x'){
                    sequence[counter+1] = c;
                    state = MOVE_ESCAPE_HEX;
                    break;
                }
                // 0-3 (octanova)/ale je od-do 001/377
                else if(c >= 48 && c <= 51){
                    sequence[counter+1] = c;
                    state = MOVE_ESCAPE_OCT;
                }
                else{
                    write = true;
                    state = MOVE_STRING_CONTENT;
                    continue;
                }
            break;

            case MOVE_ESCAPE_OCT:
                if(counter == 0 && (c >= 48 && c <= 55)){
                    sequence[counter+1] = c;
                    counter++;
                    break;
                }
                else if(counter == 1 && (c >= 49 && c <= 55)){
                    sequence[counter+1] = c;
                    counter++;
                    char escape = (int)strtol(&sequence[1], NULL, 8);
                    swap[0] = escape;
                    swap[1] = '\0';
                    strcat(token->tokenString,swap);
                    tokenStringSizeCurrent++;
                    state = MOVE_STRING_CONTENT;
                    break;
                }
                else{
                    sequence[counter+1] = c;
                    if(tokenStringSizeMax <= (tokenStringSizeCurrent + strlen(sequence)))
                        token->tokenString = realloc(token->tokenString, (tokenStringSizeMax=tokenStringSizeMax*2));
                    strcat(token->tokenString, sequence);
                    tokenStringSizeCurrent += strlen(sequence);
                    state = MOVE_STRING_CONTENT;
                }
            break;

            case MOVE_ESCAPE_HEX:
                // A-F, a-f, 0-9
                if(((c >= 65 && c <= 70) || (c >= 97 && c <= 102) || (c >= 48 && c <= 57)) && counter == 0){
                    sequence[counter+2] = c;
                    counter++;
                }
                //1-9, A-F, a-f,
                else if(counter == 1 && ((c >= 49 && c <= 57) || (c >= 65 && c <= 70) || (c >= 97 && c <= 102))){
                    sequence[counter+2] = c;
                    counter++;
                    char escape = (int)strtol(&sequence[2], NULL, 16);
                    swap[0] = escape;
                    swap[1] = '\0';
                    strcat(token->tokenString,swap);
                    tokenStringSizeCurrent++;
                    state = MOVE_STRING_CONTENT;
                }
                else{
                    sequence[counter+2] = c;
                    if(tokenStringSizeMax <= (tokenStringSizeCurrent + strlen(sequence)))
                        token->tokenString = realloc(token->tokenString, (tokenStringSizeMax=tokenStringSizeMax*2));
                    strcat(token->tokenString, sequence);
                    tokenStringSizeCurrent += strlen(sequence);
                    state = MOVE_STRING_CONTENT;
                }
            break;
            case MOVE_ID:
                //a-z, A-Z
                if((c >= 65 && c <= 90) || (c >= 97 && c <=122) || c=='_'){
                    write = true;
                    isDollar = true;
                    token->tokenType=TYPE_ID_DOLLAR;
                    state = MOVE_KEYWORD;
                }
                else{
                    fprintf(stderr, "ERROR LEXICAL \n");
                    exit(ERROR_LEXICAL);
                }
            break;

            case MOVE_LITERAL:
                //načítá první část (celé číslo)
                if(c >= 48 && c <= 57){
                    write = true;
                    break;
                }
                if(c == 'e' || c == 'E'){
                    write = true;
                    state = MOVE_EXPONENT;
                }
                else if(c == '.'){
                    write = true;
                    state = MOVE_FLOAT;
                }
                else{
                    token->tokenType = TYPE_INT_VALUE;
                    state = MOVE_END;
                    continue;
                }        
            break;
            //načítá část s . 
            case MOVE_FLOAT:
                if (c >= 48 && c <= 57){
                    write = true;
                    break;
                }
                else if(c == 'e' || c == 'E'){
                    write = true;
                    state = MOVE_EXPONENT;
                }
                else{
                    token->tokenType = TYPE_FLOAT_VALUE;
                    state = MOVE_END; 
                    continue;                   
                }
            break;
            // načítá část s exponentem
            case MOVE_EXPONENT:
                // Je ve znaku +-09 a zároveň se jedná o první znak ?
                if(counter == 0 && (c == '+' || c == '-' || (c >= 48 && c <= 57))){
                    write = true;
                    counter++;
                }
                else if(counter == 0){ 
                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }
                else if(counter != 0 && c >= 48 && c <= 57){
                    write = true;
                }
                else{
                    token->tokenType = TYPE_FLOAT_VALUE;
                    state = MOVE_END;
                    continue;
                }
            break;
            case MOVE_KEYWORD:
                
                if((c >= 65 && c <= 90) || (c >= 97 && c <=122) || c == '_' || (c >= 48 && c <= 57)){
                        write = true;
                        break;
                }  
                else {
                    token->tokenType = (isDollar)?TYPE_ID_DOLLAR:TYPE_ID;
                    //Zkontroluje jestli se nejedná o klíčové slovo
                    if(strcmp(token->tokenString,"else")==0) 
                        token->tokenType=TYPE_ELSE;
                    else if(strcmp(token->tokenString,"float")==0 || strcmp(token->tokenString,"?float")==0)
                        token->tokenType=TYPE_FLOAT;
                    else if (strcmp(token->tokenString,"function")==0)
                        token->tokenType=TYPE_FUNCTION;
                    else if(strcmp(token->tokenString,"if")==0)
                        token->tokenType=TYPE_IF;
                    else if(strcmp(token->tokenString,"int")==0 || strcmp(token->tokenString,"?int")==0)
                        token->tokenType=TYPE_INT;
                    else if(strcmp(token->tokenString,"null")==0)
                        token->tokenType=TYPE_NULL;
                    else if(strcmp(token->tokenString,"if")==0)
                        token->tokenType=TYPE_IF;
                    else if(strcmp(token->tokenString,"return")==0)
                        token->tokenType=TYPE_RETURN;
                    else if(strcmp(token->tokenString,"string")==0 || strcmp(token->tokenString,"?string")==0)
                        token->tokenType=TYPE_STRING;
                    else if(strcmp(token->tokenString,"void")==0)
                        token->tokenType=TYPE_VOID;
                    else if(strcmp(token->tokenString,"while")==0)
                        token->tokenType=TYPE_WHILE;

                    if(token->tokenType != TYPE_INT && token->tokenType != TYPE_STRING && token->tokenType != TYPE_FLOAT && token->tokenString[0] == '?'){
                        fprintf(stderr,"ERROR LEXICAL\n");
                        exit(ERROR_LEXICAL);
                    }
                    state = MOVE_END;
                }  


            case MOVE_END:
                isDollar = false;
                tokenStringSizeCurrent = 0;
                tokenStringSizeMax = 16;
                counter = 0;
                token->nextToken = malloc(sizeof(struct token)); //Alokuje další token
                if(token->nextToken == nullptr)
                    exit(ERROR_INTERNAL);
                token->nextToken->prevToken = token;
                token = token->nextToken;
                if(header == false || ((eop == true) && (token->tokenType != TYPE_EOP) && (c != EOF))){
                    fprintf(stderr,"ERROR LEXICAL\n");
                    exit(ERROR_LEXICAL);
                }
                token->tokenString = calloc(tokenStringSizeMax,sizeof(char));
                token->nextToken = nullptr;
                token->tokenType = TYPE_UNDEF;
                state = MOVE_START;
                continue;
            default:
                state = MOVE_START;
                
        }
        if(write){
            //předej znak do pole
            swap[0] = c;
            swap[1] = '\0';
            strcat(token->tokenString, swap);
            tokenStringSizeCurrent++;
        }
        if(c == EOF){
            break;
        }
        c = fgetc(stdin); 
    }
    if(c == EOF){
        token->tokenType = TYPE_EOF;
        }
    return head;
}   

/**
 * @brief Vrací následující token
 * 
 * @return tokenList (Požadovaný prvek v seznamu)
 */
tokenList getToken(){
    static tokenList head;
    static bool fsmCalled = false;

    if(!fsmCalled){
        head = FSM();
        fsmCalled = true;
    }
    if(head != nullptr){
        if(head->tokenType == TYPE_EOF)
            return head;
        head = head->nextToken;
        if(head != nullptr)
            return head->prevToken;
        return nullptr;
    }
    else
        return nullptr;
}
// debugovací funkce
void printTokens(tokenList token){
    printf("ID:%d tokenString:'%s'\n",token->tokenType,token->tokenString);
}