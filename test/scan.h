
#ifndef SCAN_H_
#define SCAN_H_


typedef enum{
    START, END, INT, INT_DEC, INT_EXPECT_HEX, INT_HEX, CHAR, CHAR_EXPECT_QUOTE, ESCAPE_CHAR, STRING, STRING_ESCAPE_CHAR, ID, OP, COND_OP, COMMENT, DELIMIT
} StateType;

extern char *stateTypeStr[];


FILE *outputFile;
extern char *outputFolder;
extern char *referenceOutputFolder;

//lineBuffer stores a line from the input file
extern char lineBuffer[MAX_STR_LEN];

#endif