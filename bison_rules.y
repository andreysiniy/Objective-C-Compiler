%{
	#include "classes_nodes.h"
	void yyerror(char const *s);
	extern int yylex(void);
	extern int lineCount;
	Program_node *root;
%}


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
%left '.' ARROW
%left '[' ']'
%nonassoc '(' ')'


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
%token SELF SUPER ID 
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


%start program

%%

program: function_and_class_list	
	   ;

function_and_class_list: class_block									
					   | function										
					   | class_declaration_list							
					   | function_and_class_list class_block			
					   | function_and_class_list function				
					   | function_and_class_list class_declaration_list	
					   ;



function: type IDENTIFIER '(' ')' '{' statement_list_e '}' 
		;
		

type: INT				
    | CHAR				
    | FLOAT				
    | ID				
    ;


numeric_constant: FLOAT_CONSTANT	
                | INTEGER_CONSTANT	
                ;

literal: STRING_CONSTANT	
       | CHAR_CONSTANT		
	   | NSSTRING_CONSTANT	
       ;


declaration: type init_declarator_list_e ';'	
		   | CLASS_NAME init_declarator_with_asterisk_list_e ';' 
           ;

declaration_list: declaration 					
				| declaration_list declaration	
				;

declaration_list_e: /*empty*/			
				  | declaration_list	
				  ;

init_declarator_list_e: /* empty */			
					| init_declarator_list	
					;

init_declarator_list: init_declarator							
					| init_declarator_list ',' init_declarator	
					;

init_declarator: IDENTIFIER														
			   | IDENTIFIER '=' expression										
			   | IDENTIFIER '[' expression_e ']'								
			   | IDENTIFIER '[' expression_e ']' '=' '{' expression_list_e '}' 
			   | IDENTIFIER '[' expression_e ']' '=' expression					
			   ;

declarator_with_asterisk: '*' IDENTIFIER					
						| '*' IDENTIFIER '[' expression ']'		
						;

declarator_with_asterisk_list: declarator_with_asterisk										
							 | declarator_with_asterisk_list ',' declarator_with_asterisk	
							 ;

declarator: IDENTIFIER						
		  | IDENTIFIER '[' expression ']'	
		  ;

declarator_list: declarator
			   | declarator_list ',' declarator								
			   ;

init_declarator_with_asterisk: '*' IDENTIFIER													
			   				 | '*' IDENTIFIER '=' expression									
							 | '*' IDENTIFIER '[' expression_e ']'									
			   				 | '*' IDENTIFIER '[' expression_e ']' '=' '{' expression_list_e '}' 	
							 | '*' IDENTIFIER '[' expression_e ']' '=' expression					
							 ;

init_declarator_with_asterisk_list: init_declarator_with_asterisk																		
								  | init_declarator_with_asterisk_list ',' init_declarator_with_asterisk	
								  ;

init_declarator_with_asterisk_list_e: /*empty*/								
									| init_declarator_with_asterisk_list	
									;

parameter_list: parameter_declaration						
			  | parameter_list ',' parameter_declaration	
			  ;

parameter_declaration: type IDENTIFIER			
					 | CLASS_NAME '*' IDENTIFIER	
					 ;



expression: IDENTIFIER							
		  | literal								
		  | numeric_constant					
		  | '(' expression ')'					
		  | SELF								
		  | SUPER								
		  | '[' receiver message_selector ']'	
		  | IDENTIFIER '(' expression_list_e ')' 
		  | '-' expression %prec UMINUS			
		  | '+' expression %prec UPLUS			
		  | '&' expression %prec UAMPERSAND		
		  | expression '+' expression			
		  | expression '-' expression			
		  | expression '*' expression			
		  | expression '/' expression			
		  | expression EQUAL expression			
		  | expression NOT_EQUAL expression		
		  | expression '>' expression			
		  | expression '<' expression			
		  | expression LESS_EQUAL expression	
		  | expression GREATER_EQUAL expression	
		  | expression '=' expression			
		  | expression '.' IDENTIFIER			
		  | expression ARROW IDENTIFIER			
		  | expression '[' expression ']'		
		  ;

expression_e: /*empty*/		
			| expression	
			;

expression_list_e: /*empty*/		
				 | expression_list	
				 ;

expression_list: expression							
			   | expression_list ',' expression		
			   ;

receiver: SUPER								
		| SELF								
		| IDENTIFIER						
		| IDENTIFIER '[' expression ']'		
		| CLASS_NAME						
		| '[' receiver message_selector ']'	
		;

message_selector: IDENTIFIER																
				| IDENTIFIER ':' expression keyword_argument_list_e							
				| IDENTIFIER ':' expression keyword_argument_list_e ',' expression_list		
				;

keyword_argument_list_e: /*empty*/				
					   | keyword_argument_list	
					   ;

keyword_argument_list: keyword_argument							
					 | keyword_argument_list keyword_argument	
					 ;

keyword_argument: IDENTIFIER ':' expression		
				| ':' expression				
				;


if_statement: IF '(' expression ')' statement					
			| IF '(' expression ')' statement ELSE statement	
			;


while_statement: WHILE '(' expression ')' statement		
			   ;

do_while_statement: DO statement WHILE '(' expression ')' ';'	
				  ;

for_statement: FOR '(' expression_list_e ';' expression_e ';' expression_list_e ')' statement						
			 | FOR '(' type init_declarator_list_e ';' expression_e ';' expression_list_e ')' statement		
			 | FOR '(' CLASS_NAME '*' init_declarator_list_e ';' expression_e ';' expression_list_e ')' statement			
			 | FOR '(' IDENTIFIER IN expression ')' statement											
			 | FOR '(' CLASS_NAME '*' IDENTIFIER IN expression ')' statement							
			 ;



statement: ';'							
		 | expression ';'				
		 | RETURN expression_e ';'		
		 | if_statement					
		 | while_statement				
		 | do_while_statement			
		 | for_statement				
		 | '{' statement_list_e '}'		
		 | declaration					
		 ;

statement_list: statement					
			  | statement_list statement	
			  ;

statement_list_e: /*empty*/			
				| statement_list	
				;

class_block: class_interface		
	 	   | class_implementation	
		   ;


class_interface: INTERFACE IDENTIFIER ':' IDENTIFIER interface_body_e END		
			   | INTERFACE IDENTIFIER interface_body_e END					
			   | INTERFACE IDENTIFIER ':' CLASS_NAME interface_body_e END		
			   | INTERFACE CLASS_NAME ':' IDENTIFIER interface_body_e END     
			   | INTERFACE CLASS_NAME interface_body_e END					
			   | INTERFACE CLASS_NAME ':' CLASS_NAME interface_body_e END		
			   ;

interface_body_e: /*empty*/			
				| interface_body	
				;

interface_body: '{' instance_variables_declaration_list_e '}' interface_declaration_list_e	
			  | interface_declaration_list						
			  ;

implementation_body_e: /*empty*/			
					 | implementation_body	
					 ;

implementation_body: '{' instance_variables_declaration_list_e '}' implementation_definition_list_e	
			       | implementation_definition_list						
				   ;

class_implementation: IMPLEMENTATION IDENTIFIER implementation_body_e END						
					| IMPLEMENTATION IDENTIFIER ':' IDENTIFIER implementation_body_e END		
					| IMPLEMENTATION CLASS_NAME implementation_body_e END						
					| IMPLEMENTATION CLASS_NAME ':' IDENTIFIER implementation_body_e END		
					| IMPLEMENTATION IDENTIFIER ':' CLASS_NAME implementation_body_e END		
					| IMPLEMENTATION CLASS_NAME ':' CLASS_NAME implementation_body_e END		
					;

class_declaration_list: CLASS identifier_list ';'	
					  ;

identifier_list: IDENTIFIER						
		  	   | identifier_list ',' IDENTIFIER	
		  	   ;

instance_variables_declaration_list_e: /*empty*/							
								| instance_variables_declaration_list	
								;

instance_variables_declaration: type declarator_list ';'						
							  | CLASS_NAME declarator_with_asterisk_list ';'	
							  ;

instance_variables_declaration_list: instance_variables_declaration											
								   | instance_variables_declaration_list instance_variables_declaration		
								   ;

interface_declaration_list_e: /*empty*/						
							| interface_declaration_list	
							;

interface_declaration_list: declaration										
						  | property										
						  | method_declaration								
						  | interface_declaration_list declaration			
						  | interface_declaration_list method_declaration	
						  | interface_declaration_list property				
						  ;

method_declaration: '+' method_type method_selector ';'		
				  | '+' '(' VOID ')' method_selector ';'	
				  | '+' method_selector ';'					
				  |  '-' method_type method_selector ';' 	
				  | '-' '(' VOID ')' method_selector ';'	
				  | '-' method_selector ';'					
				  ;

implementation_definition_list_e: /*empty*/							
								| implementation_definition_list	
								;

implementation_definition_list: declaration											
							  | SYNTHESIZE IDENTIFIER ';'											
							  | method_definition									
							  | implementation_definition_list declaration			
							  | implementation_definition_list SYNTHESIZE IDENTIFIER ';'			
							  | implementation_definition_list method_definition	
							  ;

method_definition: '+' method_type method_selector declaration_list_e '{' statement_list_e '}'	
				 | '+' '(' VOID ')' method_selector declaration_list_e '{' statement_list_e '}'	
				 | '+' method_selector declaration_list_e '{' statement_list_e '}'				
				 | '-' method_type method_selector declaration_list_e '{' statement_list_e '}'	
				 | '-' '(' VOID ')' method_selector declaration_list_e '{' statement_list_e '}'	
				 | '-' method_selector declaration_list_e '{' statement_list_e '}'				
				 ;

method_selector: IDENTIFIER																						
			   | IDENTIFIER ':' keyword_declaration_without_identifier keyword_selector_e						
			   | IDENTIFIER ':' keyword_declaration_without_identifier keyword_selector_e ',' parameter_list 	
			   ;

keyword_selector_e: /*empty*/			
				  | keyword_selector	
				  ;

keyword_selector: keyword_declaration						
				| keyword_selector keyword_declaration		
				;

keyword_declaration_without_identifier: method_type IDENTIFIER	
				   					  | IDENTIFIER				
									  ;

keyword_declaration: ':' method_type IDENTIFIER					
				   | ':' IDENTIFIER								
				   | IDENTIFIER ':' method_type IDENTIFIER		
				   | IDENTIFIER ':' IDENTIFIER					
				   ;

method_type: '(' type ')'	
		   | '(' CLASS_NAME '*' ')' 
		   | '(' type '['']' ')' 
		   | '(' CLASS_NAME '*' '['']' ')' 
		   ;

property: PROPERTY attribute type identifier_list ';'	
		| PROPERTY attribute CLASS_NAME '*' identifier_list ';'	
		;

attribute: /*empty*/			
		 | '(' ')'				
		 | '(' READONLY ')'		
		 | '(' READWRITE ')'	
		 ;

%%

void yyerror(char const *s)
{
	printf("%s in line %d",s, lineCount);
}
