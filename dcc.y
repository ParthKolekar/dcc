%{
    #include <iostream>

    extern "C" int yylex();
    extern "C" int yyparse();
    extern "C" FILE * yyin;
    extern int line_num;

    void yyerror(const char *s);
%}

%union {
	char *sval;
	int ival;
    char cval;
}

%token COMMA
%token SEMICOLON
%token OPEN_PARANTHESIS
%token CLOSE_PARANTHESIS
%token OPEN_SQUAREBRACKET
%token CLOSE_SQUAREBRACKET
%token OPEN_CURLYBRACE
%token CLOSE_CURLYBRACE
%token PLUSPLUS
%token PLUSEQUAL
%token PLUS
%token MINUSMINUS
%token MINUSEQUAL
%token MINUS
%token MULTIPLY
%token DIVIDE
%token MODULO
%token NOT
%token NOTEQUAL
%token LESSEQUAL
%token LESSTHAN
%token GREATEREQUAL
%token GREATERTHAN
%token EQUAL
%token EQUALEQUAL
%token OR
%token AND
%token BOOLEAN
%token BREAK
%token CALLOUT
%token CLASS
%token CONTINUE
%token ELSE
%token FALSE
%token FOR
%token IF
%token INT
%token RETURN
%token TRUE
%token VOID
%token IDENTIFIER
%token INT_VALUE
%token STRING_VALUE
%token CHAR_VALUE

%%

program : CLASS IDENTIFIER OPEN_CURLYBRACE CLOSE_CURLYBRACE 
        {
            
        }

%%

void yyerror (const char *s) { 
    std::cerr << "Parse error on line " << line_num << " ! Message : " << s << std::endl;
    exit(-1);
}

