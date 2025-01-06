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

// -------------------- Выражения --------------------

// ---------- expression ----------

Expression_node* Expression_node::createExpressionNodeFromIdentifier(char *name)
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = IDENTIFIER_EXPRESSION_TYPE;
    res->name = name;
    res->Left = NULL;
    res->Right = NULL;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromLiteral(Literal_node *value)
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = LITERAL_EXPRESSION_TYPE;
    res->name = NULL;
    res->constant.literal = value;
    res->Left = NULL;
    res->Right = NULL;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromNumericConstant(Numeric_constant_node *value)
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = NUMERIIC_CONSTANT_EXPRESSION_TYPE;
    res->name = NULL;
    res->constant.num = value;
    res->Left = NULL;
    res->Right = NULL;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromSimpleExpression(expression_type type, Expression_node *expression)
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = type;
    res->name = NULL;
    res->Left = NULL;
    res->Right = expression;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromSelf()
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = SELF_EXPRESSION_TYPE;
    res->name = NULL;
    res->Left = NULL;
    res->Right = NULL;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromSuper()
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = SUPER_EXPRESSION_TYPE;
    res->name = NULL;
    res->Left = NULL;
    res->Right = NULL;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromOperator(expression_type type, Expression_node *leftExpression, Expression_node *rightExpression)
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = type;
    res->name = NULL;
    res->Left = leftExpression;
    res->Right = rightExpression;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromMessageExpression(Receiver_node *receiver, Message_selector_node *arguments)
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = MESSAGE_EXPRESSION_EXPRESSION_TYPE;
    res->Receiver = receiver;
    res->Arguments = arguments;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromFunctionCall(char *name, Expression_list_node *argumentsList)
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = FUNCTION_CALL_EXPRESSION_TYPE;
    res->name = name;
    res->ArgumentsList = argumentsList;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::createExpressionNodeFromMemberAccessOperator(expression_type type, Expression_node *expression, char *memberName)
{
    Expression_node *res = new Expression_node;
    res->id = maxId++;
    res->type = type;
    res->name = memberName;
    res->Left = expression;
    res->Right = NULL;
    res->Next = NULL;
    return res;
}

Expression_node* Expression_node::setPriority(Expression_node *expression, bool priority)
{
    expression->isPriority = priority;
    return expression;
}

// ---------- expression_list ----------

Expression_list_node* Expression_list_node::createExpressionListNode(Expression_node *expression)
{
    Expression_list_node *res = new Expression_list_node;
    res->id = maxId++;
    res->First = expression;
    res->Last = expression;
    return res;
}

Expression_list_node* Expression_list_node::addToExpressionListNode(Expression_list_node *list, Expression_node *expression)
{
    list->Last->Next = expression;
    list->Last = expression;
    return list;
}

// ---------- receiver ----------

Receiver_node* Receiver_node::createReceiverNode(receiver_type type, char *name)
{
    Receiver_node *res = new Receiver_node;
    res->id = maxId++;
    res->Type = type;
    res->name = name;
    return res;
}

Receiver_node* Receiver_node::createReceiverNodeFromMessageExpression(Receiver_node *receiver, Message_selector_node *arguments)
{
    Receiver_node *res = new Receiver_node;
    res->id = maxId++;
    res->Type = MESSAGE_EXPRESSION_RECEIVER_TYPE;
    res->Receiver = receiver;
    res->Arguments = arguments;
    return res;
}

// ---------- message_selector ----------

Message_selector_node* Message_selector_node::createMessageSelectorNode(char *methodName,Expression_node *expression, Keyword_argument_list_node *arguments, Expression_list_node *exprArguments)
{
    Message_selector_node *res = new Message_selector_node;
    res->id = maxId++;
    res->MethodName = methodName;
    res->Arguments = arguments;
    res->Expression = expression;
    res->ExprArguments = exprArguments;
    return res;
}

// ---------- keyword_argument_list ----------

Keyword_argument_list_node* Keyword_argument_list_node::createKeywordArgumentListNode(Keyword_argument_node *argument)
{
    Keyword_argument_list_node *res = new Keyword_argument_list_node;
    res->id = maxId++;
    res->First = argument;
    res->Last = argument;
    return res;
}

Keyword_argument_list_node* Keyword_argument_list_node::addToKeywordArgumentListNode(Keyword_argument_list_node *list, Keyword_argument_node *argument)
{
    list->Last->Next = argument;
    list->Last = argument;
    return list;
}

// ---------- keyword_argument ----------

Keyword_argument_node* Keyword_argument_node::createKeywordArgumentNode(keyword_argument_type type, char *identifier, Expression_node *expression)
{
    Keyword_argument_node *res = new Keyword_argument_node;
    res->id = maxId++;
    res->type = type;
    res->name = identifier;
    res->expression = expression;
    res->Next = NULL;
    return res;
}

// -------------------- Управляющие структуры: развилки --------------------

// ---------- if_statement ----------

If_statement_node* If_statement_node::createIfStatementNode(if_type type, Expression_node *condition, Statement_node *trueBranch, Statement_node *falseBranch)
{
    If_statement_node *res = new If_statement_node;
    res->id = maxId++;
    res->IfType = type;
    res->type = IF_STATEMENT_TYPE;
    res->Condition = condition;
    res->TrueBranch = trueBranch;
    res->FalseBranch = falseBranch;
    return res;
}
