%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <iostream>

  int yylex(void);
  int yyerror(const char *s);
%}

%right '='
%left EQUAL NOT_EQUAL
%left '<' '>' LESS_EQUAL GREATER_EQUAL
%left '+' '-'
%left '*' '/'
%right UMINUS UPLUS UAMPERSAND
%left '[' ']'
%nonassoc '(' ')' 

%token INT_TYPE FLOAT_TYPE DOUBLE_TYPE CHAR_TYPE CLASS_TYPE
%token INT_C FLOAT_C DOUBLE_C STRING_C 
%token ID SUPER SELF
%token IDENTIFIER
%token IF ELSE WHILE DO FOR
%token IN
%token RETURN
%token INTERFACE IMPLEMENTATION
%token READONLY READWRITE
%token END
%token PROPERTY

%start program

%%

program
        : stmt_list 
        ;

type
        : INT_TYPE
        | CHAR_TYPE
        | FLOAT_TYPE
        | DOUBLE_TYPE
        | ID
	| CLASS_TYPE '*'
        ;

constants
        : INT_C
        | FLOAT_C
        | DOUBLE_C
        | STRING_C
        ;

// declarations

decl
        : type init_decl_list_empt ';'
        ;

decl_list
        : decl
        | decl_list decl
        ;

decl_list_empt
        : /* empty */
        | decl_list
        ;

init_decl_list_empt
        : /* empty */
        | init_decl_list
        ;

init_decl_list
        : init_decl
        | init_decl_list ',' init_decl
        ;

init_decl
        : IDENTIFIER
        | IDENTIFIER '=' expr
        | IDENTIFIER '[' empty_expr ']'
        | IDENTIFIER '[' empty_expr ']' '=' '{' expr_list_empty '}'
        | IDENTIFIER '[' empty_expr ']' '=' expr
        ;

// expressions

expr
        : IDENTIFIER
        | constants
        | expr_msg
        | '(' expr ')'
        | '-' expr %prec UMINUS
        | '+' expr %prec UPLUS
        | '&' expr %prec UAMPERSAND
        | expr '+' expr
        | expr '-' expr
        | expr '*' expr
        | expr '/' expr
        | expr EQUAL expr
        | expr NOT_EQUAL expr
        | expr LESS_EQUAL expr
        | expr GREATER_EQUAL expr
        | expr '=' expr
        | expr '<' expr
        | expr '>' expr
        ;

empty_expr
        : /* empty */
        | expr
        ;
        
expr_list_empty
        : /* empty */
        | expr_list
        ;

expr_list
        : expr
        | expr_list ',' expr

expr_msg
        : '[' receiver message ']'
        ;

receiver
        : SUPER
        | SELF
        | CLASS_TYPE
        | '[' receiver message ']'
        ;

message
        : IDENTIFIER
	| message_arg_list
	;

message_arg_list
        : message_arg
	| message_arg_list message_arg
	;

message_arg
        : IDENTIFIER ':' expr
	| ':' expr
	;

// statements

if_stmnt
        : IF '(' expr ')' stmt
	| IF '(' expr ')' stmt ELSE stmt
	;

while_stmt
        : WHILE '(' expr ')' stmt
	;

do_while_stmt
        : DO stmt WHILE '(' expr ')' ';'
	;

for_stmt
        : FOR '(' empty_expr ';' empty_expr ';' empty_expr ')' stmt
	| FOR '(' IDENTIFIER IN expr ')' stmt
	| FOR '(' type IDENTIFIER IN expr ')' stmt
	;


stmt
        : ';'
        | expr ';'
        | decl
        | if_stmnt
        | while_stmt
        | do_while_stmt
        | for_stmt
        | RETURN empty_expr
        | '{' empty_stmt_list '}'
        | decl
        ;

stmt_list
        : stmt
        | stmt_list stmt
        ;

empty_stmt_list
        : /* empty */
        | stmt_list
        ;

class_stmt
        : class_interface
	| class_implementation
	;

class_stmt_list
        : class_stmt
	| class_stmt_list class_stmt
	;

// classes

class_interface
        : INTERFACE IDENTIFIER ':' IDENTIFIER interface_stmt END
	| INTERFACE IDENTIFIER interface_stmt END
	| INTERFACE IDENTIFIER ':' CLASS_TYPE interface_stmt END
	;

interface_stmt
        : instance_vars interface_decl_list
        | init_decl_list
        ;

interface_decl_list
        : decl
        | pr
        ;

implementation_stmt
        : instance_vars implementation_def_list
        | implementation_def_list
        ;

class_implementation
        :

implementation_def_list
        :

instance_vars
        :

method_type
        : '(' type ')'
        ;

property
        : PROPERTY '(' attr ')' type IDENTIFIER ';'
        ;

attr
        : READONLY
        | READWRITE
        ;

%%