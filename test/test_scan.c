
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "scan.h"

void diffTest(char *fileName){
    char cmd[MAX_STR_LEN];
    sprintf(cmd, "diff %s%s.out %s%s.out", referenceOutputFolder, fileName, outputFolder, fileName);
    system(cmd);
}

void createOutputFile(char *fileName){
    char filePath[MAX_STR_LEN];
    
    struct stat st = {0};
    if (stat(outputFolder, &st) == -1) mkdir(outputFolder, 0700);
    
    sprintf(filePath, "%s%s.out", outputFolder, fileName);
    
    char cmd[MAX_STR_LEN];
    sprintf(cmd, "rm -f %s", filePath);
    system(cmd);
    outputFile = fopen(filePath, "a");
    if(outputFile == NULL){
	printf("Output file could not be opened\n");
	exit(0);
    }
} 


int main(int argc, char *argv[]){
    openSourceFile(argv[1]);
    createOutputFile(argv[1]);
    strcpy(sourceFileName, argv[1]);
    
    while(!sourceEOF){
	TokenType currentToken = getToken();
	if(strcmp(tokenTypeStr[currentToken], "OTHER") != 0 && strcmp(tokenTypeStr[currentToken], "ERROR") != 0){
	    if(currentToken == KEYWORD || currentToken == OPERATOR || currentToken == DELIMITER)
		fprintf(outputFile, "%d %s\n", lineNo, tokenString);
	    else
	        fprintf(outputFile, "%d %s %s\n", lineNo, tokenTypeStr[currentToken], tokenString);
	    //printf("%d %s %s\n", lineNo, tokenTypeStr[currentToken], tokenString);
	}
    }
    
    fclose(source);
    fclose(outputFile);
    diffTest(argv[1]);
    
    return 0;
}