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
