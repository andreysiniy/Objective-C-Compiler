#include "tables.h"
#include <algorithm>
#include <string>
// ------------ Method_declaration_node ------------
string Method_declaration_node::getMethod(Type** returnType, vector<string>* keywordsNames, vector<Type*> *keywordsTypes, vector<string>* parametersNames, vector<Type*> *parametersTypes, bool *isClassmethod)
{
	*returnType = MethodType->toDataType(); // Тип возвращаемого значения
	string methodName = string(MethodSelector->MethodName); // Имя метода
	// Тип метода
	if (type == CLASS_METHOD_DECLARATION_TYPE) {
		*isClassmethod = true;
		methodName += "Static";
	}
	else if (type == INSTANCE_METHOD_DECLARATION_TYPE) {
		*isClassmethod = false;
		methodName += "Dynamic";
	}
	MethodSelector->getParams(keywordsNames, keywordsTypes, parametersNames, parametersTypes); // Параметры
	strcpy(MethodSelector->MethodName, methodName.c_str()); //Преобразование имени метода в узле дерева
	return methodName;
}

// ---------- Method_definition_node ----------
string Method_definition_node::getMethod(Type** returnType, vector<string>* keywordsNames, vector<Type*>* keywordsTypes, vector<string>* parametersNames, vector<Type*>* parametersTypes, bool* isClassmethod, Statement_node** bodyStart)
{
	if (MethodType != NULL)
		*returnType = MethodType->toDataType(); // Тип возвращаемого значения
	else {
		MethodType = Type_node::createTypeNode(ID_TYPE); // Создать вершину возвращаемого значения
		*returnType = new Type(ID_TYPE); // Тип возвращаемого значения по умолчанию
	}
	string methodName = string(MethodSelector->MethodName); // Имя метода
	// Тип метода
	if (type == CLASS_METHOD_DECLARATION_TYPE) {
		*isClassmethod = true;
		methodName += "Static";
	}
	else if (type == INSTANCE_METHOD_DECLARATION_TYPE) {
		*isClassmethod = false;
		methodName += "Dynamic";
	}
	MethodSelector->getParams(keywordsNames, keywordsTypes, parametersNames, parametersTypes); // Параметры
	if (MethodBody != NULL)
		*bodyStart = MethodBody->First; // Начало тела метода
	else
		*bodyStart = NULL;

	strcpy(MethodSelector->MethodName, methodName.c_str()); //Преобразование имени метода вузле дерева
	return methodName;
}


// ---------- Type_node ----------
Type* Type_node::toDataType()
{
	if (type == CLASS_NAME_TYPE)
	{
		string className = ClassesTable::getFullClassName(string(ClassName));
		strcpy(ClassName, className.c_str()); //Преобразование имени класа в узле дерева
		Type* res;
		if (isArray) {
			res = new Type(type, className, 1024);
		}
		else {
			res = new Type(type, className);
		}
		return res;
	}
	else
	{
		Type* res;
		if (isArray) {
			res = new Type(type, 1024);
		}
		else {
			res = new Type(type);
		}
		return res;
	}
}

// ---------- Method_selector_node ----------
void Method_selector_node::getParams(vector<string>* keywordsNames, vector<Type*>* keywordsTypes, vector<string>* parametersNames, vector<Type*> *parametersTypes)
{
	if (KeywordDeclaration != NULL) { //Добавление первого параметра
		string firstKeywordName = string(KeywordDeclaration->KeywordName); // 
		Type* firstKeywordType = KeywordDeclaration->KeywordType->toDataType();
		keywordsNames->push_back(firstKeywordName);
		keywordsTypes->push_back(firstKeywordType);
	}

	if (KeywordSelector != NULL) { //Добавление остальных параметров
		vector<Keyword_declaration_node*>* keywordsList = KeywordSelector->getElements(); //Список keword
		for (auto it = keywordsList->cbegin(); it < keywordsList->cend(); it++)
		{
			Keyword_declaration_node* keyword = *it; //Ключевое слово

			if (keyword->Identifier != NULL) {
					string msg = "ERROR! Unsupported identifiers in keyword: '" + string(keyword->Identifier) + "' in method '" + MethodName + "'";
					throw new std::exception(msg.c_str());
			}

			string name = string(keyword->KeywordName); //Имя
			Type* type = keyword->KeywordType->toDataType(); //Тип
			if (std::find(keywordsNames->begin(), keywordsNames->end(), name) != keywordsNames->end()) { //Переопределение параметра
				string msg = "Parameter '" + name + "' redifinition in method '" + string(MethodName) + "'";
				throw new std::exception(msg.c_str());
			}
			keywordsNames->push_back(name); //Добавление имени
			keywordsTypes->push_back(type); //Добавление типа
		}
	}

	if (ParameterListNode != NULL) { //Добавление параметров
		vector<Parameter_declaration_node*>* parametersList = ParameterListNode->getElements(); //Список параметров
		for (auto it = parametersList->cbegin(); it < parametersList->cend(); it++)
		{
			Parameter_declaration_node* parameter = *it; //Параметр
			string name = string(parameter->name); //Имя
			Type* type = parameter->type->toDataType(); //Тип
			if (std::find(parametersNames->begin(), parametersNames->end(), name) != parametersNames->end() || std::find(keywordsNames->begin(), keywordsNames->end(), name) != keywordsNames->end()) { //Переопределение параметра
				string msg = "Parameter '" + name + "' redifinition in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
			parametersNames->push_back(name); //Добавление имени
			parametersTypes->push_back(type); //Добавление типа
		}
	}
}

// ---------- Statement_node ----------
void Statement_node::findLocalVariables(vector<string>* localVariablesNames, vector<Type*>* localVariablesTypes, ClassesTableElement* classElem, vector<string> keywordNames, vector<string> parameterNames, bool isInTopFunctionLevel)
{
	vector<Init_declarator_node*>* declarators = NULL; //Деклараторы
	Type_node* typeNode = NULL;
	if (type == SIMPLE_STATEMENT_TYPE || type == RETURN_STATEMENT_TYPE) {
		if (Expression != NULL) {
			Expression->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
		}
	}
	// Получение деклараторов
	if (type == DECLARATION_STATEMENT_TYPE)
	{
		Declaration_node* declaration = (Declaration_node*)this;
		if (isInTopFunctionLevel) {
			declarators = declaration->init_declarator_list->getElements();
			typeNode = declaration->typeNode;
			Init_declarator_node* decl = declaration->init_declarator_list->First;
			while (decl != NULL) {
				if (decl->expression != NULL)
					decl->expression->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
				if (decl->ArraySize != NULL)
					decl->ArraySize->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
				if (decl->InitializerList != NULL)
					decl->InitializerList->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
				decl = decl->Next;
			}
			if (declarators != NULL && typeNode != NULL)
				getTypesFromInitDeclaratorType(declarators, typeNode, localVariablesNames, localVariablesTypes); // Получение переменных
		}
		else {
			string msg = "Error! Unsupported scope. Declaration variables " + declaration->init_declarator_list->getNames() + " In line " + to_string(declaration->line);
			throw new std::exception(msg.c_str());
		}
		
	}
	if (type == FOR_STATEMENT_TYPE)
	{
		For_statement_node* for_stmt = (For_statement_node*)this;
		if (for_stmt->InitList != NULL) {
			declarators = for_stmt->InitList->getElements();
			typeNode = for_stmt->NameType;
			Init_declarator_node* decl = for_stmt->InitList->First;
			if (declarators != NULL && typeNode != NULL)
				getTypesFromInitDeclaratorType(declarators, typeNode, localVariablesNames, localVariablesTypes); // Получение переменных
			while (decl != NULL) {
				if (decl->expression != NULL)
					decl->expression->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
				if (decl->ArraySize != NULL)
					decl->ArraySize->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
				if (decl->InitializerList != NULL)
					decl->InitializerList->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
				decl = decl->Next;
			}
		}
		if (for_stmt->InitExpression != NULL) {
			for_stmt->InitExpression->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
		}
		if (for_stmt->ConditionExpression != NULL) {
			for_stmt->ConditionExpression->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
		}
		if (for_stmt->LoopExpression != NULL) {
			for_stmt->LoopExpression->checkDeclarated(*localVariablesNames, classElem, keywordNames, parameterNames);
		}
		if (for_stmt->ForType == FOREACH_WITH_DECLARATION_FOR_TYPE) {
			Init_declarator_node *dec = Init_declarator_node::createInitDeclaratorNode(SIMPLE_DECLARATOR_TYPE, for_stmt->name, NULL);
			declarators = new vector<Init_declarator_node*>;
			declarators->push_back(dec);
			typeNode = for_stmt->NameType;
			if (declarators != NULL && typeNode != NULL)
				getTypesFromInitDeclaratorType(declarators, typeNode, localVariablesNames, localVariablesTypes); // Получение переменных
		}
		
		if (for_stmt->LoopBody != NULL) {
			for_stmt->LoopBody->findLocalVariables(localVariablesNames, localVariablesTypes, classElem, keywordNames, parameterNames, false);
		}
	}
	if (type == WHILE_STATEMENT_TYPE) {
		While_statement_node* while_stmt = (While_statement_node*)this;
		if (while_stmt->LoopBody != NULL) {
			while_stmt->LoopBody->findLocalVariables(localVariablesNames, localVariablesTypes, classElem, keywordNames, parameterNames, false);
		}
	}
	if (type == DO_WHILE_STATEMENT_TYPE) {
		Do_while_statement_node* do_while_stmt = (Do_while_statement_node*)this;
		if (do_while_stmt->LoopBody != NULL) {
			do_while_stmt->LoopBody->findLocalVariables(localVariablesNames, localVariablesTypes, classElem, keywordNames, parameterNames, false);
		}
	}
	if (type == IF_STATEMENT_TYPE) {
		If_statement_node* if_stmt = (If_statement_node*)this;
		if (if_stmt->TrueBranch != NULL) {
			if_stmt->TrueBranch->findLocalVariables(localVariablesNames, localVariablesTypes, classElem, keywordNames, parameterNames,false);
		}
		if (if_stmt->FalseBranch != NULL) {
			if_stmt->FalseBranch->findLocalVariables(localVariablesNames, localVariablesTypes, classElem, keywordNames, parameterNames, false);
		}
	}
	if (type == COMPOUND_STATEMENT_TYPE) {
		Statement_node* cur = ((Statement_list_node*)this)->First;
		cur->findLocalVariables(localVariablesNames, localVariablesTypes, classElem, keywordNames, parameterNames, false);
	}


	if (Next != NULL)
	{
		Next->findLocalVariables(localVariablesNames, localVariablesTypes, classElem, keywordNames, parameterNames, isInTopFunctionLevel);
	}
}

void getTypesFromInitDeclaratorType(vector<Init_declarator_node*>* declarators, Type_node* typeNode, vector<string>* varsNames, vector<Type*>* varsTypes)
{
	for (auto it = declarators->begin(); it < declarators->end(); it++)
	{
		string name = string((*it)->Declarator); //Имя
		type_type type = typeNode->type; //Тип
		Expression_node* arrSize = (*it)->ArraySize; //Размер массива
		Expression_node* initializer = (*it)->expression; // Инициализатор
		Expression_list_node* initializerList = (*it)->InitializerList; // Инициализатор массива
		bool isArr = (*it)->isArray;
		if (type == CLASS_NAME_TYPE)
		{
			string className = ClassesTable::getFullClassName(string(typeNode->ClassName));
			strcpy(typeNode->ClassName, className.c_str()); //Преобразование имени класса в узле дерева
			if (arrSize != NULL || initializerList != NULL)
			{ // Массив типа класса
				int arraySize;
				Type* curType;
				if (arrSize != NULL)
				{
					curType = new Type(type, className, arrSize);
				}
				else
				{
					arraySize = initializerList->getElements()->size();
					curType = new Type(type, className, arraySize);
				}
				
				if (std::find(varsNames->begin(), varsNames->end(), name) != varsNames->end() && initializerList != NULL) {
					string msg = "Variable '" + name + "' redifinition in line: " + to_string((*it)->line);
					throw new std::exception(msg.c_str());
				}
				varsNames->push_back(name);
				varsTypes->push_back(curType);
				
			}
			else
			{ //Тип класса
				Type* curType = new Type(type, className);
				if (std::find(varsNames->begin(), varsNames->end(), name) != varsNames->end() && initializer != NULL) {
					string msg = "Variable '" + name + "' redifinition in line: " + to_string((*it)->line);
					throw new std::exception(msg.c_str());
				}
				varsNames->push_back(name);
				varsTypes->push_back(curType);
			}
		}
		else
		{
			if (isArr)
			{ // Массив примитивного типа
				Type* curType;
				int arraySize;
				if (arrSize != NULL)
				{
					curType = new Type(type, arrSize);
				}
				else
				{
					if (initializer != NULL && initializer->type == LITERAL_EXPRESSION_TYPE) {
						if (initializer->literal->type == STRING_CONSTANT_TYPE) {
							arraySize = strlen(initializer->literal->value) + 1;
						}
					}
					else if (initializerList != NULL){
						arraySize = initializerList->getElements()->size();
					}
					curType = new Type(type, arraySize);
				}
				
				if (std::find(varsNames->begin(), varsNames->end(), name) != varsNames->end() && initializerList != NULL) {
					string msg = "Variable '" + name + "' redifinition in line: " + to_string((*it)->line);
					throw new std::exception(msg.c_str());
				}
				varsNames->push_back(name);
				varsTypes->push_back(curType);
			}
			else
			{ //Примитивный тип
				Type* curType = new Type(type);
				if (std::find(varsNames->begin(), varsNames->end(), name) != varsNames->end() && initializer != NULL) {
					string msg = "Variable '" + name + "' redifinition in line: " + to_string((*it)->line);
					throw new std::exception(msg.c_str());
				}
				varsNames->push_back(name);
				varsTypes->push_back(curType);
			}
		}
	}
}

void Statement_node::fillFieldRefs(ConstantsTable *constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	if (type == EMPTY_STATEMENT_TYPE) {

	}
	else if (type == SIMPLE_STATEMENT_TYPE) {
		if (Expression != NULL) {
			Expression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		}
	}
	else if (type == RETURN_STATEMENT_TYPE) {
		if (Expression != NULL) {
			Expression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		}
	}
	else if (type == IF_STATEMENT_TYPE) {
		If_statement_node *cur = (If_statement_node*)this;
		cur->Condition->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if (cur->TrueBranch != NULL)
			cur->TrueBranch->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if (cur->FalseBranch != NULL)
			cur->FalseBranch->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	else if (type == WHILE_STATEMENT_TYPE) {
		While_statement_node *cur = (While_statement_node*)this;
		cur->LoopCondition->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if (cur->LoopBody != NULL)
			cur->LoopBody->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	else if (type == DO_WHILE_STATEMENT_TYPE) {
		Do_while_statement_node *cur = (Do_while_statement_node*)this;
		cur->LoopCondition->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if (cur->LoopBody != NULL)
			cur->LoopBody->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	else if (type == FOR_STATEMENT_TYPE) {
		For_statement_node *cur = (For_statement_node*)this;
		if (cur->InitExpression != NULL)
			cur->InitExpression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if (cur->ConditionExpression != NULL)
			cur->ConditionExpression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if (cur->LoopExpression != NULL)
			cur->LoopExpression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if (cur->InitList != NULL)
			cur->InitList->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if (cur->LoopBody != NULL)
			cur->LoopBody->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	else if (type == COMPOUND_STATEMENT_TYPE) {
		Statement_list_node *cur = (Statement_list_node*)this;
		if (cur->First != NULL) {
			Statement_node *elem = cur->First;
			while (elem != NULL) {
				elem->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
				elem = elem->Next;
			}
		}
	}
	else if (type == DECLARATION_STATEMENT_TYPE) {
		Declaration_node *cur = (Declaration_node*)this;
		if (cur->init_declarator_list != NULL)
			cur->init_declarator_list->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
}

void Statement_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	if (type == EMPTY_STATEMENT_TYPE) {

	}
	else if (type == SIMPLE_STATEMENT_TYPE) {
		if (Expression != NULL)
			Expression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	else if (type == RETURN_STATEMENT_TYPE) {
		if (Expression != NULL)
			Expression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	else if (type == IF_STATEMENT_TYPE) {
		If_statement_node* cur = (If_statement_node*)this;
		cur->Condition->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		if (cur->TrueBranch != NULL)
			cur->TrueBranch->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		if (cur->FalseBranch != NULL)
			cur->FalseBranch->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	else if (type == WHILE_STATEMENT_TYPE) {
		While_statement_node* cur = (While_statement_node*)this;
		cur->LoopCondition->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		if (cur->LoopBody != NULL)
			cur->LoopBody->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	else if (type == DO_WHILE_STATEMENT_TYPE) {
		Do_while_statement_node* cur = (Do_while_statement_node*)this;
		cur->LoopCondition->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		if (cur->LoopBody != NULL)
			cur->LoopBody->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	else if (type == FOR_STATEMENT_TYPE) {
		For_statement_node* cur = (For_statement_node*)this;
		if (cur->InitExpression != NULL)
			cur->InitExpression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		if (cur->ConditionExpression != NULL)
			cur->ConditionExpression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		if (cur->LoopExpression != NULL)
			cur->LoopExpression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		if (cur->InitList != NULL)
			cur->InitList->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		if (cur->LoopBody != NULL)
			cur->LoopBody->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	else if (type == COMPOUND_STATEMENT_TYPE) {
		Statement_list_node* cur = (Statement_list_node*)this;
		if (cur->First != NULL) {
			Statement_node* elem = cur->First;
			while (elem != NULL) {
				elem->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
				elem = elem->Next;
			}
		}
	}
	else if (type == DECLARATION_STATEMENT_TYPE) {
		Declaration_node* cur = (Declaration_node*)this;
		if (cur->init_declarator_list != NULL)
			cur->init_declarator_list->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
}

void Statement_node::fillLiterals(ConstantsTable* constantTable)
{
	if (type == EMPTY_STATEMENT_TYPE) {

	}
	else if (type == SIMPLE_STATEMENT_TYPE) {
		if (Expression != NULL)
			Expression->fillLiterals(constantTable);
	}
	else if (type == RETURN_STATEMENT_TYPE) {
		if (Expression != NULL)
			Expression->fillLiterals(constantTable);
	}
	else if (type == IF_STATEMENT_TYPE) {
		If_statement_node* cur = (If_statement_node*)this;
		cur->Condition->fillLiterals(constantTable);
		if (cur->TrueBranch != NULL)
			cur->TrueBranch->fillLiterals(constantTable);
		if (cur->FalseBranch != NULL) {
			cur->FalseBranch->fillLiterals(constantTable);
		}
	}
	else if (type == WHILE_STATEMENT_TYPE) {
		While_statement_node* cur = (While_statement_node*)this;
		cur->LoopCondition->fillLiterals(constantTable);
		if (cur->LoopBody != NULL) {
			cur->LoopBody->fillLiterals(constantTable);
		}
	}
	else if (type == DO_WHILE_STATEMENT_TYPE) {
		Do_while_statement_node* cur = (Do_while_statement_node*)this;
		cur->LoopCondition->fillLiterals(constantTable);
		if (cur->LoopBody != NULL) {
			cur->LoopBody->fillLiterals(constantTable);
		}
	}
	else if (type == FOR_STATEMENT_TYPE) {
		For_statement_node* cur = (For_statement_node*)this;
		if (cur->InitExpression != NULL)
			cur->InitExpression->fillLiterals(constantTable);
		if (cur->ConditionExpression != NULL)
			cur->ConditionExpression->fillLiterals(constantTable);
		if (cur->LoopExpression != NULL)
			cur->LoopExpression->fillLiterals(constantTable);
		if (cur->InitList != NULL)
			cur->InitList->fillLiterals(constantTable);
		if (cur->LoopBody != NULL)
			cur->LoopBody->fillLiterals(constantTable);
	}
	else if (type == COMPOUND_STATEMENT_TYPE) {
		Statement_list_node* cur = (Statement_list_node*)this;
		if (cur->First != NULL) {
			Statement_node* elem = cur->First;
			while (elem != NULL) {
				elem->fillLiterals(constantTable);
				elem = elem->Next;
			}
		}
	}
	else if (type == DECLARATION_STATEMENT_TYPE) {
		Declaration_node* cur = (Declaration_node*)this;
		if (cur->init_declarator_list != NULL)
			cur->init_declarator_list->fillLiterals(constantTable);
	}
}

// ---------- Function_declaration ----------
string Function_node::getFunction(Type** returnType, Statement_node** bodyStart)
{
	*returnType = ReturnType->toDataType(); //Тип возвращаемого значения
	string functionName = string(Name); //Имя функции
	*bodyStart = statement->First; // Начало тела функции
	return functionName;
}

// ---------- Expression_node ----------
void Expression_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable *localVariablesTable, ClassesTableElement *classTableElement)
{
	checkLvalueError();

	if (type == IDENTIFIER_EXPRESSION_TYPE) {
		if (!localVariablesTable->isContains(name) && !classTableElement->isContainsField(name)) {
			string msg = "Variable '" + string(name) + "' not declarated in line: " + to_string(line);
			throw new std::exception(msg.c_str());
		}
		else if (!localVariablesTable->isContains(name) && classTableElement->isContainsField(name)) {
			string descriptor; //Строка дескриптора
			string className; //Имя класса
			classTableElement->getFieldForRef(name, &descriptor, &className); //Получение дескриптора и имени класса
			//Формирование fieldRef
			Constant = constantTable->findOrAddFieldRefConstant(className, name, descriptor);
		}
	}
	else if (type == SELF_EXPRESSION_TYPE) {

	}
	else if (type == SUPER_EXPRESSION_TYPE) {
		string msg = "Can not call field from super object in line: " + to_string(line);
		throw new std::exception(msg.c_str());
	}
	if (type == ARROW_EXPRESSION_TYPE) {
		string objName;
		// Получение имени объекта и проверка его корректности
		if (Left->type == IDENTIFIER_EXPRESSION_TYPE)
			objName = Left->name; //Имя объекта
		else if (Left->type == SELF_EXPRESSION_TYPE)
			objName = "self";
		else if (Left->type == SUPER_EXPRESSION_TYPE) {
			string msg = "Can not call field from super object in line: " + to_string(line);
			throw new std::exception(msg.c_str());
		}
		else {
			string msg = "Invalid expression type in field call. in line: " + to_string(line);
			throw new std::exception(msg.c_str());
		}


		if (name == NULL) {
			string msg = "Field call not to field in line: " + to_string(line);
			throw new std::exception(msg.c_str());
		}


		string fieldName = name; //Имя поля
		if (!localVariablesTable->isContains(objName) && !classTableElement->isContainsField(objName)) { //Не является локальной переменной или полем класса
			string msg = "Variable '" + string(objName) + "' not declarated in line: " + to_string(line);
			throw new std::exception(msg.c_str());
		}
		else if (classTableElement->isContainsField(objName) && !localVariablesTable->isContains(objName)) {
			FieldsTableElement* field = classTableElement->Fields->items[objName]; //Локальная переменная
			if (field->type->DataType != CLASS_NAME_TYPE) { // Не является экземпляром класса
				string msg = "Variable '" + objName + "' is not instance of object. in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
			string className = field->type->ClassName; //Имя класса локальной переменной
			ClassesTableElement* classElem = ClassesTable::items->at(className);
			if (!classElem->isContainsField(fieldName) || classElem->Fields->items[fieldName]->IsInstance == false) { //Поле не содержится в классе объекта
				string msg = "Class '" + className + "' don't contains field '" + fieldName + "' in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
			if (classElem != classTableElement && classTableElement->getClassName() != "rtl/!Program!" && !classTableElement->isHaveOneOfSuperclass(className)) {
				// Поле является protected и не выполнено условие для возможности использования protected
				string msg = "Variable '" + fieldName + "' only protected in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}


			string descriptor; // дескриптор
			string fieldClassName;  // Имя класса поля
			classElem->getFieldForRef(fieldName, &descriptor, &fieldClassName); // получение данных для field ref
			//Формирование fieldRef
			Constant = constantTable->findOrAddFieldRefConstant(fieldClassName, fieldName, descriptor);
		}
		else { //Является локальной переменной
			LocalVariablesTableElement* local = localVariablesTable->items[objName]; //Локальная переменная
			if (local->type->DataType != CLASS_NAME_TYPE) { // Не является экземпляром класса
				string msg = "Variable '" + objName + "' is not instance of object. in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
			string className = local->type->ClassName; //Имя класса локальной переменной
			ClassesTableElement* classElem = ClassesTable::items->at(className);
			if (!classElem->isContainsField(fieldName) || classElem->Fields->items[fieldName]->IsInstance == false) { //Поле не содержится в классе объекта
				string msg = "Class '" + className + "' don't contains field '" + fieldName + "' in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
			if (classElem != classTableElement && classTableElement->getClassName() != "rtl/!Program!" && !classTableElement->isHaveOneOfSuperclass(className)) {
				// Поле является protected и не выполнено условие для возможности использования protected
				string msg = "Variable '" + fieldName + "' only protected in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}


			string descriptor; // дескриптор
			string fieldClassName;  // Имя класса поля
			classElem->getFieldForRef(fieldName, &descriptor, &fieldClassName); // получение данных для field ref
			//Формирование fieldRef
 			Constant = constantTable->findOrAddFieldRefConstant(fieldClassName, fieldName, descriptor);
		}
		
	}
	if (Left != NULL) {
		Left->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	if (Right != NULL) {
		Right->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	if (Receiver != NULL) {
		Receiver->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	if (Arguments != NULL) {
		Arguments->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	if (ArgumentsList != NULL) {
		ArgumentsList->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
}

void Expression_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	if (type == MESSAGE_EXPRESSION_EXPRESSION_TYPE) { //Вызов метода
		string receiverName;
		bool isObject;
		bool isReceiver = false;
		if (Receiver->type == MESSAGE_EXPRESSION_RECEIVER_TYPE) {// Если ресивер имеет тип message_selector, то вызывать на дочерних элементах
			Type *returnType;
			Receiver->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod, &returnType);
			if (returnType->DataType != CLASS_NAME_TYPE) {
				string msg = "Receiver is not an object in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
			isObject = true;
			isReceiver = true;
			receiverName = returnType->ClassName;
		}
		else {
			isObject = Receiver->getName(&receiverName); // получить имя объекта/класса и его тип
			//преобразования для self и super
			if (receiverName == "self") {
				if (!isInInstanceMethod)
					receiverName = classTableElement->getClassName();
				isObject = isInInstanceMethod;
			}
			if (receiverName == "super") {
				if (!isInInstanceMethod)
					receiverName = classTableElement->getSuperClassName();
				isObject = isInInstanceMethod;
			}
		}


		// От message_selector получать имя метода.
		string methodName = Arguments->MethodName; // Имя метода
		string methodNameWithType; //Имя метода с dynamic или static
		
		if (isObject)
			methodNameWithType = methodName + "Dynamic";
		else
			methodNameWithType = methodName + "Static";

		strcpy(Arguments->MethodName, methodNameWithType.c_str()); // Преобразование имени метода в узле дерева

		// Проверить наличие метода в классе
		if (isReceiver) {
			bool isContainsMethod = ClassesTable::items->at(receiverName)->isContainsMethod(methodNameWithType);
			if (!isContainsMethod) {
				string msg = "Class '" + receiverName + "' don't contains method '-" + methodName + "'";
				throw new std::exception(msg.c_str());
			}
		}
		else {
			if (isObject) {
				if (!localVariablesTable->isContains(receiverName) && !classTableElement->isContainsField(receiverName) && receiverName != "super") {
					string msg = "Using undeclarated variable '" + receiverName + "' in line: " + to_string(line);
					throw new std::exception(msg.c_str());
				}
				else if (!localVariablesTable->isContains(receiverName) && classTableElement->isContainsField(receiverName) && receiverName != "super") { //Является полем класса
					FieldsTableElement* field = classTableElement->Fields->items[receiverName];
					if (field->type->DataType != CLASS_NAME_TYPE) {
						string msg = "Variable '" + receiverName + "' is not an object in line: " + to_string(line);
						throw new std::exception(msg.c_str());
					}
					string className = field->type->ClassName;
					bool isContainsMethod = ClassesTable::items->at(className)->isContainsMethod(methodNameWithType);
					if (!isContainsMethod) {
						string msg = "Class '" + className + "' don't contains method '-" + methodName + "'";
						throw new std::exception(msg.c_str());
					}
				}
				else if (receiverName != "super") { //Является локальной переменной
					LocalVariablesTableElement* local = localVariablesTable->items[receiverName];
					if (local->type->DataType != CLASS_NAME_TYPE) {
						string msg = "Variable '" + receiverName + "' is not an object in line: " + to_string(line);
						throw new std::exception(msg.c_str());
					}
					string className = local->type->ClassName;
					bool isContainsMethod = ClassesTable::items->at(className)->isContainsMethod(methodNameWithType);
					if (!isContainsMethod) {
						string msg = "Class '" + receiverName + "' don't contains method '-" + methodName + "'";
						throw new std::exception(msg.c_str());
					}
				}
				else { //Super
					string className = classTableElement->getSuperClassName();
					bool isContainsMethod = ClassesTable::items->at(className)->isContainsMethod(methodNameWithType);
					if (!isContainsMethod) {
						string msg = "Class '" + className + "' don't contains method '-" + methodName + "'";
						throw new std::exception(msg.c_str());
					}
				}
			}
			else {
				string fullClassName = ClassesTable::getFullClassName(receiverName); //Получить полное имя класса //TODO: Возможно, нужно будет убрать, если при преобразовании дерева будут заменяться сразу все имена классов
				if (Receiver->name != NULL)
					strcpy(Receiver->name, fullClassName.c_str());
				bool isContainsMethod = ClassesTable::items->at(fullClassName)->isContainsMethod(methodNameWithType); //Проверить наличие метода
				if (!isContainsMethod) {
					string msg = "Class '" + receiverName + "' don't contains method '+" + methodName + "'";
					throw new std::exception(msg.c_str());
				}
			}
		}

		// Добавить methodRef
		if (isReceiver) {
			ClassesTableElement* element = ClassesTable::items->at(receiverName);
			// Получить информацию для создания константы типа method ref
			string descriptor;
			string className;
			element->getMethodForRef(methodNameWithType, &descriptor, &className, Receiver->type == SUPER_RECEIVER_TYPE);
			// Добавить константу method ref
			if (methodNameWithType != "newStatic" && methodNameWithType != "allocStatic")
				Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
			else {
				if (element->Methods->items.count(methodNameWithType) == 0) {
					Constant = constantTable->findOrAddMethodRefConstant(className, "<init>", "()V");
					isInitMethod = true;
				}
				else
					Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
			}
		}
		else {
			if (isObject) {
				string className;
				if (receiverName == "super") {//Super
					className = classTableElement->getSuperClassName();
				}
				else if (!localVariablesTable->isContains(receiverName) && classTableElement->isContainsField(receiverName)) { //Является полем класса
					FieldsTableElement* field = classTableElement->Fields->items[receiverName];
					className = field->type->ClassName;
				}
				else { //Является локальной переменной
					LocalVariablesTableElement* local = localVariablesTable->items[receiverName];
					className = local->type->ClassName;
				}
				ClassesTableElement* element = ClassesTable::items->at(className);
				// Получить информацию для создания константы типа method ref
				string descriptor;
				element->getMethodForRef(methodNameWithType, &descriptor, &className, Receiver->type == SUPER_RECEIVER_TYPE);
				// Добавить константу method ref
				if (methodNameWithType != "newStatic" && methodNameWithType != "allocStatic")
					Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
				else
					if (element->Methods->items.count(methodNameWithType) == 0)
						Constant = constantTable->findOrAddMethodRefConstant(className, "<init>", "()V");
					else
						Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
			}
			else {
				string fullClassName = ClassesTable::getFullClassName(receiverName); //Получить полное имя класса //TODO: Возможно, нужно будет убрать, если при преобразовании дерева будут заменяться сразу все имена классов
				if (Receiver->name != NULL)
					strcpy(Receiver->name, fullClassName.c_str()); // Преобразование имени класса в узле дерева
				ClassesTableElement* element = ClassesTable::items->at(fullClassName);
				// Получить информацию для создания константы типа method ref
				string descriptor;
				string className;
				element->getMethodForRef(methodNameWithType, &descriptor, &className, Receiver->type == SUPER_RECEIVER_TYPE);
				// Добавить константу method ref
				if (methodNameWithType != "newStatic" && methodNameWithType != "allocStatic")
					Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
				else
					if (element->Methods->items.count(methodNameWithType) == 0) {
						Constant = constantTable->findOrAddMethodRefConstant(className, "<init>", "()V");
						isInitMethod = true;
					}
					else
						Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
			}
		}

		// Выполнит поиску на дочерних элементах message selector
		Arguments->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	// Вызвать на дочерних элементах
	if (Left != NULL) {
		Left->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	if (Right != NULL) {
		Right->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	if (ArgumentsList != NULL) {
		ArgumentsList->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
}

void Expression_node::fillLiterals(ConstantsTable* constantTable)
{
	if (type == LITERAL_EXPRESSION_TYPE) {
		literal->fillLiterals(constantTable);
	}
	else if (type == NUMERIIC_CONSTANT_EXPRESSION_TYPE) {
		num->fillLiterals(constantTable);
	}

	if (Left != NULL)
		Left->fillLiterals(constantTable);
	if (Right != NULL)
		Right->fillLiterals(constantTable);
	if (Receiver != NULL)
		Receiver->fillLiterals(constantTable);
	if (Arguments != NULL)
		Arguments->fillLiterals(constantTable);
	if (ArgumentsList != NULL)
		ArgumentsList->fillLiterals(constantTable);

}

// ---------- Receiver_node ----------
void Receiver_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	if (type == SELF_RECEIVER_TYPE) {
		LocalVariablesTableElement* self = localVariablesTable->items["self"];
		string descriptor = self->type->getDescriptor(); //Строка дескриптора
		string className = self->type->ClassName; //Имя класса
		//Формирование fieldRef
		Constant = constantTable->findOrAddFieldRefConstant(className, "self", descriptor);
	}
	else if (type == OBJECT_NAME_RECEIVER_TYPE) {
		if (classTableElement->isContainsField(name)) {
			string descriptor; //Дескриптор
			string className; //Имя класса
			classTableElement->getFieldForRef(name, &descriptor, &className); //Получение данных поля для создания fieldRef
			Constant = constantTable->findOrAddFieldRefConstant(className, name, descriptor); //Формирование fieldRef
		}
	}
	else if (type == OBJECT_ARRAY_RECEIVER_TYPE) {
		if (classTableElement->isContainsField(name)) {
			string descriptor; //Дескриптор
			string className; //Имя класса
			classTableElement->getFieldForRef(name, &descriptor, &className); //Получение данных поля для создания fieldRef
			Constant = constantTable->findOrAddFieldRefConstant(className, name, descriptor); //Формирование fieldRef
		}
		ObjectArrayIndex->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	else if (type == MESSAGE_EXPRESSION_RECEIVER_TYPE) {
		Receiver->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		if  (Arguments != NULL)
			Arguments->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
}

bool Receiver_node::getName(string* Name)
{
	if (type == SUPER_RECEIVER_TYPE) {
		*Name = "super";
		return true;
	}
	else if (type == SELF_RECEIVER_TYPE) {
		*Name = "self";
		return true;
	}
	else if (type == OBJECT_NAME_RECEIVER_TYPE) {
		*Name = name;
		return true;
	}
	else if (type == OBJECT_ARRAY_RECEIVER_TYPE) {
		*Name = name;
		return true;
	}
	else if (type == CLASS_NAME_RECEIVER_TYPE) {
		*Name = name;
		return false;
	}
	else if (type == MESSAGE_EXPRESSION_RECEIVER_TYPE) {
		return true;
	}
}

void Receiver_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod, Type **returnType)
{
	if (type == MESSAGE_EXPRESSION_RECEIVER_TYPE) {
		string receiverName;
		bool isObject;
		bool isReceiver = false;
		if (Receiver->type == MESSAGE_EXPRESSION_RECEIVER_TYPE) {// Если ресивер имеет тип message_selector, то вызывать на дочерних элементах
			Type* returnType;
			Receiver->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod, &returnType);
			if (returnType->DataType != CLASS_NAME_TYPE) {
				string msg = "Receiver is not an object in line: " + to_string(line);
				throw new std::exception(msg.c_str());
			}
			isObject = true;
			isReceiver = true;
			receiverName = returnType->ClassName;
		}
		else {
			isObject = Receiver->getName(&receiverName); // получить имя объекта/класса и его тип
			//преобразования для self и super
			if (receiverName == "self") {
				if (!isInInstanceMethod)
					receiverName = classTableElement->getClassName();
				isObject = isInInstanceMethod;
			}
			if (receiverName == "super") {
				if (!isInInstanceMethod)
					receiverName = classTableElement->getSuperClassName();
				isObject = isInInstanceMethod;
			}
		}


		// От message_selector получать имя метода.
		string methodName = Arguments->MethodName; // Имя метода
		string methodNameWithType; //Имя метода с dynamic или static

		if (isObject)
			methodNameWithType = methodName + "Dynamic";
		else
			methodNameWithType = methodName + "Static";

		strcpy(Arguments->MethodName, methodNameWithType.c_str()); // Преобразование имени метода в узле дерева


		// Проверить наличие метода в классе
		if (isReceiver) {
			bool isContainsMethod = ClassesTable::items->at(receiverName)->isContainsMethod(methodNameWithType);
			if (!isContainsMethod) {
				string msg = "Class '" + receiverName + "' don't contains method '-" + methodName + "'";
				throw new std::exception(msg.c_str());
			}
		}
		else {
			if (isObject) {
				if (!localVariablesTable->isContains(receiverName) && !classTableElement->isContainsField(receiverName) && receiverName != "super") {
					string msg = "Using undeclarated variable '" + receiverName + "' in line: " + to_string(line);
					throw new std::exception(msg.c_str());
				}
				else if (!localVariablesTable->isContains(receiverName) && !classTableElement->isContainsField(receiverName) && receiverName != "super") { //Является полем класса
					FieldsTableElement* field = classTableElement->Fields->items[receiverName];
					if (field->type->DataType != CLASS_NAME_TYPE) {
						string msg = "Variable '" + receiverName + "' is not an object in line: " + to_string(line);
						throw new std::exception(msg.c_str());
					}
					string className = field->type->ClassName;
					bool isContainsMethod = ClassesTable::items->at(className)->isContainsMethod(methodNameWithType);
					if (!isContainsMethod) {
						string msg = "Class '" + receiverName + "' don't contains method '-" + methodName + "'";
						throw new std::exception(msg.c_str());
					}
				}
				else if (receiverName != "super") { //Является локальной переменной
					LocalVariablesTableElement* local = localVariablesTable->items[receiverName];
					if (local->type->DataType != CLASS_NAME_TYPE) {
						string msg = "Variable '" + receiverName + "' is not an object in line: " + to_string(line);
						throw new std::exception(msg.c_str());
					}
					string className = local->type->ClassName;
					bool isContainsMethod = ClassesTable::items->at(className)->isContainsMethod(methodNameWithType);
					if (!isContainsMethod) {
						string msg = "Class '" + receiverName + "' don't contains method '-" + methodName + "'";
						throw new std::exception(msg.c_str());
					}
				}
				else { //Super
					string className = classTableElement->getSuperClassName();
					bool isContainsMethod = ClassesTable::items->at(className)->isContainsMethod(methodNameWithType);
					if (!isContainsMethod) {
						string msg = "Class '" + className + "' don't contains method '-" + methodName + "'";
						throw new std::exception(msg.c_str());
					}
				}
			}
			else {
				string fullClassName = ClassesTable::getFullClassName(receiverName); //Получить полное имя класса //TODO: Возможно, нужно будет убрать, если при преобразовании дерева будут заменяться сразу все имена классов
				if (Receiver->name != NULL)
					strcpy(Receiver->name, fullClassName.c_str()); //Преобразование имени класса в узле дерева
				bool isContainsMethod = ClassesTable::items->at(fullClassName)->isContainsMethod(methodNameWithType); //Проверить наличие метода
				if (!isContainsMethod) {
					string msg = "Class '" + receiverName + "' don't contains method '+" + methodName + "'";
					throw new std::exception(msg.c_str());
				}
			}
		}

		// Добавить methodRef
		if (isReceiver) {
			ClassesTableElement* element = ClassesTable::items->at(receiverName);
			// Получить информацию для создания константы типа method ref
			string descriptor;
			string className;
			element->getMethodForRef(methodNameWithType, &descriptor, &className, Receiver->type == SUPER_RECEIVER_TYPE);
			// Добавить константу method ref
			if (methodNameWithType != "newStatic" && methodNameWithType != "allocStatic")
				Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
			else
				if (element->Methods->items.count(methodNameWithType) == 0) {
					Constant = constantTable->findOrAddMethodRefConstant(className, "<init>", "()V");
					isInitMethod = true;
				}
				else
					Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
			*returnType = ClassesTable::items->at(className)->Methods->items[methodNameWithType]->ReturnType;
		}
		else {
			if (isObject) {
				string className;
				if (receiverName == "super") {//Super
					className = classTableElement->getSuperClassName();
				}
				else if (!localVariablesTable->isContains(receiverName) && classTableElement->isContainsField(receiverName)) { //Является полем класса
					FieldsTableElement* field = classTableElement->Fields->items[receiverName];
					className = field->type->ClassName;
				}
				else { //Является локальной переменной
					LocalVariablesTableElement* local = localVariablesTable->items[receiverName];
					className = local->type->ClassName;
				}
				ClassesTableElement* element = ClassesTable::items->at(className);
				// Получить информацию для создания константы типа method ref
				string descriptor;
				element->getMethodForRef(methodNameWithType, &descriptor, &className, Receiver->type == SUPER_RECEIVER_TYPE);
				// Добавить константу method ref
				if (methodNameWithType != "newStatic" && methodNameWithType != "allocStatic")
					Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
				else
					if (element->Methods->items.count(methodNameWithType) == 0) {
						Constant = constantTable->findOrAddMethodRefConstant(className, "<init>", "()V");
						isInitMethod = true;
					}
					else
						Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);

				*returnType = ClassesTable::items->at(className)->Methods->items[methodNameWithType]->ReturnType;
			}
			else {
				string fullClassName = ClassesTable::getFullClassName(receiverName); //Получить полное имя класса //TODO: Возможно, нужно будет убрать, если при преобразовании дерева будут заменяться сразу все имена классов
				if (Receiver->name != NULL)
					strcpy(Receiver->name, fullClassName.c_str()); // Преобразование имени класса в узле дерева
				ClassesTableElement* element = ClassesTable::items->at(fullClassName);
				// Получить информацию для создания константы типа method ref
				string descriptor;
				string className;
				element->getMethodForRef(methodNameWithType, &descriptor, &className, Receiver->type == SUPER_RECEIVER_TYPE);
				// Добавить константу method ref
				if (methodNameWithType != "newStatic" && methodNameWithType != "allocStatic")
					Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);
				else 
					if (element->Methods->items.count(methodNameWithType) == 0)
						Constant = constantTable->findOrAddMethodRefConstant(className, "<init>", "()V");
					else
						Constant = constantTable->findOrAddMethodRefConstant(className, methodNameWithType, descriptor);

				*returnType = ClassesTable::items->at(className)->Methods->items[methodNameWithType]->ReturnType;
			}
		}

		// Выполнит поиску на дочерних элементах message selector
		Arguments->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
	else if (type == OBJECT_ARRAY_RECEIVER_TYPE) {
		ObjectArrayIndex->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	}
}

void Receiver_node::fillLiterals(ConstantsTable* constantTable)
{
	if (Receiver != NULL)
		Receiver->fillLiterals(constantTable);
	if (Arguments != NULL)
		Arguments->fillLiterals(constantTable);
	if (ObjectArrayIndex != NULL)
		ObjectArrayIndex->fillLiterals(constantTable);
}

// ---------- Message_selector_node ----------
void Message_selector_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	if (FirstArgument != NULL) {
		FirstArgument->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	}
	if (Arguments != NULL)
		Arguments->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	if (ExprArguments != NULL)
		ExprArguments->fillFieldRefs(constantTable, localVariablesTable, classTableElement);

}

void Message_selector_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	if (FirstArgument != NULL)
		FirstArgument->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	if (Arguments != NULL)
		Arguments->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	if (ExprArguments != NULL)
		ExprArguments->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
}

void Message_selector_node::fillLiterals(ConstantsTable* constantTable)
{
	if (FirstArgument != NULL)
		FirstArgument->fillLiterals(constantTable);
	if (Arguments != NULL)
		Arguments->fillLiterals(constantTable);
	if (ExprArguments != NULL)
		ExprArguments->fillLiterals(constantTable);
}

// ---------- Keyword_argument_list_node ----------
void Keyword_argument_list_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	Keyword_argument_node* arg = First;
	while (arg != NULL)
	{
		arg->expression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		arg = arg->Next;
	}
}

void Keyword_argument_list_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInstanceMethod)
{
	Keyword_argument_node* arg = First;
	while (arg != NULL)
	{
		arg->expression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInstanceMethod);
		arg = arg->Next;
	}
}

void Keyword_argument_list_node::fillLiterals(ConstantsTable* constantTable)
{
	Keyword_argument_node* arg = First;
	while (arg != NULL)
	{
		arg->expression->fillLiterals(constantTable);
		arg = arg->Next;
	}
}

// ---------- Expression_list_node ----------

void Expression_list_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	Expression_node* cur = First;
	while (cur != NULL)
	{
		cur->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		cur = cur->Next;
	}
}

void Expression_list_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	Expression_node* cur = First;
	while (cur != NULL)
	{
		cur->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		cur = cur->Next;
	}
}

void Expression_list_node::fillLiterals(ConstantsTable* constantTable)
{
	Expression_node* cur = First;
	while (cur != NULL)
	{
		cur->fillLiterals(constantTable);
		cur = cur->Next;
	}
}

// ---------- Init_declarator_list_node ----------
void Init_declarator_list_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	Init_declarator_node* declarator = First;
	while (declarator != NULL)
	{
		declarator->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
		declarator = declarator->Next;
	}
}

void Init_declarator_list_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	Init_declarator_node* declarator = First;
	while (declarator != NULL)
	{
		declarator->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
		declarator = declarator->Next;
	}
}

void Init_declarator_list_node::fillLiterals(ConstantsTable* constantTable)
{
	Init_declarator_node* declarator = First;
	while (declarator != NULL)
	{
		declarator->fillLiterals(constantTable);
		declarator = declarator->Next;
	}
}

// ---------- Init_declarator_node ---------- 
void Init_declarator_node::fillFieldRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement)
{
	if (expression != NULL)
		expression->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	if (ArraySize != NULL)
		ArraySize->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
	if (InitializerList != NULL)
		InitializerList->fillFieldRefs(constantTable, localVariablesTable, classTableElement);
}

void Init_declarator_node::fillMethodRefs(ConstantsTable* constantTable, LocalVariablesTable* localVariablesTable, ClassesTableElement* classTableElement, bool isInInstanceMethod)
{
	if (expression != NULL)
		expression->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	if (ArraySize != NULL)
		ArraySize->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);
	if (InitializerList != NULL)
		InitializerList->fillMethodRefs(constantTable, localVariablesTable, classTableElement, isInInstanceMethod);

}

void Init_declarator_node::fillLiterals(ConstantsTable* constantTable)
{
	if (expression != NULL)
		expression->fillLiterals(constantTable);
	if (ArraySize != NULL)
		ArraySize->fillLiterals(constantTable);
	if (InitializerList != NULL)
		InitializerList->fillLiterals(constantTable);
}



// ---------- Numeric_constant ----------
void Numeric_constant_node::fillLiterals(ConstantsTable* constantTable)
{
	if (Int > 32767 || Int < -32768) {
		constantTable->findOrAddConstant(Integer, Int);
	}
}

// ---------- literal ----------
void Literal_node::fillLiterals(ConstantsTable* constantTable)
{
	if (type == STRING_CONSTANT_TYPE || type == NSSTRING_CONSTANT_TYPE) {
		int utfIndex = constantTable->findOrAddConstant(UTF8, value);
		constantTable->findOrAddConstant(String, 0, utfIndex);
	}
}

