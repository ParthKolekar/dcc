
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "scan.h"
#include "parse.h"

#define STMT_PRODUCTIONS 8

int expr();

TreeNode *newNode(char *tokenStr, TokenType token){
    TreeNode *t = malloc(sizeof(TreeNode));
    if(t == NULL)
	printf("Out of memory error at line %d\n",lineNo);
    else{
	int i;
	for(i = 0; i < MAXCHILDREN; i++) t->child = NULL;
	t->sibling = NULL;
	t->lineNo = lineNo;
	t->tokenStr = tokenStr;
	t->token = token;
    }
    return t;
}


TreeNode *currentNode;

srcRdState createRestorePoint(){
    readState.lineNo = lineNo;
    readState.linePos = linePos;
    strcpy(readState.lineBuffer, lineBuffer);
    strcpy(readState.tokenString, tokenString);
    readState.nextTok = next;
    fgetpos(source, &readState.pos);
    return readState;
    //printf("saving -> lineNo : %d\tlinePos : %d\tbuffer : %s\n", lineNo, linePos, lineBuffer);
}

void backtrack(srcRdState rdState){
    //printf("backtracking from -> lineNo : %d\tlinePos : %d\ttokenString : %s\tbuffer : %s\n", lineNo, linePos, tokenString, lineBuffer);
    lineNo = rdState.lineNo;
    linePos = rdState.linePos;
    strcpy(lineBuffer, rdState.lineBuffer);
    strcpy(tokenString, rdState.tokenString);
    next = rdState.nextTok;
    fsetpos(source, &rdState.pos);
    //printf("backtracking to -> lineNo : %d\tlinePos : %d\ttokenString : %s\tbuffer : %s\n", lineNo, linePos, tokenString, lineBuffer);
}

int match(TokenType token, char *tokenStr){
    int matched = 0; 
    /*if(token == IDENTIFIER)
        printf("expected token : %d\tcurrent token : %d\texpected = %s\t\tfound = %s\n", token, next, "identifier", tokenString);
    else
	printf("expected token : %d\tcurrent token : %d\texpected = %s\t\tfound = %s\n", token, next, tokenStr, tokenString);*/
    if(next == token) matched = 1;
    if(matched && tokenStr != NULL)
        matched = (strcmp(tokenString, tokenStr) == 0);
    
    if(matched)
        next = getToken();
    return matched;
}

int expect(TokenType token, char *tokenStr, char *expectedType){
    int matched = match(token, tokenStr);
    if(!matched) {
	if(tokenStr == NULL)
	    printf("\nError at line : %d:%d\texpected : %s\tfound : %s\n\n", lineNo, linePos, expectedType, tokenString);
	else
	    printf("\nError at line : %d:%d\texpected : %s\tfound : %s\n\n", lineNo, linePos, tokenStr, tokenString);
	//exit(EXIT_FAILURE);
    }
    return matched;
}

int errorIfNotMatch(int (*fp)(), char *expected){
    srcRdState rdState = createRestorePoint();
    if(!fp()){
	backtrack(rdState);
	printf("\nError at line : %d:%d\texpected : %s\tfound : %s\n\n", lineNo, linePos, expected, tokenString);
	//exit(EXIT_FAILURE);
    }
    return 1;
}

int matchZeroOrMoreTimes(int (*fptr)()){
    while(1){
	srcRdState rdState = createRestorePoint();
	if(!fptr()){
	    backtrack(rdState);
	    break;
	}
    }
    return 1;
}

int matchAtleastOnce(int (*fptr)()){
    int matched = fptr();
    while(matched && match(DELIMITER, ",")){
	matched = fptr();
    }
    return matched;
}

int var(){
    int matched = expect(IDENTIFIER, NULL, "identifier");
    if(matched && match(DELIMITER, "("))
	matched = 0;
    if(matched && match(DELIMITER, "["))
	    matched = expect(INTLITERAL, NULL, "intliteral") && expect(DELIMITER, "]", NULL);
    return matched;
}

int type(){
    return match(KEYWORD, "int") || match(KEYWORD, "boolean");
}    
    
int fieldDecl(){
    return type() && matchAtleastOnce(&var) && expect(DELIMITER, ";", NULL);
}

int varDecl(){
    int matched = type() && expect(IDENTIFIER, NULL, "identifier");
    while(matched && match(DELIMITER, ",")){
	matched = expect(IDENTIFIER, NULL, "identifier");
    }
    return matched && expect(DELIMITER, ";", NULL);
}

int expr1(){
    srcRdState rdState = createRestorePoint();
    int matched = location();
    if(!matched) {
	backtrack(rdState);
        matched = methodCall();
    }
    if(!matched) {
	backtrack(rdState);
        matched = match(INTLITERAL, NULL) || match(CHARLITERAL, NULL) || match(BOOLEANLITERAL, NULL);
    }
    if(!matched)
	matched = (match(OPERATOR, "-") || match(OPERATOR, "!")) && errorIfNotMatch(&expr, "expression");
    if(!matched){
	backtrack(rdState);	
        matched = match(DELIMITER, "(") && errorIfNotMatch(&expr, "expression") && expect(DELIMITER, ")", NULL);
    }
    return matched;
}

int factor(){
    int matched = expr1();
    while(matched && (match(OPERATOR, "*") || match(OPERATOR, "/") || match(OPERATOR, "%"))){
	matched = expr1();
    }
    return matched;
    return 0;
}

int term(){
    int matched = factor();
    while(matched && (match(OPERATOR, "+") || match(OPERATOR, "-")))
	matched = factor();
    return matched;
}

int rel(){
    int matched = term();
    while(matched && (match(OPERATOR, "<") || match(OPERATOR, "<=") || match(OPERATOR, ">") || match(OPERATOR, ">=")))
	matched = term();
    return matched;
}

int eq(){
    int matched = rel();
    while(matched && (match(OPERATOR, "==") || match(OPERATOR, "!=")))
	matched = rel();
    return matched;
}

int bool(){
    int matched = eq();
    while(matched && match(OPERATOR, "&&"))
	matched = eq();
    return matched;
}

int expr(){
    int matched = bool();
    while(matched && match(OPERATOR, "||"))
	matched = bool();
    return matched;
}

int location(){
    int matched = match(IDENTIFIER, NULL);
    if(matched && match(DELIMITER, "(")) return 0;
    if(matched && match(DELIMITER, "["))
	matched = errorIfNotMatch(&expr, "expression") && expect(DELIMITER, "]", NULL);
    return matched;
}

int calloutArg(){
    int matched = match(STRINGLITERAL, NULL) || expr();
    return matched;
}

int mthdCal2(){
    int matched = match(KEYWORD, "callout")
               && expect(DELIMITER, "(", NULL)
	       && expect(STRINGLITERAL, NULL, "stringliteral");
    while(matched && match(DELIMITER, ",")){
	matched = calloutArg();
    }
    return matched && expect(DELIMITER, ")", NULL);
}

int mthdCal1(){
    int matched = match(IDENTIFIER, NULL) && match(DELIMITER, "("); 
    if(matched && !match(DELIMITER, ")")){
	matched = errorIfNotMatch(&expr, "expression");
	while(matched && match(DELIMITER, ",")){
	    matched = errorIfNotMatch(&expr, "expression");
	}
	matched = expect(DELIMITER, ")", NULL);
    }
    return matched;
}

int methodCall(){
    int matched;
    srcRdState rdState = createRestorePoint();
    if( mthdCal1() ) matched = 1; 
    else {
	backtrack(rdState);
	matched = mthdCal2();
    }
    return matched;
}

int assignOp(){
    return match(OPERATOR, "=")
        || match(OPERATOR, "+=")
	|| match(OPERATOR, "-=");
}

int stmtAssign(){
    return location() && assignOp() && expr() && expect(DELIMITER, ";", NULL);
}

int stmtMethodCall(){
    return methodCall() && expect(DELIMITER, ";", NULL);
}

int stmtIf(){
    int matched = match(KEYWORD, "if")
               && expect(DELIMITER, "(", NULL)
	       && errorIfNotMatch(&expr, "expression")
               && expect(DELIMITER, ")", NULL)
	       && block();
    if(matched && match(KEYWORD, "else"))
	matched = block();
    return matched;       
}

int stmtFor(){
    int matched = match(KEYWORD, "for")
               && expect(IDENTIFIER, NULL, "identifier")
	       && expect(OPERATOR, "=", NULL)
	       && errorIfNotMatch(&expr, "expression")
	       && expect(DELIMITER, ",", NULL)
	       && errorIfNotMatch(&expr, "expression")
	       && block();
    return matched;
}

int stmtReturn(){
    int matched = match(KEYWORD, "return");
    if(matched && !match(DELIMITER, ";")){
	matched = errorIfNotMatch(&expr, "expression")
	       && expect(DELIMITER, ";", NULL);
    }
    return matched;
}

int stmtBreak(){
    return match(KEYWORD, "break") && expect(DELIMITER, ";", NULL);
}

int stmtContinue(){
    return match(KEYWORD, "continue") && expect(DELIMITER, ";", NULL);
}

int stmtBlock(){
    return block();
    
}

int statement(){
    int matched = 0;
    int (*fpArr[STMT_PRODUCTIONS])() = {stmtAssign, stmtMethodCall, stmtIf, stmtFor, stmtReturn, stmtBreak, stmtContinue, stmtBlock};
    int i;
    for(i = 0; i < STMT_PRODUCTIONS; i++){
	srcRdState rdState = createRestorePoint();
	if( fpArr[i]() ) {
	    matched = 1; 
	    break;
	}
	else backtrack(rdState);
    }
    return matched;
}

int block(){
    int matched = match(DELIMITER, "{") 
               && matchZeroOrMoreTimes(&varDecl)
	       && matchZeroOrMoreTimes(&statement)
	       && expect(DELIMITER, "}", NULL);
    return matched;
}

int formalParams(){
    return type() && expect(IDENTIFIER, NULL, "identifier");    
}

int params(){
    int matched;
    if(strcmp(tokenString, ")") == 0) return 1;
    matched = matchAtleastOnce(&formalParams);
    return matched;
}

int methodDecl(){
    int matched = (match(KEYWORD, "void") || type())
                && expect(IDENTIFIER, NULL, "identifier") 
		&& expect(DELIMITER, "(", NULL);
    if(matched) params(); 
    matched = matched && expect(DELIMITER, ")", NULL) 
		&& block();
    return matched;
}


int program(){
    expect(KEYWORD, "class", NULL);
    expect(IDENTIFIER, "Program", NULL);
    expect(DELIMITER, "{", NULL);
    matchZeroOrMoreTimes(&fieldDecl);
    matchZeroOrMoreTimes(&methodDecl); 
    expect(DELIMITER, "}", NULL);
}    

int parse(){
    TreeNode *tree;
    next = getToken();
    program();
    return 0;
}
