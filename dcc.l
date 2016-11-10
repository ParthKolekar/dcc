%{
    #include <cstdlib>
    #include <iostream>

    #include "AST.h"
    #include "dcc.tab.h"
    #include "Visitor.h"

    #define YY_DECL extern "C" int yylex()

    char * translateString(char *, int);
%}

%option nounput
%option noyywrap
%option yylineno

ALPHA [a-zA-Z]
CHAR ([\40-\176]{-}['"\\])|\\n|\\t|\\\\|\\\"|\\\'
DIGIT [0-9]
HEXADECIMAL [0-9a-fA-F]
UNDERSCOREDALPHA [a-zA-Z_]

%%

\/\/.*\n {
}

\, {
    return COMMA;
}

\; {
    return SEMICOLON;
}

\( {
    return OPEN_PARANTHESIS;
}

\) {
    return CLOSE_PARANTHESIS;
}

\[ {
    return OPEN_SQUAREBRACKET;
}

\] {
    return CLOSE_SQUAREBRACKET;
}

\{  {
    return OPEN_CURLYBRACE;
}

\}  {
    return CLOSE_CURLYBRACE;
}

\+\= {
    return PLUSEQUAL;
}

\+ {
    return PLUS;
}

\-\= {
    return MINUSEQUAL;
}

\- {
    return MINUS;
}

\* {
    return MULTIPLY;
}

\/ {
    return DIVIDE;
}

\% {
    return MODULO;
}

\!\= {
    return NOTEQUAL;
}

\! {
    return NOT;
}

\<\= {
    return LESSEQUAL;
}

\< {
    return LESSTHAN;
}

\>\= {
    return GREATEREQUAL;
}

\> {
    return GREATERTHAN;
}

\=\= {
    return EQUALEQUAL;
}

\= {
    return EQUAL;
}

\|\| {
    return OR;
}

\&\& {
    return AND;
}

boolean {
    return BOOLEAN;
}

break {
    return BREAK;
}

callout {
    return CALLOUT;
}

class {
    return CLASS;
}

continue {
    return CONTINUE;
}

else {
    return ELSE;
}

false {
    return FALSE;
}

for {
    return FOR;
}

if {
    return IF;
}

int {
    return INT;
}

return {
    return RETURN;
}

true {
    return TRUE;
}

void {
    return VOID;
}

{UNDERSCOREDALPHA}({UNDERSCOREDALPHA}|{DIGIT})* {
    yylval.sval = strdup(yytext);
    return IDENTIFIER;
}

{DIGIT}+ {
    yylval.ival = atoi(yytext);
    return INT_VALUE;
}

0x{HEXADECIMAL}+ {
    yylval.ival = atoi(yytext);
    return INT_VALUE;
}

\"{CHAR}*\" {
    yylval.sval = translateString(yytext + 1, strlen(yytext) - 2);
    return STRING_VALUE;
}

\'{CHAR}\' {
    yylval.cval = yytext[1];
    return CHAR_VALUE;
}

[\t\ \n] {
}

. {
    std::cerr << "UNKNOWN SOMETHING HAS HAPPENED" << yytext;
    exit(-1);
}

%%

char * translateString(char * string, int size) {
    char * ch = string;
    char * newString = (char *) malloc(size + 1);
    char * temp = newString;
    int i;
    for (i = 0; i < size; i++) {
        switch (*ch) {
            case '\\':
                i++;
                ch++;
                switch (*ch) {
                    case 'a':
                        *temp = '\a';
                        break;
                    case 'b':
                        *temp = '\b';
                        break;
                    case 'f':
                        *temp = '\f';
                        break;
                    case 'n':
                        *temp = '\n';
                        break;
                    case 'r':
                        *temp = '\r';
                        break;
                    case 't':
                        *temp = '\t';
                        break;
                    case 'v':
                        *temp = '\v';
                        break;
                    case '\\':
                        *temp = '\\';
                        break;
                    case '\'':
                        *temp = '\'';
                        break;
                    case '\"':
                        *temp = '\"';
                        break;
                    case '?':
                        *temp = '\?';
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        // TODO: Fill
                        break;
                    case 'x':
                        // TODO: Fill
                        break;
                    default:
                        break;
                }
                break;
            default: 
                *temp = *ch;
                break;
        }
        ch++;
        temp++;
    }
    *temp = '\0';
    return newString;
}
