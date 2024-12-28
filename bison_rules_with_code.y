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

// ---------- Операции с их приоритетом ----------
%right '='
%left EQUAL NOT_EQUAL
%left '<' '>' LESS_EQUAL GREATER_EQUAL
%left '+' '-'
%left '*' '/'
%right UMINUS UPLUS UAMPERSAND
%left '.' ARROW
%left '[' ']'
%nonassoc '(' ')'

//---------- Терминальные символы ----------
%token <Type> INT 
%token <Type> CHAR 
%token <Type> FLOAT
%token <Type> VOID
%token ENUM
%token IF ELSE WHILE DO FOR
%token IN
%token INTERFACE IMPLEMENTATION 
%token END
%token PROPERTY
%token READONLY READWRITE
%token SYNTHESIZE
%token SELF SUPER ID /* НУЖНО ЛИ SUPER добавлять в expression */
%token CLASS
%token RETURN
%token <Integer_constant> INTEGER_CONSTANT
%token <String_constant> STRING_CONSTANT
%token <Char_constant> CHAR_CONSTANT
%token <Float_constant> FLOAT_CONSTANT
%token <NSString_constant> NSSTRING_CONSTANT
%token <Identifier> IDENTIFIER
%token <Identifier> CLASS_NAME

%type <Type> type method_type
%type <Numeric_constant> numeric_constant
%type <Literal> literal
%type <Declaration> declaration
%type <Declaration_list> declaration_list_e declaration_list
%type <Init_declarator_list> init_declarator_list_e init_declarator_list init_declarator_with_asterisk_list init_declarator_with_asterisk_list_e
%type <Init_declarator> init_declarator init_declarator_with_asterisk
%type <Parameter_list> parameter_list
%type <Parameter_declaration> parameter_declaration
%type <Expression> expression expression_e
%type <Expression_list> expression_list expression_list_e
%type <Receiver> receiver
%type <Message_selector> message_selector
%type <Keyword_argument_list> keyword_argument_list keyword_argument_list_e
%type <Keyword_argument> keyword_argument
%type <If> if_statement
%type <While> while_statement
%type <Do_while> do_while_statement
%type <For> for_statement
%type <Statement> statement
%type <Statement_list> statement_list_e statement_list
%type <Class_block> class_block
%type <Class_interface> class_interface
%type <Interface_body> interface_body interface_body_e
%type <Implementation_body> implementation_body implementation_body_e
%type <Class_implementation> class_implementation
%type <Class_declaration_list> class_declaration_list 
%type <Identifier_list> identifier_list
%type <Interface_declaration_list> interface_declaration_list interface_declaration_list_e
%type <Method_declaration> method_declaration 
%type <Implementation_definition_list> implementation_definition_list implementation_definition_list_e
%type <Method_definition> method_definition 
%type <Method_selector> method_selector
%type <Keyword_selector> keyword_selector keyword_selector_e
%type <Keyword_declaration> keyword_declaration keyword_declaration_without_identifier
%type <Property> property
%type <Attribute> attribute
%type <Program> program 
%type <Function_and_class_list> function_and_class_list
%type <Function> function
%type <Declarator_list> declarator_list declarator_with_asterisk_list
%type <Instance_variables_declaration> instance_variables_declaration
%type <Instance_variables_declaration_list> instance_variables_declaration_list instance_variables_declaration_list_e
%type <Declarator> declarator_with_asterisk declarator
