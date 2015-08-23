
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "scan.h"
#include "parse.h"

int main(int argc, char *argv[]){
    openSourceFile(argv[1]);
    strcpy(sourceFileName, argv[1]);
    
    parse();
    
    fclose(source);
    
    return 0;
}