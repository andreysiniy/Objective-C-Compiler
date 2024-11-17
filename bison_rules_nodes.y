%{
	#include "classes_nodes.h"
	void yyerror(char const *s);
	extern int yylex(void);
	Program_node *root;
%}

%union {
    int Int_const;
    char *String_const;
    char *Char_const;
    float Float_const;
    char *Identifier;
    Type_node *Type;
    Numeric_const_node *Numeric_const;
    Literal_node *Literal;
    Consts_node *Consts;
    Decl_node *Decl;
    Decl_list_node *Decl_list;
    Init_decl_list_node *Init_decl_list;
    Init_decl *Init_decl;
    Param_list_node *Param_list;
    Param_decl_node *Param_decl;
    Expr_node *Expr;
    Expr_list_node *Expr_list;
    Receiver_node *Receiver;
    Message *Message;
    Message_arg_list_node *Arg_list;
    Message_arg_node *Arg;
    If_stmt_node *If;
    While_stmt_node *While;
    Do_while_stmt_node *Do_while;
    For_stmt_node *For;
    Stmt_node *Stmt;
    Stmt_list_node *Stmt_list;
    Class_block_node *Class_block;
    Class_interface_node *Class_interface;
    Interface_body_node *Interface_body;
    Implementation_body_node *Implementation_body;
    class_implementation_node *Class_implementation;
    Class_decl_list_node *Class_decl_list;
    Interface_decl_list_node *Interface_decl_list;
    Method_decl_node *Method_decl;
    Implementation_def_list_node *Implementation_def_list;
    Method_def_node *Method_def;
    Keyword_list_node *Keyword_list;
    Keyword_decl_node *Keyword_decl;
    Property_node *Property;
    Attr_node *Attr;
    Program_node *Program;
    Class_list_node *Class_list;
    //TODO function and function_and_classes
    Field_list_node *Filed_list;
    Inst_fields_decl_list_node *Inst_field_decl_list;
    Inst_fields_decl_list *Inst_field_decl;
    Field_node *Field;   
}

%right '='
%left EQUAL NOT_EQUAL
%left '<' '>' LESS_EQUAL GREATER_EQUAL
%left '+' '-'
%left '*' '/'
%right UMINUS UPLUS UAMPERSAND
%left '[' ']'
%left '.' ARROW
%nonassoc '(' ')' 
%nonassoc NO_ELSE
%nonassoc ELSE
%nonassoc PREC_EXPR
%nonassoc ARRAY_EXPR 

%token INT_TYPE FLOAT_TYPE DOUBLE_TYPE CHAR_TYPE 
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

%type <Type> type method_type
%type <Consts> constants
%type <Decl> decl
%type <Decl_list> decl_list_empty decl_list
%type <Init_decl_list> init_astr_decl_list init_astr_decl_list_empty init_decl_list init_decl_list_empty
%type <Init_decl> init_astr_decl init_decl
%type <Param_list> param_list
%type <Param_decl> param_decl
%type <Expr> expr //TODO НЕТ ПУСТОГО ЭКСПРА
%type <Expr_list> expr_list expr_list_empty
%type <Receiver> receiver
%type <Message> message
%type <Arg_list> message_arg_list
%type <Arg> message_arg
%type <If> if_stmt
%type <While> while_stmt
%type <Do_while> do_while_stmt
%type <For> for_stmt
%type <Stmt> stmt
%type <Stmt_list> stmt_list empty_stmt_list
%type <Class_block> //TODO нет Class block
%type <Class_interface> class_interface
%type <Interface_body> interface_body //TODO interface_body_empty
%type <Implementation_body> implementation_body
%type <Class_implementation> class_implementation
%type <Class_decl_list> //TODO ????
%type <Class_list> classes 
%type <Interface_decl_list> interface_decl_list interface_decl_list_empty
%type <Method_decl> method_decl
%type <Implementation_def_list> //TODO ???
%type <Method_def> method_def
//TODO Method_selector
%type <Keyword_list> keyword_list keyword_list_empty
%type <Keyword_decl> keyword_decl //TODO
%type <Property> property
%type <Attr> attr
%type <Program> program
%type <Filed_list> field_list field_astr_list
%type <Inst_field_decl> inst_fields_decl
%type <Inst_field_decl_list> inst_fields_decl_list_empty inst_fields_decl_list
%type <Field> field field_astr