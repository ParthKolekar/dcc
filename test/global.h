
#ifndef GLOBAL_H_
#define GLOBAL_H_

#define MAX_STR_LEN 500


typedef enum{
    BOOLEANLITERAL, INTLITERAL, CHARLITERAL, STRINGLITERAL, IDENTIFIER, KEYWORD, OPERATOR, ERROR, DELIMITER, COMMENT_TOK, OTHER 
} TokenType;

extern char *tokenTypeStr[];


extern char keyWordsStr[][50];


FILE *source;
extern int sourceEOF;
extern char *sourceFolder;
char sourceFileName[MAX_STR_LEN];

extern int lineNo, linePos;

extern char tokenString[MAX_STR_LEN];

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

#define MAXCHILDREN 3

typedef struct treeNode{
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;
    int lineNo;
    char *tokenStr;
    TokenType token;
} TreeNode;

#endif

