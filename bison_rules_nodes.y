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
    Decl_node *Decl;
    Decl_list_node *Decl_list;
    Init_decl_list_node *Init_decl_list;
    Param_list_node *Param_list;
    Param_decl_node *Param_decl;
    Expr_node *Expr;
    Expr_list_node *Expr_list;
    Receiver_node *Receiver;
    Message *Message;
    Message_arg_list_node *Arg_list;
    Message_arg_node *Arg_node;
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