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
}

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
%token RETURN;
%token TRUE
%token VOID
%token COMMENT
%token IDENTIFIER
%token CHAR
%token STRING
%token DECIMAL
%token HEXADECIMAL
%token INIT
%token ENTE

%%

program : INIT FIELD_DECL_LIST METHOD_DECLARATION_LIST ENTE

FIELD_DECL_LIST : BREAK
	;

METHOD_DECLARATION_LIST : BOOLEAN
	;

%%

void yyerror (const char *s) { 
    std::cerr << "Parse error on line " << line_num << " ! Message : " << s << std::endl;
    exit(-1);
}

