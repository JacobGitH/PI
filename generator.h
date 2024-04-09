
#include "parser.h"
#include <stdio.h>

void rectifyString(char* value);
int genCondition(expr* cond);
int genInlineCall(expr* node);
int genExpression(pNode node);
int genFunctioncall(pNode node);
int genReturntype(pNode node);
int genReturn(pNode node);
int genIf(pNode node);
int genWhile(pNode node);
int genFunction(pNode node);
void genSymbol(expr* cond);
int branchTopLevel(pNode node);
int generate(pNode node);