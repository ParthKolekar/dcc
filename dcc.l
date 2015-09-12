%{
    #include "dcc.tab.h"
    #define YY_DECL extern "C" int yylex()
    int line_num = 0;
    #include <iostream>
    #include <cstdlib>
%}

%option noyywrap

DIGIT [0-9]
ALPHA [a-zA-Z]
UNDERSCOREDALPHA [a-zA-Z_]
HEXADECIMAL [0-9a-fA-F]
CHAR ([\40-\176]{-}['"\\])|\\n|\\t|\\\\|\\\"|\\\'

%%

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

\+\+ {
    return PLUSPLUS;
}

\+\= {
    return PLUSEQUAL;
}

\+ {
    return PLUS;
}

\-\- {
    return MINUSMINUS;
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

\! {
    return NOT;
}

\!\= {
    return NOTEQUAL;
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

\= {
    return EQUAL;
}

\=\= {
    return EQUALEQUAL;
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
    yylval.sval = strdup(yytext);
    return STRING_VALUE;
}

\'{CHAR}\' {
    yylval.cval = yytext[1];
    return CHAR_VALUE;
}

\/\/[.*]\n {
    line_num++;
}

[\t\ ] {
}

\n {
    line_num++;
}

. {
    std::cerr << "UNKNOWN SOMETHING HAS HAPPENED" << yytext;
}

%%
