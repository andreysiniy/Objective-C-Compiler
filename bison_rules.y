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
%nonassoc '(' ')' '[' ']'

%token INT_TYPE FLOAT_TYPE DOUBLE_TYPE CHAR_TYPE CLASS_TYPE
%token INT_C FLOAT_C DOUBLE_C STRING_C 
%token ID SUPER SELF
%token IDENTIFIER
%start program

%%

program: stmt_list 
        ;

type: INT_TYPE
    | CHAR_TYPE
    | FLOAT_TYPE
    | DOUBLE_TYPE
    | ID
	| CLASS_TYPE '*'
    ;

constants: INT_C
        |  FLOAT_C
        |  DOUBLE_C
        |  STRING_C
        ;

// declarations

decl: type init_decl_list_empt ';'
    ;

decl_list: decl
        |  decl_list decl
        ;

decl_list_empt: /* empty */
            |   decl_list
            ;

init_decl_list_empt: /* empty */
                |   init_decl_list
                ;

init_decl_list: init_decl
            |   init_decl_list ',' init_decl
            ;

init_decl: IDENTIFIER
        |   IDENTIFIER '=' expr
        ;
// expressions

expr: IDENTIFIER
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

expr_msg: '[' receiver message ']'
        ;

receiver: SUPER
        | SELF
        | CLASS_TYPE
        ;

message: IDENTIFIER
		| message_arg_list
		;

message_arg_list: message_arg
				| message_arg_list message_arg
				;

message_arg: IDENTIFIER ':' expr
			| ':' expr
			;

// statements

stmt_list: stmt
        |  stmt_list stmt
        ;

stmt: ';'
    |  expr ';'
    |  decl
    ;
%%