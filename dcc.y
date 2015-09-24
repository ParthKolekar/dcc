%{
    #include <iostream>

    extern "C" int yylex();
    extern "C" int yyparse();
    extern "C" FILE * yyin;
    extern int yylineno;

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

%token EQUAL 
%token PLUSEQUAL 
%token MINUSEQUAL

%nonassoc LESSEQUAL LESSTHAN GREATEREQUAL GREATERTHAN

%left EQUALEQUAL NOTEQUAL
%left OR
%left AND
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO

%precedence NOT UMINUS

%token <sval> IDENTIFIER
%token <ival> INT_VALUE
%token <sval> STRING_VALUE
%token <cval> CHAR_VALUE

%%

program : CLASS IDENTIFIER OPEN_CURLYBRACE field_decl_list method_decl_list CLOSE_CURLYBRACE {}
        | CLASS IDENTIFIER OPEN_CURLYBRACE field_decl_list CLOSE_CURLYBRACE  {}
        | CLASS IDENTIFIER OPEN_CURLYBRACE method_decl_list CLOSE_CURLYBRACE {}
        | CLASS IDENTIFIER OPEN_CURLYBRACE CLOSE_CURLYBRACE {}
        ;

field_decl_list : field_decl {}
                | field_decl_list field_decl {}
                ;

field_decl : type identifier_list SEMICOLON {}
           | type identifier_array_list SEMICOLON {}
           ;

identifier_list : IDENTIFIER {}
                | identifier_list COMMA IDENTIFIER {}
                ;


identifier_array_list : identifier_array {}
                      | identifier_array_list COMMA identifier_array {}
                      ;

identifier_array : IDENTIFIER OPEN_SQUAREBRACKET INT_VALUE CLOSE_SQUAREBRACKET {}
                 ;

method_decl_list : method_decl {}
                 | method_decl_list method_decl {}
                 ;

method_decl : type IDENTIFIER OPEN_PARANTHESIS type_identifier_list CLOSE_PARANTHESIS block {}
            | type IDENTIFIER OPEN_PARANTHESIS CLOSE_PARANTHESIS block {}    
            | VOID IDENTIFIER OPEN_PARANTHESIS type_identifier_list CLOSE_PARANTHESIS block {}
            | VOID IDENTIFIER OPEN_PARANTHESIS CLOSE_PARANTHESIS block {}
            ;

type_identifier_list : type_identifier {}
                     | type_identifier_list COMMA type_identifier {}
                     ;

type_identifier : type IDENTIFIER {}
                ;

block : OPEN_CURLYBRACE var_decl_list statement_list CLOSE_CURLYBRACE {}
      | OPEN_CURLYBRACE var_decl_list CLOSE_CURLYBRACE {}
      | OPEN_CURLYBRACE statement_list CLOSE_CURLYBRACE {}
      | OPEN_CURLYBRACE CLOSE_CURLYBRACE {}
      ;

statement_list : statement {}
               | statement_list statement {}
               ;

var_decl_list : var_decl {}
              | var_decl_list var_decl {}
              ;

var_decl : type identifier_list SEMICOLON {}
         ;

type : INT {}
     | BOOLEAN {}
     ;

statement : SEMICOLON {}
          | location assign_op expr SEMICOLON {}
          | method_call SEMICOLON {}
          | IF OPEN_PARANTHESIS expr CLOSE_PARANTHESIS block ELSE block {}
          | IF OPEN_PARANTHESIS expr CLOSE_PARANTHESIS block {}
          | FOR IDENTIFIER EQUAL expr COMMA expr block {}
          | RETURN expr SEMICOLON {}
          | RETURN SEMICOLON {}
          | BREAK SEMICOLON {}
          | CONTINUE SEMICOLON {}
          | block {}
          ;

assign_op : EQUAL {}
          | PLUSEQUAL {}
          | MINUSEQUAL {}
          ;

method_call : method_name OPEN_PARANTHESIS expr_list CLOSE_PARANTHESIS {}
            | method_name OPEN_PARANTHESIS CLOSE_PARANTHESIS {}
            | CALLOUT OPEN_PARANTHESIS STRING_VALUE COMMA callout_arg_list CLOSE_PARANTHESIS {}
            | CALLOUT OPEN_PARANTHESIS STRING_VALUE CLOSE_PARANTHESIS {}
            ;
  
expr_list : expr {}
          | expr_list COMMA expr {}
          ;

callout_arg_list : callout_arg {}
                 | callout_arg_list COMMA callout_arg {}
                 ;

method_name : IDENTIFIER {}
            ;

location : IDENTIFIER {}
         | IDENTIFIER OPEN_SQUAREBRACKET expr CLOSE_SQUAREBRACKET {}
         ;

expr : location {}
     | method_call {}
     | literal {}
     | expr OR expr {}
     | expr AND expr {}
     | expr EQUALEQUAL expr {}
     | expr NOTEQUAL expr {}
     | expr LESSTHAN expr {}
     | expr LESSEQUAL expr {}
     | expr GREATEREQUAL expr {}
     | expr GREATERTHAN expr {}
     | expr PLUS expr {}
     | expr MINUS expr {}
     | expr MULTIPLY expr {}
     | expr DIVIDE expr {}
     | expr MODULO expr {}
     | NOT expr {}
     | MINUS expr %prec UMINUS {}
     | OPEN_PARANTHESIS expr CLOSE_PARANTHESIS {}
     ;

callout_arg : expr  {}
            | STRING_VALUE {}
            ;

literal : INT_VALUE {}
        | CHAR_VALUE {}
        | TRUE {}
        | FALSE {}
        ;

%%


void yyerror (const char *s) {
    std::cerr << "Parse Error on Line : " << yylineno << std::endl << "Message : " << s << std::endl;
    exit(-1);
}

