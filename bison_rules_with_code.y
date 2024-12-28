%{
	#include "classes_nodes.h"
	void yyerror(char const *s);
	extern int yylex(void);
	extern int lineCount;
	Program_node *root;
%}

// ---------- Объединение ----------
%union {
	int Integer_constant;
	char *String_constant;
	char *Char_constant;
	float Float_constant;
	char *NSString_constant;
	char *Identifier;
	Type_node *Type;
	Numeric_constant_node *Numeric_constant;
	Literal_node *Literal;
	Declaration_node *Declaration;
	Declaration_list_node *Declaration_list;
	Init_declarator_list_node *Init_declarator_list;
	Init_declarator_node *Init_declarator;
	Parameter_list_node *Parameter_list;
	Parameter_declaration_node *Parameter_declaration;
	Expression_node *Expression;
	Expression_list_node *Expression_list;
	Receiver_node *Receiver;
	Message_selector_node *Message_selector;
	Keyword_argument_list_node *Keyword_argument_list;
	Keyword_argument_node *Keyword_argument;
	If_statement_node *If;
	While_statement_node *While;
	Do_while_statement_node *Do_while;
	For_statement_node *For;
	Statement_node *Statement;
	Statement_list_node *Statement_list;
	Class_block_node *Class_block;
	Class_interface_node *Class_interface;
	Interface_body_node *Interface_body;
	Implementation_body_node *Implementation_body;
	Class_implementation_node *Class_implementation;
	Class_declaration_list_node *Class_declaration_list;
	Interface_declaration_list_node *Interface_declaration_list;
	Method_declaration_node *Method_declaration;
	Implementation_definition_list_node *Implementation_definition_list;
	Method_definition_node *Method_definition;
	Method_selector_node *Method_selector;
	Keyword_selector_node *Keyword_selector;
	Keyword_declaration_node *Keyword_declaration;
	Property_node *Property;
	Attribute_node *Attribute;
	Program_node *Program;
	Identifier_list_node *Identifier_list;
	Function_and_class_list_node *Function_and_class_list;
	Function_node *Function;
	Declarator_list_node *Declarator_list;
	Instance_variables_declaration_node *Instance_variables_declaration;
	Instance_variables_declaration_list_node *Instance_variables_declaration_list;
	Declarator_node *Declarator;
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