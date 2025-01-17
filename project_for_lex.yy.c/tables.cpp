#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "tables.h"
using namespace std;

// -------------------- ConstantsTableElement --------------------

ConstantsTableElement::ConstantsTableElement(int id, constantType type, string utf8string)
{
	Id = id;
	Type = type;
	Utf8String = new string(utf8string);
}


ConstantsTableElement::ConstantsTableElement(int id, constantType type, int number, int firstRef, int secondRef)
{
	Id = id;
	Type = type;
	Number = number;
	FirstRef = firstRef;
	SecondRef = secondRef;
}

// -------------------- ConstantsTable --------------------

ConstantsTable::ConstantsTable()
{
	items[maxId] = new ConstantsTableElement(maxId, UTF8, "Code");
	maxId++;
}

int ConstantsTable::findOrAddConstant(constantType type, string utf8string)
{
	int res = findConstant(type, &utf8string);
	if (res == -1)
	{
		res = maxId++;
		items[res] = new ConstantsTableElement(res, type, utf8string);
	}
	return res;
}

int ConstantsTable::findOrAddConstant(constantType type, int number, int firstRef, int secondRef)
{
	int res = findConstant(type, NULL, number, firstRef, secondRef);
	if (res == -1)
	{
		res = maxId++;
		items[res] = new ConstantsTableElement(res, type, number, firstRef, secondRef);
	}
	return res;
}

int ConstantsTable::findConstant(constantType type, string* utf8string, int number, int firstRef, int secondRef)
{
	string compared = utf8string == NULL ? "" : *utf8string;
	auto iter = items.cbegin();
	while (iter != items.cend())
	{
		string curStr = iter->second->Utf8String == NULL ? "" : *iter->second->Utf8String;
		if (iter->second->Type == type && curStr == compared && iter->second->Number == number && iter->second->FirstRef == firstRef && iter->second->SecondRef == secondRef)
		{
			return iter->first;
		}
		++iter;
	}
	return -1;
}

ConstantsTableElement* ConstantsTable::getConstant(int id)
{
	return items[id];
}

string ConstantsTable::getConstantString(int id)
{
	if (items[id]->Type != constantType::UTF8)
	{
		return "";
	}
	return *items[id]->Utf8String;
}

int ConstantsTable::findOrAddFieldRefConstant(string className, string fieldName, string descriptor)
{
	int classNameConst = this->findOrAddConstant(UTF8, className);
	int classConst = this->findOrAddConstant(Class, NULL, classNameConst);
	int nameConst = this->findOrAddConstant(UTF8, fieldName);
	int descriptorConst = this->findOrAddConstant(UTF8, descriptor);
	int nameAndTypeConst = this->findOrAddConstant(NameAndType, NULL, nameConst, descriptorConst);
	int fieldRefConst = this->findOrAddConstant(FieldRef, NULL, nameAndTypeConst, classConst);

	return fieldRefConst;
}

int ConstantsTable::findOrAddMethodRefConstant(string className, string methodName, string descriptor)
{
	int classNameConst = this->findOrAddConstant(UTF8, className);
	int classConst = this->findOrAddConstant(Class, NULL, classNameConst);
	int nameConst = this->findOrAddConstant(UTF8, methodName);
	int descriptorConst = this->findOrAddConstant(UTF8, descriptor);
	int nameAndTypeConst = this->findOrAddConstant(NameAndType, NULL, nameConst, descriptorConst);
	int methodRefConst = this->findOrAddConstant(MethodRef, NULL, nameAndTypeConst, classConst);

	return methodRefConst;
}

// -------------------- ClassesTableElement --------------------

ClassesTableElement::ClassesTableElement(string name, string* superclassName, bool isImplementation)
{
	ConstantTable = new ConstantsTable();
	Fields = new FieldsTable();
	Methods = new MethodsTable();
	Properties = new PropertiesTable();
	Name = ConstantTable->findOrAddConstant(UTF8, name);
	if (superclassName != NULL)
		SuperclassName = ConstantTable->findOrAddConstant(UTF8, *superclassName);
	ThisClass = ConstantTable->findOrAddConstant(Class, NULL, Name);
	if (superclassName != NULL)
		Superclass = ConstantTable->findOrAddConstant(Class, NULL, SuperclassName);
	IsImplementation = isImplementation;
}
string ClassesTableElement::getClassName()
{
	return ConstantTable->getConstantString(Name);
}

string ClassesTableElement::getSuperClassName()
{
	if (SuperclassName == NULL)
		return "java/lang/Object";
	else
		return ConstantTable->getConstantString(SuperclassName);
}

void ClassesTableElement::fillFieldRefs()
{
	for (auto iter = Methods->items.cbegin(); iter != Methods->items.cend(); ++iter)
	{
		iter->second->fillFieldRefs(ConstantTable, this);
	}
}

void ClassesTableElement::fillMethodRefs()
{
	for (auto iter = Methods->items.cbegin(); iter != Methods->items.cend(); ++iter)
	{
		iter->second->fillMethodRefs(ConstantTable, this);
	}
}

void ClassesTableElement::fillLiterals()
{
	for (auto iter = Methods->items.cbegin(); iter != Methods->items.cend(); ++iter)
	{
		iter->second->fillLiterals(ConstantTable);
	}

	for (auto iter = Fields->items.cbegin(); iter != Fields->items.cend(); ++iter)
	{
		iter->second->fillLiterals(ConstantTable);
	}
}

bool ClassesTableElement::isContainsField(string fieldName)
{
	if (Fields->items.count(fieldName) != 0)
		return true;
	else {
		if (SuperclassName != NULL)
			return ClassesTable::items->at(getSuperClassName())->isContainsField(fieldName);
	}
	return false;
}

FieldsTableElement* ClassesTableElement::getFieldForRef(string name, string* descriptor, string* className)
{
	if (isContainsField(name)) { // Содержит поле
		if (Fields->items.count(name) != 0) { //Поле содержится в текущем классе
			*descriptor = Fields->items[name]->DescriptorStr;
			*className = getClassName();
			return Fields->items[name];
		}
		else { //Поле содержится в одном из родительских классов
			if (SuperclassName != NULL)
				return ClassesTable::items->at(getSuperClassName())->getFieldForRef(name, descriptor, className);
		}
	}
	return NULL;
}

bool ClassesTableElement::isHaveOneOfSuperclass(string name)
{
	if (SuperclassName == NULL) { //Родительский класс отсутствует
		return false;
	}
	else {
		if (getSuperClassName() == name) //Имя родительского класса совпадает с искомым
			return true;
		else if (getSuperClassName() != "java/lang/Object")
			return ClassesTable::items->at(getSuperClassName())->isHaveOneOfSuperclass(name); //Проверить является искомый класс родительским для родительского
		else
			return false;
	}
}

bool ClassesTableElement::isContainsMethod(string methodName)
{
	if (Methods->items.count(methodName) != 0)
		return true;
	else {
		if (SuperclassName != NULL)
			return ClassesTable::items->at(getSuperClassName())->isContainsMethod(methodName);
	}
	return false;
}

MethodsTableElement* ClassesTableElement::getMethodForRef(string name, string* descriptor, string* className, bool isSupercall)
{
	if (isContainsMethod(name)) { // Содержит метод
		if (Methods->items.count(name) != 0) { //Метод содержится в текущем классе
			*descriptor = Methods->items[name]->DescriptorStr;
			*className = getClassName();
			return Methods->items[name];
		}
		else { //Метод содержится в одном из родительских классов
			if (SuperclassName != NULL) {
				MethodsTableElement* method = ClassesTable::items->at(getSuperClassName())->getMethodForRef(name, descriptor, className);
				if (isSupercall)
					*className = getSuperClassName();
				else
					*className = getClassName();
				return method;
			}
		}
	}
	return NULL;
}

void ClassesTableElement::semanticTransform()
{
	for (auto iter = Methods->items.cbegin(); iter != Methods->items.cend(); ++iter)
	{
		iter->second->semanticTransform(ConstantTable);
	}

	if (Superclass == NULL) {
		SuperclassName = ConstantTable->findOrAddConstant(UTF8, "java/lang/Object");
		Superclass = ConstantTable->findOrAddConstant(Class, NULL, SuperclassName);
	}

	appendConstructor();
}

// -------------------- ClassesTable --------------------
map<string, ClassesTableElement*>* ClassesTable::items = new map<string, ClassesTableElement*>;

ClassesTableElement* ClassesTable::addClass(string name, string* superclassName, bool isImplementation, Class_block_node* classBlock)
{
	string fullName = "global/" + name;
	string* fullSuperclassName = NULL;
	if (superclassName != NULL) {
		if (*superclassName == "NSObject" || *superclassName == "NSString" || *superclassName == "NSArray" || *superclassName == "InOutFuncs")
			fullSuperclassName = new string("rtl/" + *superclassName);
		else
			fullSuperclassName = new string("global/" + *superclassName);
	}
	if (name == "NSObject" || name == "NSString" || name == "NSArray" || name == "InOutFuncs") {
		string msg = "Unsupported protocols. Because of class '" + name + "' is reserved.";
		throw new std::exception(msg.c_str());
	}
		
	ClassesTableElement* element = new ClassesTableElement("global/" + name, fullSuperclassName, isImplementation); // Новый добавляемый элемент


	if (!isImplementation && items->count(fullName) && items->at(fullName)->IsImplementation) { // Проверка, чтобы интерфейс класса при его наличии находился раньше реализации
		string msg = "Class interface'" + name + "' after implementation";
		throw new std::exception(msg.c_str());
	}
	else if (items->count(fullName) && items->at(fullName)->IsImplementation == isImplementation) { // Проверка на повторное объявление класса
		string msg = "Rediifnition of class '" + name + "'";
		throw new std::exception(msg.c_str());
	}
	else if (superclassName != NULL && items->count(fullName) && items->at(fullName)->ConstantTable->getConstantString(items->at(fullName)->SuperclassName) != *fullSuperclassName) { // Проверка на совпадение суперкласса в интерфейсе и реаизации
		string msg = "Class '" + name + "' with different superclass";
		throw new std::exception(msg.c_str());
	}
	else if (items->count(fullName) && !items->at(fullName)->IsImplementation && isImplementation) { // Объявление реализации после интерфейса
		items->at(fullName)->IsImplementation = true;
		items->at(fullName)->IsHaveInterface = true;
		delete element;
	}
	else {
		(*items)[fullName] = element; // Добавление (обновление элемента в таблице классов)
	}

	//Преобразование имени в узле дерева
	if (isImplementation) {
		Class_implementation_node* implementation = (Class_implementation_node*)classBlock;
		strcpy(implementation->ClassName, fullName.c_str());
		if (fullSuperclassName != NULL)
			strcpy(implementation->SuperclassName, fullSuperclassName->c_str());
		else
			implementation->SuperclassName = NULL;

	}
	else {
		Class_interface_node* interface = (Class_interface_node*)classBlock;
		strcpy(interface->ClassName, fullName.c_str());
		if (fullSuperclassName != NULL)
			strcpy(interface->SuperclassName, fullSuperclassName->c_str());
		else
			interface->SuperclassName = NULL;
	}

	return items->at(fullName);
}
void ClassesTable::initClassInOutFuncs()
{
	// Создание класса InOutFuncs
	ClassesTableElement* InOutFuncs = new ClassesTableElement("rtl/InOutFuncs", NULL, true);

	// Добавление метода printInt
	ConstantsTable* сonstantTable = InOutFuncs->ConstantTable;
	Type* printIntReturnType = new Type(VOID_TYPE);
	vector<Type*>* printIntKeywordsType = new vector<Type*>{ new Type(INT_TYPE) };
	vector<Type*>* printIntParamsType = new vector<Type*>;
	InOutFuncs->Methods->addMethod(сonstantTable, "printIntStatic", "(I)V", true, NULL, printIntReturnType, printIntParamsType, printIntKeywordsType);

	//Добавление метода printChar
	Type* printCharReturnType = new Type(VOID_TYPE);
	vector<Type*>* printCharKeywordsType = new vector<Type*>{ new Type(CHAR_TYPE) };
	vector<Type*>* printCharParamsType = new vector<Type*>;
	InOutFuncs->Methods->addMethod(сonstantTable, "printCharStatic", "(C)V", true, NULL, printCharReturnType, printCharParamsType, printCharKeywordsType);

	//Добавление метода printString
	Type* printStringReturnType = new Type(VOID_TYPE);
	vector<Type*>* printStringKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSString") };
	vector<Type*>* printStringParamsType = new vector<Type*>;
	InOutFuncs->Methods->addMethod(сonstantTable, "printNSStringStatic", "(Lrtl/NSString;)V", true, NULL, printStringReturnType, printStringParamsType, printStringKeywordsType);

	//Добавление метода printCharArray
	Type* printCharArrayReturnType = new Type(VOID_TYPE);
	vector<Type*>* printCharArrayKeywordsType = new vector<Type*>{ new Type(CHAR_TYPE, 1024) };
	vector<Type*>* printCharArrayParamsType = new vector<Type*>;
	InOutFuncs->Methods->addMethod(сonstantTable, "printCharArrayStatic", "([C)V", true, NULL, printCharArrayReturnType, printCharArrayParamsType, printCharArrayKeywordsType);

	//Добавление метода printObject
	Type* printObjectReturnType = new Type(VOID_TYPE);
	vector<Type*>* printObjectKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "java/lang/Object") };
	vector<Type*>* printObjectParamsType = new vector<Type*>;
	InOutFuncs->Methods->addMethod(сonstantTable, "printObjectStatic", "(Ljava/lang/Object;)V", true, NULL, printObjectReturnType, printObjectParamsType, printObjectKeywordsType);

	//Добавление метода read
	Type* readReturnType = new Type(CLASS_NAME_TYPE, "java/lang/String");
	vector<Type*>* readKeywordsType = new vector<Type*>;
	vector<Type*>* readParamsType = new vector<Type*>;
	InOutFuncs->Methods->addMethod(сonstantTable, "readStatic", "()Ljava/lang/String;", true, NULL, readReturnType, readParamsType, readKeywordsType);

	//Добавление метода readInt
	Type* readIntReturnType = new Type(INT_TYPE);
	vector<Type*>* readIntKeywordsType = new vector<Type*>;
	vector<Type*>* readIntParamsType = new vector<Type*>;
	InOutFuncs->Methods->addMethod(сonstantTable, "readIntStatic", "()I", true, NULL, readIntReturnType, readIntParamsType, readIntKeywordsType);

	//Добавление метода readChar
	Type* readCharReturnType = new Type(CHAR_TYPE);
	vector<Type*>* readCharKeywordsType = new vector<Type*>;
	vector<Type*>* readCharParamsType = new vector<Type*>;
	InOutFuncs->Methods->addMethod(сonstantTable, "readCharStatic", "()C", true, NULL, readCharReturnType, readCharParamsType, readCharKeywordsType);


	//Добавление FieldRef
	сonstantTable->findOrAddFieldRefConstant("java/lang/System", "out", "Ljava/io/PrintStream;");

	//Добавление MethodRef
	сonstantTable->findOrAddMethodRefConstant("java/lang/Object", "<init>", "()V");
	сonstantTable->findOrAddMethodRefConstant("java/io/PrintStream", "print", "(I)V");
	сonstantTable->findOrAddMethodRefConstant("java/io/PrintStream", "print", "(C)V");
	сonstantTable->findOrAddMethodRefConstant("java/io/PrintStream", "print", "(Ljava/lang/String;)V");
	сonstantTable->findOrAddMethodRefConstant("java/io/PrintStream", "print", "([C)V");
	сonstantTable->findOrAddMethodRefConstant("java/io/PrintStream", "print", "Ljava/lang/Object;");
	сonstantTable->findOrAddMethodRefConstant("java/lang/System", "console", "Ljava/io/Console;");
	сonstantTable->findOrAddMethodRefConstant("java/io/Console", "readLine", "()Ljava/lang/String;");
	сonstantTable->findOrAddMethodRefConstant("rtl/InOutFuncs", "read", "()Ljava/lang/String;");
	сonstantTable->findOrAddMethodRefConstant("java/lang/Integer", "parseInt", "(Ljava/lang/String;)I");
	сonstantTable->findOrAddMethodRefConstant("java/lang/String", "charAt", "(I)C");

	//Добавление класса InOutFuncs в таблицу классов
	(*items)["rtl/InOutFuncs"] = InOutFuncs;
}
