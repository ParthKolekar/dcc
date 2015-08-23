
#ifndef PARSE_H_
#define PARSE_H_

struct sourceReadState{
    int lineNo;
    int linePos;
    char lineBuffer[MAX_STR_LEN];
    char tokenString[MAX_STR_LEN];
    TokenType nextTok;
    fpos_t pos;
}readState;

typedef struct sourceReadState srcRdState;

TokenType next;

#endif