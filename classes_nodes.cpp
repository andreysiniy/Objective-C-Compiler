#include "classes_nodes.h"
#include <string>
using namespace std;
long maxId = 0; // Глобальный id узла
set<string> ClassNames; //Множество имен объявленных классов

//---------- program ----------

Program_node* Program_node::createProgramNode(Function_and_class_list_node* list)
{
	Program_node *res = new Program_node;
    res->id = maxId++;
    res->list = list;
    return res;
}

// ---------- function_and_class_list ----------

Function_and_class_list_node* Function_and_class_list_node::createFunctionAndClassListNodeFromFunction(Function_node *functionList)
{
    Function_and_class_list_node *res = new Function_and_class_list_node;
    res->id = maxId++;
    res->FunctionsAndClasses = new vector<function_and_class>;
    struct function_and_class functionAndClass;
    functionAndClass.function = functionList;
    functionAndClass.class_block = NULL;
    functionAndClass.class_declaration_list = NULL;
    res->FunctionsAndClasses->push_back(functionAndClass);
    return res;
}

Function_and_class_list_node* Function_and_class_list_node::createFunctionAndClassListNodeFromClassDeclarationList(Class_declaration_list_node *classDeclarationList)
{
    Function_and_class_list_node *res = new Function_and_class_list_node;
    res->id = maxId++;
    res->FunctionsAndClasses = new vector<function_and_class>;
    struct function_and_class functionAndClass;
    functionAndClass.class_declaration_list = classDeclarationList;
    functionAndClass.class_block = NULL;
    functionAndClass.function = NULL;
    res->FunctionsAndClasses->push_back(functionAndClass);

    return res;
}

Function_and_class_list_node* Function_and_class_list_node::createFunctionAndClassListNodeFromClassBlock(Class_block_node *classBlock)
{
    Function_and_class_list_node *res = new Function_and_class_list_node;
    res->id = maxId++;
    res->FunctionsAndClasses = new vector<function_and_class>;
    struct function_and_class functionAndClass;
    functionAndClass.class_block = classBlock;
    functionAndClass.class_declaration_list = NULL;
    functionAndClass.function = NULL;
    res->FunctionsAndClasses->push_back(functionAndClass);
    return res;
}

Function_and_class_list_node* Function_and_class_list_node::addToFunctionAndClassListNodeFromFunction(Function_and_class_list_node *list, Function_node *function)
{
    struct function_and_class functionAndClass;
    functionAndClass.function = function;
    functionAndClass.class_block = NULL;
    functionAndClass.class_declaration_list = NULL;
    list->FunctionsAndClasses->push_back(functionAndClass);
    return list;
}

Function_and_class_list_node* Function_and_class_list_node::addToFunctionAndClassListNodeFromClassDeclarationList(Function_and_class_list_node *list, Class_declaration_list_node *classDeclarationList)
{
    struct function_and_class functionAndClass;
    functionAndClass.class_declaration_list = classDeclarationList;
    functionAndClass.class_block = NULL;
    functionAndClass.function = NULL;
    list->FunctionsAndClasses->push_back(functionAndClass);
    return list;
}

Function_and_class_list_node* Function_and_class_list_node::addToFunctionAndClassListNodeFromClassBlock(Function_and_class_list_node *list, Class_block_node *classStatement)
{
    struct function_and_class functionAndClass;
    functionAndClass.class_block = classStatement;
    functionAndClass.class_declaration_list = NULL;
    functionAndClass.function = NULL;
    list->FunctionsAndClasses->push_back(functionAndClass);
    return list;
}

// ---------- function ----------

Function_node* Function_node::createFunctionNode(Type_node *type, char *name, Statement_list_node *statement)
{
    Function_node *res = new Function_node;
    res->id = maxId++;
    res->Type = type;
    res->Name = name;
    res->statement = statement;
    return res;
}

// -------------------- Типы --------------------

// ---------- type ----------

Type_node* Type_node::createTypeNode(type_type type)
{
    Type_node *res = new Type_node;
    res->id = maxId++;
    res->type = type;
    res->ClassName = NULL;
    return res;
}

Type_node* Type_node::createTypeNodeFromClassName(type_type type, char *name)
{
    Type_node *res = new Type_node;
    res->id = maxId++;
    res->type = type;
    res->ClassName = name;
    return res;
}

//-------------------- Константы --------------------

// ---------- numeric_constant ----------

Numeric_constant_node* Numeric_constant_node::createNumericConstantNodeFromInteger(int number)
{
    Numeric_constant_node *res = new Numeric_constant_node;
    res->id = maxId++;
    res->type = INTEGER_CONSTANT_TYPE;
    res->number.Int = number;
    return res;
}

Numeric_constant_node* Numeric_constant_node::createNumericConstantNodeFromFloat(float number)
{
    Numeric_constant_node *res = new Numeric_constant_node;
    res->id = maxId++;
    res->type = FLOAT_CONSTANT_TYPE;
    res->number.Float = number;
    return res;
}

// ---------- literal ----------

Literal_node* Literal_node::createLiteralNode(literal_type type, char *value)
{
    Literal_node *res = new Literal_node;
    res->id = maxId++;
    res->type = type;
    res->value = value;
    return res;
}

// --------------------- Объявления --------------------

// ---------- declaration ----------

Declaration_node* Declaration_node::createDeclarationNode(Type_node *type, Init_declarator_list_node *initDeclarators)
{
    Declaration_node *res = new Declaration_node;
    res->id = maxId++;
    res->type = type;
    res->init_declarator_list = initDeclarators;
    res->Next = NULL;
    return res;
}

// ---------- declaration_list ----------

Declaration_list_node* Declaration_list_node::createDeclarationListNode(Declaration_node *declaration)
{
    Declaration_list_node *res = new Declaration_list_node;
    res->id = maxId++;
    res->First = declaration;
    res->Last = declaration;
    return res;
}

Declaration_list_node* Declaration_list_node::addToDeclarationListNode(Declaration_list_node *list, Declaration_node *declaration)
{
    list->Last->Next = declaration;
    list->Last = declaration;
    return list;
}

// ---------- init_declarator_list ----------

Init_declarator_list_node* Init_declarator_list_node::createInitDeclaratorListNode(Init_declarator_node *initDeclarator)
{
    Init_declarator_list_node *res = new Init_declarator_list_node;
    res->id = maxId++;
    res->First = initDeclarator;
    res->Last = initDeclarator;
    return res;
}

Init_declarator_list_node* Init_declarator_list_node::addToInitDeclaratorListNode(Init_declarator_list_node *list, Init_declarator_node *initDeclarator)
{
    list->Last->Next = initDeclarator;
    list->Last = initDeclarator;
    return list;
}

// ---------- init_declarator ----------

Init_declarator_node* Init_declarator_node::createInitDeclaratorNode(init_declarator_type type, char *declarator, Expression_node *expression)
{
    Init_declarator_node *res = new Init_declarator_node;
    res->id = maxId++;
    res->type = type;
    res->Declarator = declarator;
    res->expression = expression;
    res->ArraySize = NULL;
    res->InitializerList = NULL;
    res->Next = NULL;
    return res;
}

Init_declarator_node* Init_declarator_node::createInitDeclaratorNodeFromArray(init_declarator_type type, char *declarator, Expression_node *arraySize, Expression_node *expression, Expression_list_node *initializerList)
{
    Init_declarator_node *res = new Init_declarator_node;
    res->id = maxId++;
    res->type = type;
    res->Declarator = declarator;
    res->expression = expression;
    res->ArraySize = arraySize;
    res->InitializerList = initializerList;
    res->Next = NULL;
    return res;
}

// ---------- declarator ----------

Declarator_node* Declarator_node::createDeclaratorNode(char *identifier, Expression_node *expression)
{
    Declarator_node *res = new Declarator_node;
    res->id = maxId++;
    res->Identifier = identifier;
    res->Expression = expression;
    return res;
}

// ---------- declarator_list ----------

Declarator_list_node* Declarator_list_node::createDeclaratorListNode(Declarator_node *declarator)
{
    Declarator_list_node *res = new Declarator_list_node;
    res->id = maxId++;
    res->Declarators = new vector<Declarator_node*>;
    res->Declarators->push_back(declarator);
    return res;
}

Declarator_list_node* Declarator_list_node::addToDeclaratorListNode(Declarator_list_node *list, Declarator_node *declarator)
{
    list->Declarators->push_back(declarator);
    return list;
}

// ---------- parameter_list ----------

Parameter_list_node* Parameter_list_node::createParameterListNode(Parameter_declaration_node *parameter)
{
    Parameter_list_node *res = new Parameter_list_node;
    res->id = maxId++;
    res->First = parameter;
    res->Last = parameter;
    return res;
}

Parameter_list_node* Parameter_list_node::addToParameterListNode(Parameter_list_node *list, Parameter_declaration_node *parameter)
{
    list->Last->Next = parameter;
    list->Last = parameter;
    return list;
}

// ---------- parameter_declaration ----------

Parameter_declaration_node* Parameter_declaration_node::createParameterDeclarationNode(Type_node *type, char *name)
{
    Parameter_declaration_node *res = new Parameter_declaration_node;
    res->id = maxId++;
    res->type = type;
    res->name = name;
    res->Next = NULL;
    return res;
}
