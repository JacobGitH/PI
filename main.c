#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "main.h"

int main(){
    pNode ast = parse();
    
    semanticCheck(ast);

    if (!generate(ast)) return 0;  // TODO: return actual error code from generation
    //freeTree(ast);
    
    return EXIT_SUCCESS;
}