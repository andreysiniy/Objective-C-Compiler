#include "tables.h"
#include <algorithm>
#include <string>
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

