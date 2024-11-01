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
%nonassoc NO_ELSE
%nonassoc ELSE

%token INT_TYPE FLOAT_TYPE DOUBLE_TYPE CHAR_TYPE CLASS_TYPE
%token INT_C FLOAT_C DOUBLE_C STRING_C 
%token ID SUPER SELF
%token IDENTIFIER
%token IF WHILE DO FOR
%token IN
%token RETURN
%token INTERFACE IMPLEMENTATION
%token READONLY READWRITE
%token END
%token PROPERTY
%token VOID

%start program

%%

program
        : stmt_list
        | classes 
        ;

type
        : INT_TYPE
        | CHAR_TYPE
        | FLOAT_TYPE
        | DOUBLE_TYPE
        | ID
        ;

constants
        : INT_C
        | FLOAT_C
        | DOUBLE_C
        | STRING_C
        ;

// DECLARATIONS RULES

decl
        : type init_decl_list_empt ';'
        | CLASS_TYPE init_astr_decl_list_empty ';'
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

field
        : IDENTIFIER
        | IDENTIFIER '[' expr ']'
        ;

field_list
        : field
        | field_list ',' field
        ;

field_astr
        : '*' IDENTIFIER
        | '*' IDENTIFIER '[' expr ']'
        ;

field_astr_list
        : field
        | field_astr_list ',' field
        ;

init_astr_decl
        : '*' IDENTIFIER
        | '*' IDENTIFIER '=' expr
        | IDENTIFIER '[' empty_expr ']'
        | IDENTIFIER '[' empty_expr ']' '=' '{' expr_list_empty '}'
        | IDENTIFIER '[' empty_expr ']' '=' expr
        ;

init_astr_decl_list
        : init_astr_decl
        | init_astr_decl_list ',' init_astr_decl
        ;

init_astr_decl_list_empty
        : /* empty */
        | init_astr_decl_list
        ;

// EXPRESSIONS RULES

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

// STATEMENTS RULES 

if_stmt
        : IF '(' expr ')' stmt %prec NO_ELSE
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
        | if_stmt
        | while_stmt
        | do_while_stmt
        | for_stmt
        | RETURN empty_expr ';'
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

// CLASSES RULES 

classes
        : class_interface
        | class_implementation
        ;

class_interface
        : INTERFACE IDENTIFIER ':' IDENTIFIER empty_interface_body END
	| INTERFACE IDENTIFIER empty_interface_body END
	| INTERFACE IDENTIFIER ':' CLASS_TYPE empty_interface_body END
        | INTERFACE CLASS_TYPE ':' IDENTIFIER empty_interface_body END
        | INTERFACE CLASS_TYPE empty_interface_body END
        | INTERFACE CLASS_TYPE ':' CLASS_TYPE empty_interface_body END
	;

class_implementation
        : IMPLEMENTATION IDENTIFIER empty_implementation_body END
        | IMPLEMENTATION IDENTIFIER ':' IDENTIFIER empty_implementation_body END
        | IMPLEMENTATION CLASS_TYPE implementation_body END
        | IMPLEMENTATION CLASS_TYPE ':' IDENTIFIER  empty_implementation_body END
        | IMPLEMENTATION IDENTIFIER ':' CLASS_TYPE empty_implementation_body END
        | IMPLEMENTATION CLASS_TYPE ':' CLASS_TYPE empty_implementation_body END
        ;

empty_interface_body
        : /* empty */
        | interface_body
        ;

interface_body
        : '{' inst_fields_decl_list_empty '}' interface_decl_list_empty
        | interface_decl_list
        ;

empty_implementation_body
        : /* empty */
        | implementation_body
        ;

implementation_body
        : '{' inst_fields_decl_list_empty '}' implementation_decl_list_empty
        | implementation_decl_list

inst_fields_decl_list_empty
        : /* empty */
        | inst_fields_decl_list
        ;

inst_fields_decl_list
        : inst_fields_decl
        | inst_fields_decl_list inst_fields_decl
        ;

inst_fields_decl
        : type field_list ';'
        | CLASS_TYPE field_astr_list ';'
        ; 


interface_decl_list_empty
        : /* empty */
        | interface_decl_list
        ;

interface_decl_list
        : decl
        | property
        | method_decl
        | interface_decl_list decl
        | interface_decl_list method_decl
        | interface_decl_list property
        ;

// TODO: params support
method_decl
        : '+' method_type IDENTIFIER ';'
        | '-' method_type IDENTIFIER ';'
        | '+' '(' VOID ')' IDENTIFIER ';'
        | '-' '(' VOID ')' IDENTIFIER ';'
        ;

implementation_decl_list_empty
        : /* empty */
        | implementation_decl_list
        ;

implementation_decl_list
        : decl
        | method_decl
        | implementation_decl_list decl
        | implementation_decl_list method_decl
        ;

method_type
        : '(' type ')'
        | '(' CLASS_TYPE '*' ')'
        | '(' type '['']' ')' 
        | '(' CLASS_TYPE '*' '['']' ')'
        ;

property
        : PROPERTY attr type IDENTIFIER ';'
        | PROPERTY attr CLASS_TYPE '*' IDENTIFIER ';'
        ;

attr
        : /* empty */
        | '(' ')'
        | '(' READONLY ')'
        | '(' READWRITE ')'
        ;

%%