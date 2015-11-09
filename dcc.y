%{
    #include <iostream>
    #include "AST.h"

    extern "C" int yylex();
    extern "C" int yyparse();
    extern "C" FILE * yyin;
    extern int yylineno;

    void yyerror(const char *s);

    ASTProgram * program;
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

%nonassoc LESSEQUAL LESSTHAN GREATEREQUAL GREATERTHAN

%left EQUALEQUAL NOTEQUAL
%left OR
%left AND
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO

%right NOT UMINUS
%right EQUAL PLUSEQUAL MINUSEQUAL

%token <sval> IDENTIFIER

%token <ival> INT_VALUE
%token <sval> STRING_VALUE
%token <cval> CHAR_VALUE

%type <ASTProgram *> program

%type <ASTFieldDecl *> field_decl
%type <std::vector<ASTFieldDecl *> *> field_decl_list

%type <std::vector<ASTVarIdentifier *> *> identifier_list

%type <ASTArrayIdentifier *> identifier_array
%type <std::vector<ASTArrayIdentifier *> *> identifier_array_list

%type <ASTMethodDecl *> method_decl
%type <ASTTypeIdentifier *> type_identifier
%type <ASTBlockStatement *> block

%%

program : CLASS IDENTIFIER OPEN_CURLYBRACE field_decl_list method_decl_list CLOSE_CURLYBRACE { $$ = new ASTProgram($2, $4, $5); }
        | CLASS IDENTIFIER OPEN_CURLYBRACE field_decl_list CLOSE_CURLYBRACE  { $$ = new ASTProgram($2, null, $4); }
        | CLASS IDENTIFIER OPEN_CURLYBRACE method_decl_list CLOSE_CURLYBRACE { $$ = new ASTProgram($2, $4, null); }
        | CLASS IDENTIFIER OPEN_CURLYBRACE CLOSE_CURLYBRACE { $$ = new ASTProgram($2, null, null); }
        ;

field_decl_list : field_decl { $$ = new std::vector<ASTFieldDecl *>(); $$->push_back($1); }
                | field_decl_list field_decl { $1->push_back($2); $$ = $1; }
                ;

field_decl : type identifier_list SEMICOLON { $$ = new ASTFieldDecl($2, $1); }
           | type identifier_array_list SEMICOLON { $$ = new ASTFieldDecl($2, $1); }
           ;

identifier_list : IDENTIFIER { $$ = new std::vector<ASTVarIdentifier *>(); $$->push_back(new ASTVarIdentifier(std::string($1))); }
                | identifier_list COMMA IDENTIFIER { $1->push_back(new ASTVarIdentifier(std::string($3))); $$ = $1; }
                ;


identifier_array_list : identifier_array { $$ = new std::vector<ASTArrayIdentifier *>(); $$->push_back($1); }
                      | identifier_array_list COMMA identifier_array { $1->push_back($3); $$ = $1; }
                      ;

identifier_array : IDENTIFIER OPEN_SQUAREBRACKET INT_VALUE CLOSE_SQUAREBRACKET { $$ = new ASTArrayIdentifier(std::string($1), $3); }
                 ;

method_decl_list : method_decl { $$ = new std::vector<ASTMethodDecl *>(); $$->push_back($1); }
                 | method_decl_list method_decl { $1->push_back($2); $$ = $1; }
                 ;

method_decl : type IDENTIFIER OPEN_PARANTHESIS type_identifier_list CLOSE_PARANTHESIS block { $$ = new ASTMethodDecl(std::string($2), $1, $4, $6); }
            | type IDENTIFIER OPEN_PARANTHESIS CLOSE_PARANTHESIS block { $$ = new ASTMethodDecl(std::string($2), $1, null, $5); }
            | VOID IDENTIFIER OPEN_PARANTHESIS type_identifier_list CLOSE_PARANTHESIS block { $$ = new ASTMethodDecl(std::string($2), Datatype::void_type, $4, $6); }
            | VOID IDENTIFIER OPEN_PARANTHESIS CLOSE_PARANTHESIS block { $$ = new ASTMethodDecl(std::string($2), Datatype::void_type, null, $5); }
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

type : INT { $$ =  }
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

