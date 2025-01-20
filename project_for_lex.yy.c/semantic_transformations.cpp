#include "tables.h"
#include <string>
extern long maxId;

void Statement_node::semanticTransform(LocalVariablesTable* locals, ConstantsTable *constants)
{
	if (type == SIMPLE_STATEMENT_TYPE) {
		Expression->assignmentTransform();
		Expression->setDataTypesAndCasts(locals);
		Expression->setAttributes(locals, constants);
	}
	else if (type == RETURN_STATEMENT_TYPE) {
		if (Expression != NULL) {
			Expression->assignmentTransform();
			Expression->setDataTypesAndCasts(locals);
			Expression->setAttributes(locals, constants);
		}
	}
	else if (type == IF_STATEMENT_TYPE) {
		If_statement_node* cur = (If_statement_node*)this;
		cur->Condition->assignmentTransform();
		cur->Condition->setDataTypesAndCasts(locals);
		cur->Condition->setAttributes(locals, constants);
		if (cur->TrueBranch != NULL)
			cur->TrueBranch->semanticTransform(locals, constants);
		if (cur->FalseBranch != NULL) {
			cur->FalseBranch->semanticTransform(locals, constants);
		}
	}
	else if (type == WHILE_STATEMENT_TYPE) {
		While_statement_node* cur = (While_statement_node*)this;
		cur->LoopCondition->assignmentTransform();
		cur->LoopCondition->setDataTypesAndCasts(locals);
		cur->LoopCondition->setAttributes(locals, constants);
		if (cur->LoopBody != NULL)
			cur->LoopBody->semanticTransform(locals, constants);
	}
	else if (type == FOR_STATEMENT_TYPE) {
		For_statement_node* cur = (For_statement_node*)this;
		if (cur->InitExpression != NULL) {
			cur->InitExpression->assignmentTransform();
			cur->InitExpression->setDataTypesAndCasts(locals);
			cur->InitExpression->setAttributes(locals, constants);
		}
		if (cur->ConditionExpression != NULL) {
			cur->ConditionExpression->assignmentTransform();
			cur->ConditionExpression->setDataTypesAndCasts(locals);
			cur->ConditionExpression->setAttributes(locals, constants);
		}
		if (cur->LoopExpression != NULL) {
			cur->LoopExpression->assignmentTransform();
			cur->LoopExpression->setDataTypesAndCasts(locals);
			cur->LoopExpression->setAttributes(locals, constants);
		}
		if (cur->InitList != NULL)
			cur->InitList->semanticTransform(locals, cur->NameType->toDataType(), constants);
		if (cur->LoopBody != NULL)
			cur->LoopBody->semanticTransform(locals, constants);

		if (cur->ForType == FOREACH_FOR_TYPE || cur->ForType == FOREACH_WITH_DECLARATION_FOR_TYPE) {
			cur->checkFastEnumerationTypes(locals);
			locals->findOrAddLocalVariable("<iterator" + to_string(cur->id) + ">", new Type(INT_TYPE));
			locals->findOrAddLocalVariable("<array" + to_string(cur->id) + ">", cur->ConditionExpression->DataType);
			constants->findOrAddMethodRefConstant("rtl/NSArray", "countDynamic", "()I");
			constants->findOrAddMethodRefConstant("rtl/NSArray", "objectAtIndexDynamic", "(I)Lrtl/NSObject;");
			constants->findOrAddMethodRefConstant("rtl/NSArray", "arrayWithArrayStatic", "(Lrtl/NSArray;)Lrtl/NSArray;");
		}
	}
	else if (type == DO_WHILE_STATEMENT_TYPE) {
		Do_while_statement_node* cur = (Do_while_statement_node*)this;
		cur->LoopCondition->assignmentTransform();
		cur->LoopCondition->setDataTypesAndCasts(locals);
		cur->LoopCondition->setAttributes(locals, constants);
		if (cur->LoopBody != NULL)
			cur->LoopBody->semanticTransform(locals, constants);
	}
	else if (type == COMPOUND_STATEMENT_TYPE) {
		Statement_list_node* cur = (Statement_list_node*)this;
		if (cur->First != NULL) {
			Statement_node* elem = cur->First;
			while (elem != NULL) {
				elem->semanticTransform(locals, constants);
				elem = elem->Next;
			}
		}
	}
	else if (type == DECLARATION_STATEMENT_TYPE) {
		Declaration_node* cur = (Declaration_node*)this;
		if (cur->init_declarator_list != NULL)
			cur->init_declarator_list->semanticTransform(locals, cur->typeNode->toDataType(), constants);
	}
}

void Init_declarator_list_node::semanticTransform(LocalVariablesTable *locals, Type *dataType, ConstantsTable *constants)
{
	Init_declarator_node* declarator = First;
	while (declarator != NULL)
	{
		declarator->semanticTransform(locals, dataType, constants);
		declarator = declarator->Next;
	}
}

void Init_declarator_node::semanticTransform(LocalVariablesTable* locals, Type *dataType, ConstantsTable *constants)
{
	if (expression != NULL) {
		expression->assignmentTransform();
		expression->setDataTypesAndCasts(locals);
		expression->setAttributes(locals, constants);

		if (type == ARRAY_WITH_INITIALIZING_DECLARATOR_TYPE) {
			if (dataType->DataType != CHAR_TYPE || expression->type != LITERAL_EXPRESSION_TYPE) {
				string msg = string("Type '") + expression->DataType->toString() + "' can't assignment to '" + dataType->toString() + "' in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
		}

		if (!expression->DataType->equal(dataType) && !(type == ARRAY_WITH_INITIALIZING_DECLARATOR_TYPE && expression->DataType->DataType == dataType->DataType && expression->DataType->ClassName == dataType->ClassName)) {
			if (expression->DataType->isCastableTo(dataType)) {
				Expression_node* cast = new Expression_node();
				cast->id = maxId++;
				if (dataType->DataType == INT_TYPE) {
					cast->type = INT_CAST;
				}
				else if (dataType->DataType == CHAR_TYPE) {
					cast->type = CHAR_CAST;
				}
				else {
					cast->type = CLASS_CAST;
				}
				cast->DataType = dataType;
				cast->Right = expression;
				expression = cast;
			}
			else {
				string msg = string("Type '") + expression->DataType->toString() + "' doesn't castable to type '" + dataType->toString() + "' in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
		}
	}
	if (InitializerList != NULL) {
		InitializerList->assignmentTransform();
		InitializerList->setDataTypesAndCasts(locals);
		InitializerList->setAttributes(locals, constants);
	}
	if (ArraySize != NULL) {
		ArraySize->assignmentTransform();
		ArraySize->setDataTypesAndCasts(locals);
		ArraySize->setAttributes(locals, constants);
		Type* castType = new Type(INT_TYPE);
		if (!ArraySize->DataType->isCastableTo(castType)) {
			string msg = "Array size isn't 'int' or castable to 'int'. It has type '" + ArraySize->DataType->toString() + "' in line: " + to_string(line);
			throw new std::exception(msg.c_str());
		}
		delete castType;
	}
}


// -------------------- ASSIGNMENT TRANSFORMATIONS --------------------

void Expression_node::assignmentTransform()
{
	arrayAssignmentTransform();
	memberAccessAssignmentTransform();
}

void Expression_node::arrayAssignmentTransform()
{
	if (this->type == ASSIGNMENT_EXPRESSION_TYPE && this->Left->type == ARRAY_ELEMENT_ACCESS_EXPRESSION_TYPE) {
		this->type = ARRAY_ASSIGNMENT_EXPRESSION_TYPE; 
		
		Expression_node* tmp = this->Left;
		this->Left = tmp->Left; //Левая часть от expr массива
		this->Child = tmp->Right; //Правая часть от expr массива
		delete tmp;
	}
	
	// Вызов преобразования на дочерних
	if (this->Left != NULL)
		this->Left->assignmentTransform();
	if (this->Right != NULL)
		this->Right->assignmentTransform();
	if (this->Child != NULL)
		this->Child->assignmentTransform();
}

void Expression_node::memberAccessAssignmentTransform()
{
	if (this->type == ASSIGNMENT_EXPRESSION_TYPE && (this->Left->type == DOT_EXPRESSION_TYPE || this->Left->type == ARROW_EXPRESSION_TYPE)) {
		this->type = MEMBER_ACCESS_ASSIGNMENT_EXPRESSION_TYPE;

		Expression_node* tmp = this->Left;
		this->Left = tmp->Left; //Левая часть от expr dot
		this->name = tmp->name; //Правая часть от expr dot (identifier)
		this->Constant = tmp->Constant;
		this->Field = tmp->Field;
		delete tmp;
	}

	// Вызов преобразования на дочерних
	if (this->Left != NULL)
		this->Left->assignmentTransform();
	if (this->Right != NULL)
		this->Right->assignmentTransform();
	if (this->Child != NULL)
		this->Child->assignmentTransform();
}

void Expression_list_node::assignmentTransform()
{
	Expression_node* cur = First;
	while (cur != NULL)
	{
		cur->assignmentTransform();
		cur = cur->Next;
	}
}


// -------------------- ADD DEFAULT RETURN --------------------
void MethodsTableElement::addDefaultReturn(Statement_node* lastStatement)
{
	Statement_node* defaultReturn;
	if (ReturnType->DataType == VOID_TYPE)
		defaultReturn = Statement_node::createStatementNodeFromSimpleStatement(RETURN_STATEMENT_TYPE, NULL); //Создать пустой return
	else {
		int defaultValue = ReturnType->getDefaultValue(); //Получить значение по умолчанию
		Numeric_constant_node *num = Numeric_constant_node::createNumericConstantNodeFromInteger(defaultValue); //Сформировать константу
		Expression_node* expr = Expression_node::createExpressionNodeFromNumericConstant(num); //Сформировать expression
		defaultReturn = Statement_node::createStatementNodeFromSimpleStatement(RETURN_STATEMENT_TYPE, expr); //Сформировать statement с return
	}
	
	lastStatement->Next = defaultReturn;
}

void FunctionsTableElement::addDefaultReturn(Statement_node* lastStatement)
{
	Statement_node* defaultReturn;
	if (ReturnType->DataType == VOID_TYPE || NameStr == "main")
		defaultReturn = Statement_node::createStatementNodeFromSimpleStatement(RETURN_STATEMENT_TYPE, NULL); //Создать пустой return
	else {
		int defaultValue = ReturnType->getDefaultValue(); //Получить значение по умолчанию
		Numeric_constant_node* num = Numeric_constant_node::createNumericConstantNodeFromInteger(defaultValue); //Сформировать константу
		Expression_node* expr = Expression_node::createExpressionNodeFromNumericConstant(num); //Сформировать expression
		defaultReturn = Statement_node::createStatementNodeFromSimpleStatement(RETURN_STATEMENT_TYPE, expr); //Сформировать statement с return
	}

	lastStatement->Next = defaultReturn;
}
}
