
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "global.h"
#include "scan.h"

char *stateTypeStr[] = {"START", "END", "INT", "INT_DEC", "INT_EXPECT_HEX", "INT_HEX", "CHAR", "CHAR_EXPECT_QUOTE", "ESCAPE_CHAR", "STRING", "STRING_ESCAPE_CHAR", "ID", "OP", "COND_OP", "COMMENT", "DELIMIT"};

char *tokenTypeStr[] = {"BOOLEANLITERAL", "INTLITERAL", "CHARLITERAL", "STRINGLITERAL", "IDENTIFIER", "KEYWORD", "OPERATOR", "ERROR", "DELIMITER", "COMMENT_TOK", "OTHER"};

char keyWordsStr[][50] = {"boolean", "break", "callout", "class", "continue", "else", "for", "if", "int", "return", "void"};

int sourceEOF = 0;

char *sourceFolder = "scanner_input/";
char *outputFolder = "scanner_output/";
char *referenceOutputFolder = "scanner_expected_output/";

int lineNo = 0, linePos = 0;

char tokenString[MAX_STR_LEN] = {'\0'};

//lineBuffer stores a line from the input file
char lineBuffer[MAX_STR_LEN] = {'\0'};



void ungetNextChar(){
    linePos--;
}    

char getNextChar(){
    char ch;
    if(lineBuffer[linePos] == '\0'){
        if(fgets(lineBuffer, MAX_STR_LEN, source) != NULL){
	    linePos = 0;
	    lineNo++;
	}
    }
    if(lineBuffer[linePos] != '\0'){
	ch = lineBuffer[linePos];
	linePos++;
    }
    else{
	sourceEOF = 1;
	ch = EOF;
    }
    return ch;
}

   

void openSourceFile(char *fileName){
    char sourceFile[MAX_STR_LEN];
    //sprintf(sourceFile, "%s%s", sourceFolder, fileName);
    sprintf(sourceFile, "%s", fileName);
    source = fopen(sourceFile, "r");
    if(source == NULL){
	printf("Input file could not be opened\n");
	exit(0);
    }   
}    

int isKeyword(char *str){
    int i;
    for(i = 0; i < sizeof(keyWordsStr) / sizeof(keyWordsStr[0]); i++){
	if(strcmp(keyWordsStr[i], str) == 0)
	    return 1;
    }
    return 0;
}    

int isBoolean(char *str){
    return(strcmp(str, "true") == 0 || strcmp(str, "false") == 0);
}


int isWhiteSpace(char ch){
    if(ch == ' ' || ch == '\t' || ch == '\n' ) return 1;
    return 0;
}

void handleTokenError(char ch, int expectedCharExists, char expectedChar){
    if(ch != EOF){
	char errorMsg[MAX_STR_LEN] = {'\0'};
	if(expectedCharExists){
	    if(ch == '\n')
		sprintf(errorMsg, "%s line %d:%d: expecting \'%c\', found \'\\n\'\n", sourceFileName, lineNo, linePos, expectedChar);
	    else
		sprintf(errorMsg, "%s line %d:%d: expecting \'%c\', found \'%c\'\n", sourceFileName, lineNo, linePos, expectedChar, ch);
	}
	else{
	    if(isWhiteSpace(ch) || ch == '\f')
		sprintf(errorMsg, "%s line %d:%d: unexpected char: 0x%X\n", sourceFileName, lineNo, linePos, ch);
	    else
		sprintf(errorMsg, "%s line %d:%d: unexpected char: \'%c\'\n", sourceFileName, lineNo, linePos, ch);
	}
	puts(errorMsg);
	//fputs(errorMsg, outputFile);
    }
}    

//returns the token type of next token and corresponding string is stored in tokenString
TokenType getToken(){
    TokenType currentToken = OTHER;
    char ch;
    int saveCharInToken = 1;
    StateType state = START;
    int tokenStringIndex = 0;
    bzero(tokenString, MAX_STR_LEN);
    
    while(state != END){
	ch = getNextChar();
	
	switch(state){
	    case START :
		if(isdigit(ch)){
		    if(ch == '0') state = INT;
		    else	  state = INT_DEC;
		}    
		else if(ch == '\'') 
		    state = CHAR;
		else if(ch == '\"') 
		    state = STRING;
		else if(ch == '_' || isalpha(ch)) 
		    state = ID;
		else if(ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%') 
		    state = OP;
		else if(ch == '<' || ch == '>' || ch == '=' || ch == '!') 
		    state = OP;
		else if(ch == '&' || ch == '|')
		    state = COND_OP;
		else if(ch == '[' || ch == ']' || ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ',' || ch == ';')
		    state = DELIMIT;
		else if(isWhiteSpace(ch))
		    continue;
		else{
		    handleTokenError(ch, 0, 0);
		    currentToken = OTHER;
		    state = END;
		    saveCharInToken = 0;
		}
		break;
	    case INT :
		if(ch == 'x')		state = INT_EXPECT_HEX;
		else if(isdigit(ch))	state = INT_DEC;
		else {
		    state = END;
		    currentToken = INTLITERAL;
		    saveCharInToken = 0;
		    ungetNextChar();
		}
		break;
	    case INT_DEC :
		if(!isdigit(ch)) {
		    state = END;
		    currentToken = INTLITERAL;
		    saveCharInToken = 0;
		    ungetNextChar();
		}
		break;
	    case INT_EXPECT_HEX :
		if(isxdigit(ch)) state = INT_HEX;
		else {
		    handleTokenError(ch, 0, 0);
		    currentToken = ERROR;
		}
		break;
	    case INT_HEX :
		if(!isxdigit(ch)) {
		    state = END;
		    currentToken = INTLITERAL;
		    saveCharInToken = 0;
		    ungetNextChar();
		}
		break;
	    case CHAR :
		if(ch >= 32 && ch <= 126 && ch != '\'' && ch != '\"' && ch != '\\') state = CHAR_EXPECT_QUOTE;
		else if(ch == '\\') state = ESCAPE_CHAR;
		else {
		    handleTokenError(ch, 0, 0);
		    state = CHAR_EXPECT_QUOTE;
		    currentToken = ERROR;
		}
		break;
	    case CHAR_EXPECT_QUOTE :
		if(ch == '\'') {
		    state = END;
		    if(currentToken != ERROR) currentToken = CHARLITERAL;
		    saveCharInToken = 1;
		}
		else {
		    handleTokenError(ch, 1, '\'');
		    currentToken = ERROR;
		}
		break;
	    case ESCAPE_CHAR :
		if(ch == 'n' || ch == 't' || ch == '\'' || ch == '\"' || ch == '\\') state = CHAR_EXPECT_QUOTE;
		else {
		    handleTokenError(ch, 0, 0);
		    state = CHAR_EXPECT_QUOTE;
		    currentToken = ERROR;
		}
		break;
	    case STRING :
		if(ch == '\\') state = STRING_ESCAPE_CHAR;
		else if(ch == '\"'){
		    state = END;
		    if(currentToken != ERROR) currentToken = STRINGLITERAL;
		    saveCharInToken = 1;
		}
		else if(!(ch >= 32 && ch <= 126 && ch != '\'' && ch != '\\')) {
		    handleTokenError(ch, 1, '\"');
		    currentToken = ERROR;
		    state = END;
		}
		break;
	    case STRING_ESCAPE_CHAR :
		if(ch == 'n' || ch == 't' || ch == '\'' || ch == '\"' || ch == '\\') state = STRING;
		else {
		    handleTokenError(ch, 0, 0);
		    state = STRING;
		    currentToken = ERROR;
		}
		break;
	    case ID :
		if(!isalnum(ch) && ch != '_') {
		    state = END;
		    currentToken = IDENTIFIER;
		    saveCharInToken = 0;
		    ungetNextChar();
		}
		break;
	    case OP :
		if(tokenString[0] == '/' && ch == '/') {
		    state = COMMENT;
		    currentToken = COMMENT_TOK;
		}
		else if(ch == '=') {
		    state = END;
		    currentToken = OPERATOR;
		    saveCharInToken = 1;
		}
		else{
		    state = END;
		    currentToken = OPERATOR;
		    saveCharInToken = 0;
		    ungetNextChar();
		}
		break;
	    case COND_OP :
		if(ch == tokenString[0]) {
		    state = END;
		    currentToken = OPERATOR;
		    saveCharInToken = 1;
		}
		else {
		    handleTokenError(ch, 1, tokenString[0]);
		    currentToken = ERROR;
		}
		break;
	    case COMMENT :
		if(ch == '\n') {
		    state = END;
		    saveCharInToken = 0;
		    ungetNextChar();
		}
		break;
	    case DELIMIT :
		state = END;
		currentToken = DELIMITER;
		saveCharInToken = 0;
		ungetNextChar();
	}
	
	if(ch == EOF) break;
	if(saveCharInToken && currentToken != ERROR) tokenString[tokenStringIndex++] = ch;
	//printf("ch = %c\tsave = %d\tstate = %s\ttoken = %s\ttokenString = %s\n", ch, saveCharInToken, stateTypeStr[state], tokenTypeStr[currentToken], tokenString);
    }
    tokenString[tokenStringIndex] = '\0';
    if(currentToken == COMMENT_TOK)
	return getToken();
    if(currentToken == IDENTIFIER && isBoolean(tokenString))
	currentToken = BOOLEANLITERAL;
    else if(currentToken == IDENTIFIER && isKeyword(tokenString))
	currentToken = KEYWORD;
    return currentToken;
}
