%{
    #include "dcc.tab.h"
    #define YY_DECL extern "C" int yylex()
    int line_num = 0;
%}

%option noyywrap

%%

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

\/\/[.*]\n {
	return COMMENT;
}

[_a-zA-Z][a-zA-Z0-9]* {
	return IDENTIFIER;
}

\'[a-zA-Z]\' {
	return CHAR;
	// TODO: Fix bad logic.
}

\"[a-zA-Z]\" {
	return STRING;
	// TODO: Fix bad logic.
}

[0-9]* {
	return DECIMAL;
}

0x[0-9a-fA-F] {
	return HEXADECIMAL;
}

"class Program {" {
	return INIT;
}

\} {
	return ENTE;
}

%%
