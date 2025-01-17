﻿#include <iostream>
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

void ClassesTable::initRTL()
{
	initClassProgram();
	initClassInOutFuncs();
	initClassNSObject();
	initClassNSString();
	initClassNSArray();
}

void ClassesTable::initClassProgram()
{
	// Создание класса Program
	ClassesTableElement* Program = new ClassesTableElement("rtl/!Program!", NULL, true);

	//Добавление класса Program в таблицу классов
	(*items)["rtl/!Program!"] = Program;
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

void ClassesTable::initClassNSObject()
{
	//Создание класса NSObject
	ClassesTableElement* nsobject = new ClassesTableElement("rtl/NSObject", NULL, true);
	ConstantsTable* constantTable = nsobject->ConstantTable;

	//Добавление конструктора <init>
	Type* constructorReturnType = new Type(VOID_TYPE);
	vector<Type*>* constructorKeywordsType = new vector<Type*>;
	vector<Type*>* constructorParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "<init>", "()V", false, NULL, constructorReturnType, constructorParamsType, constructorKeywordsType);

	//Добавление метода alloc
	Type* allocReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSObject");
	vector<Type*>* allocKeywordsType = new vector<Type*>;
	vector<Type*>* allocParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "allocStatic", "()Lrtl/NSObject;", true, NULL, allocReturnType, allocParamsType, allocKeywordsType);

	//Добавление метода init
	Type* initReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSObject");
	vector<Type*>* initKeywordsType = new vector<Type*>;
	vector<Type*>* initParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "initDynamic", "()Lrtl/NSObject;", false, NULL, initReturnType, initParamsType, initKeywordsType);

	//Добавление метода new
	Type* newReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSObject");
	vector<Type*>* newKeywordsType = new vector<Type*>;
	vector<Type*>* newParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "newStatic", "()Lrtl/NSObject;", true, NULL, newReturnType, newParamsType, newKeywordsType);

	//Добавление  метода getClassDynamic
	Type* getClassDynamicReturnType = new Type(CLASS_NAME_TYPE, "java/lang/Class");
	vector<Type*>* getClassDynamicKeywordsType = new vector<Type*>;
	vector<Type*>* getClassDynamicParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "getClassDynamic", "()Ljava/lang/Class;", false, NULL, getClassDynamicReturnType, getClassDynamicParamsType, getClassDynamicKeywordsType);

	//Добавление метода getClassStatic
	Type* getClassStaticReturnType = new Type(CLASS_NAME_TYPE, "java/lang/Class");
	vector<Type*>* getClassStaticKeywordsType = new vector<Type*>;
	vector<Type*>* getClassStaticParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "getClassStatic", "()Ljava/lang/Class;", true, NULL, getClassStaticReturnType, getClassStaticParamsType, getClassStaticKeywordsType);

	//Добавление метода isSubclassOfClass
	Type* isSubclassOfClassReturnType = new Type(INT_TYPE);
	vector<Type*>* isSubclassOfClassKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "java/lang/Class") };
	vector<Type*>* isSubclassOfClassParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "isSubclassOfClassStatic", "(Ljava/lang/Class;)I", true, NULL, isSubclassOfClassReturnType, isSubclassOfClassParamsType, isSubclassOfClassKeywordsType);

	//Добавление метода className
	Type* classNameReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* classNameKeywordsType = new vector<Type*>;
	vector<Type*>* classNameParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "classNameDynamic", "()Lrtl/NSString;", false, NULL, classNameReturnType, classNameParamsType, classNameKeywordsType);

	//Добавление метода superclass
	Type* superclassReturnType = new Type(CLASS_NAME_TYPE, "java/lang/Class");
	vector<Type*>* superclassKeywordsType = new vector<Type*>;
	vector<Type*>* superclassParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "superclassDynamic", "()Ljava/lang/Class;", false, NULL, superclassReturnType, superclassParamsType, superclassKeywordsType);

	//Добавление метода description
	Type* descriptionReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* descriptionKeywordsType = new vector<Type*>;
	vector<Type*>* descriptionParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "descriptionDynamic", "()Lrtl/NSString;", false, NULL, descriptionReturnType, descriptionParamsType, descriptionKeywordsType);

	//Добавление метода isEqual
	Type* isEqualReturnType = new Type(INT_TYPE);
	vector<Type*>* isEqualKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSObject") };
	vector<Type*>* isEqualParamsType = new vector<Type*>;
	nsobject->Methods->addMethod(constantTable, "isEqualDynamic", "(Lrtl/NSObject;)I", false, NULL, isEqualReturnType, isEqualParamsType, isEqualKeywordsType);

	//Добавление MethodRef
	constantTable->findOrAddMethodRefConstant("java/lang/Object", "<init>", "()V");
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "<init>", "()V");
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "allocStatic", "()Lrtl/NSObject;");
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "initDynamic", "()Lrtl/NSObject;");
	constantTable->findOrAddMethodRefConstant("java/lang/Object", "getClass", "()Ljava/lang/Class;");
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "getClassStatic", "()Ljava/lang/Class;");
	constantTable->findOrAddMethodRefConstant("java/lang/Class", "isAssignableFrom", "(Ljava/lang/Class;)Z");
	constantTable->findOrAddMethodRefConstant("java/lang/Class", "getName", "()Ljava/lang/String;");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "toCharArray", "()[C");
	constantTable->findOrAddMethodRefConstant("rtl/NSString", "stringWithCStringStatic", "([C)Lrtl/NSString;");
	constantTable->findOrAddMethodRefConstant("java/lang/Class", "getSuperclass", "()Ljava/lang/Class;");

	//Добавление константы string
	int strNum = constantTable->findOrAddConstant(UTF8, "nsobject implementation");
	constantTable->findOrAddConstant(String, NULL, strNum);

	//Добавление класса NSObject в таблицу классов
	(*items)["rtl/NSObject"] = nsobject;
}

void ClassesTable::initClassNSString()
{
	//Создание класса NSString
	string superclassName = "rtl/NSObject";
	ClassesTableElement* nsstring = new ClassesTableElement("rtl/NSString", &superclassName, true);
	ConstantsTable* constantTable = nsstring->ConstantTable;

	//Добавление метода string
	Type* stringReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* stringKeywordsType = new vector<Type*>;
	vector<Type*>* stringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "stringStatic", "()Lrtl/NSString;", true, NULL, stringReturnType, stringParamsType, stringKeywordsType);

	//Добавление метода stringWithCString
	Type* stringWithCStringReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* stringWithCStringKeywordsType = new vector<Type*>{ new Type(CHAR_TYPE, 1024) };
	vector<Type*>* stringWithCStringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "stringWithCStringStatic", "([C)Lrtl/NSString;", true, NULL, stringWithCStringReturnType, stringWithCStringParamsType, stringWithCStringKeywordsType);

	//Добавление метода stringWithString
	Type* stringWithStringReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* stringWithStringKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSString") };
	vector<Type*>* stringWithStringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "stringWithStringStatic", "(Lrtl/NSString;)Lrtl/NSString;", true, NULL, stringWithStringReturnType, stringWithStringParamsType, stringWithStringKeywordsType);

	//Добавление метода cString
	Type* cStringReturnType = new Type(CHAR_TYPE, 1024);
	vector<Type*>* cStringKeywordsType = new vector<Type*>;
	vector<Type*>* cStringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "cStringDynamic", "()[C;", false, NULL, cStringReturnType, cStringParamsType, cStringKeywordsType);

	//Добавление метода capitalizeString
	Type* capitalizeStringReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* capitalizeStringKewordsType = new vector<Type*>;
	vector<Type*>* capitalizeStringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "capitalizeStringDynamic", "()Lrtl/NSString;", false, NULL, capitalizeStringReturnType, capitalizeStringParamsType, capitalizeStringKewordsType);

	//Добавление метода characterAtIndex
	Type* characterAtIndexReturnType = new Type(CHAR_TYPE);
	vector<Type*>* characterAtIndexKeywordsType = new vector<Type*>{ new Type(INT_TYPE) };
	vector<Type*>* characterAtIndexParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "characterAtIndexDynamic", "(I)C;", false, NULL, characterAtIndexReturnType, characterAtIndexParamsType, characterAtIndexKeywordsType);

	//Добавление метода hasPrefix
	Type* hasPrefixReturnType = new Type(INT_TYPE);
	vector<Type*>* hasPrefixKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSString") };
	vector<Type*>* hasPrefixParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "hasPrefixDynamic", "(Lrtl/NSString;)I", false, NULL, hasPrefixReturnType, hasPrefixParamsType, hasPrefixKeywordsType);

	//Добавление метода hasSuffix
	Type* hasSuffixReturnType = new Type(INT_TYPE);
	vector<Type*>* hasSuffixKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSString") };
	vector<Type*>* hasSuffixParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "hasSuffixDynamic", "(Lrtl/NSString;)I", false, NULL, hasSuffixReturnType, hasSuffixParamsType, hasSuffixKeywordsType);

	//Добавление метода intValue
	Type* intValueReturnType = new Type(INT_TYPE);
	vector<Type*>* intValueKeywordsType = new vector<Type*>;
	vector<Type*>* intValueParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "intValueDynamic", "()I", false, NULL, intValueReturnType, intValueParamsType, intValueKeywordsType);

	//Добавление метода isEqual
	Type* isEqualReturnType = new Type(INT_TYPE);
	vector<Type*>* isEqualKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSObject") };
	vector<Type*>* isEqualParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "isEqualDynamic", "(Lrtl/NSObject;)I", false, NULL, isEqualReturnType, isEqualParamsType, isEqualKeywordsType);

	//Добавление метода isEqualToString
	Type* isEqualToStringReturnType = new Type(INT_TYPE);
	vector<Type*>* isEqualToStringKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSString") };
	vector<Type*>* isEqualToStringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "isEqualToStringDynamic", "(Lrtl/NSString;)I", false, NULL, isEqualToStringReturnType, isEqualToStringParamsType, isEqualToStringKeywordsType);

	//Добавление метода length
	Type* lengthReturnType = new Type(INT_TYPE);
	vector<Type*>* lengthKeywordsType = new vector<Type*>;
	vector<Type*>* lengthParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "lengthDynamic", "()I", false, NULL, lengthReturnType, lengthParamsType, lengthKeywordsType);

	//Добавление метода lowercaseString
	Type* lowercaseStringReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* lowercaseStringKeywordsType = new vector<Type*>;
	vector<Type*>* lowercaseStringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "lowercaseStringDynamic", "()Lrtl/NSString;", false, NULL, lowercaseStringReturnType, lowercaseStringParamsType, lowercaseStringKeywordsType);

	//Добавление метода uppercaseString
	Type* uppercaseStringReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* uppercaseStringKeywordsType = new vector<Type*>;
	vector<Type*>* uppercaseStringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "uppercaseStringDynamic", "()Lrtl/NSString;", false, NULL, uppercaseStringReturnType, uppercaseStringParamsType, uppercaseStringKeywordsType);

	//Добавление метода stringByAppendingString
	Type* stringByAppendingStringReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* stringByAppendingStringKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSString") };
	vector<Type*>* stringByAppendingStringParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "stringByAppendingStringDynamic", "(Lrtl/NSString;)Lrtl/NSString;", false, NULL, stringByAppendingStringReturnType, stringByAppendingStringParamsType, stringByAppendingStringKeywordsType);

	//Добавление метода description
	Type* descriptionReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* descriptionKeywordsType = new vector<Type*>;
	vector<Type*>* descriptionParamsType = new vector<Type*>;
	nsstring->Methods->addMethod(constantTable, "descriptionDynamic", "()Lrtl/NSString;", false, NULL, descriptionReturnType, descriptionParamsType, descriptionKeywordsType);

	//Добавление FieldRef
	constantTable->findOrAddFieldRefConstant("rtl/NSString", "string", "java/lang/String");

	//Добавление MethodRef
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "<init>", "()V");
	constantTable->findOrAddMethodRefConstant("rtl/NSString", "<init>", "(Ljava/lang/String;)V");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "<init>", "([C)V");
	constantTable->findOrAddMethodRefConstant("rtl/NSString", "<init>", "(Lrtl/NSString;)V");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "length", "()I");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "charAt", "(I)C");
	constantTable->findOrAddMethodRefConstant("java/lang/Character", "isWhitespace", "(C)Z");
	constantTable->findOrAddMethodRefConstant("java/lang/Character", "toUpperCase", "(C)C");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "startWith", "(Ljava/lang/String;)Z");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "endsWith", "(Ljava/lang/String;)Z");
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "initDynamic", "()Lrtl/NSObject");
	constantTable->findOrAddMethodRefConstant("java/lang/Integer", "parseInt", "(Ljava/lang/String;)I");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "equals", "(Ljava/lang/Object;)Z");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "toLowerCase", "()Ljava/lang/String;");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "toUpperCase", "()Ljava/lang/String;");

	//Добавление класса NSString в таблицу классов
	(*items)["rtl/NSString"] = nsstring;
}

void ClassesTable::initClassNSArray()
{
	//Создание класса NSArray
	string superclassName = "rtl/NSObject";
	ClassesTableElement* nsarray = new ClassesTableElement("rtl/NSArray", &superclassName, true);
	ConstantsTable* constantTable = nsarray->ConstantTable;

	//Добавление метода array
	Type* arrayReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSArray");
	vector<Type*>* arrayKeywordsType = new vector<Type*>;
	vector<Type*>* arrayParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "arrayStatic", "()Lrtl/NSArray;", true, NULL, arrayReturnType, arrayParamsType, arrayKeywordsType);

	//Добавление метода arrayWithArray
	Type* arrayWithArrayReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSArray");
	vector<Type*>* arrayWithArrayKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSArray") };
	vector<Type*>* arrayWithArrayParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "arrayWithArrayStatic", "(Lrtl/NSArray;)Lrtl/NSArray;", true, NULL, arrayWithArrayReturnType, arrayWithArrayParamsType, arrayWithArrayKeywordsType);

	//Добавление метода arrayWithObjects
	Type* arrayWithObjectsReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSArray");
	vector<Type*>* arrayWithObjectsKeywordsType = new vector<Type*>;
	vector<Type*>* arrayWithObjectsParamsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSObject", 1024) };
	nsarray->Methods->addMethod(constantTable, "arrayWithObjectsStatic", "([Lrtl/NSObject;)Lrtl/NSArray;", true, NULL, arrayWithObjectsReturnType, arrayWithObjectsParamsType, arrayWithObjectsKeywordsType);

	//Добавление метода arrayWithObject
	Type* arrayWithObjectReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSArray");
	vector<Type*>* arrayWithObjectKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSObject") };
	vector<Type*>* arrayWithObjectParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "arrayWithObjectStatic", "(Lrtl/NSObject;)Lrtl/NSArray;", true, NULL, arrayWithObjectReturnType, arrayWithObjectParamsType, arrayWithObjectKeywordsType);

	//Добавление метода arrayByAddingObject
	Type* arrayByAddingObjectReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSArray");
	vector<Type*>* arrayByAddingObjectKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSObject") };
	vector<Type*>* arrayByAddingObjectParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "arrayByAddingObjectDynamic", "(Lrtl/NSObject;)Lrtl/NSArray;", false, NULL, arrayByAddingObjectReturnType, arrayByAddingObjectParamsType, arrayByAddingObjectKeywordsType);

	//Добавление метода arrayByAddingObjectsFromArray
	Type* arrayByAddingObjectsFromArrayReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSArray");
	vector<Type*>* arrayByAddingObjectsFromArrayKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE,"rtl/NSArray") };
	vector<Type*>* arrayByAddingObjectsFromArrayParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "arrayByAddingObjectsFromArrayDynamic", "(Lrtl/NSArray;)Lrtl/NSArray;", false, NULL, arrayByAddingObjectsFromArrayReturnType, arrayByAddingObjectsFromArrayParamsType, arrayByAddingObjectsFromArrayParamsType);

	//Добавление метода componentsJoinedByString
	Type* componentsJoinedByStringReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* componentsJoinedByStringKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSString") };
	vector<Type*>* componentsJoinedByStringParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "componentsJoinedByStringDynamic", "(Lrtl/NSString;)Lrtl/NSString;", false, NULL, componentsJoinedByStringReturnType, componentsJoinedByStringParamsType, componentsJoinedByStringKeywordsType);

	//Добавление метода containsObject
	Type* containsObjectReturnType = new Type(INT_TYPE);
	vector<Type*>* containsObjectKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSObject") };
	vector<Type*>* containsObjectParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "containsObjectDynamic", "(Lrtl/NSObject;)I", false, NULL, containsObjectReturnType, containsObjectParamsType, containsObjectKeywordsType);

	//Добавление метода count
	Type* countReturnType = new Type(INT_TYPE);
	vector<Type*>* countKeywordsType = new vector<Type*>;
	vector<Type*>* countParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "countDynamic", "()I", false, NULL, countReturnType, countParamsType, countKeywordsType);

	//Добавление метода description
	Type* descriptionReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSString");
	vector<Type*>* descriptionKeywordsType = new vector<Type*>;
	vector<Type*>* descriptionParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "descriptionDynamic", "()Lrtl/NSString;", false, NULL, descriptionReturnType, descriptionParamsType, descriptionKeywordsType);

	//Добавление метода firstObject
	Type* firstObjectReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSObject");
	vector<Type*>* firstObjectKeywordsType = new vector<Type*>;
	vector<Type*>* firstObjectParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "firstObjectDynamic", "()Lrtl/NSObject;", false, NULL, firstObjectReturnType, firstObjectParamsType, firstObjectKeywordsType);

	//Добавление метода firstObjectCommonWithArray
	Type* firstObjectCommonWithArrayReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSObject");
	vector<Type*>* firstObjectCommonWithArrayKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSArray") };
	vector<Type*>* firstObjectCommonWithArrayParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "firstObjectCommonWithArrayDynamic", "(Lrtl/NSArray;)Lrtl/NSObject;", false, NULL, firstObjectCommonWithArrayReturnType, firstObjectCommonWithArrayParamsType, firstObjectCommonWithArrayKeywordsType);

	//Добавление метода getObjects
	Type* getObjectsReturnType = new Type(VOID_TYPE);
	vector<Type*>* getObjectsKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSObject", 1024) };
	vector<Type*>* getObjectsParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "getObjectsDynamic", "([Lrtl/NSObject;)V", false, NULL, getObjectsReturnType, getObjectsParamsType, getObjectsKeywordsType);

	//Добавление метода indexOfObject
	Type* indexOfObjectReturnType = new Type(INT_TYPE);
	vector<Type*>* indexOfObjectKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSObject") };
	vector<Type*>* indexOfObjectParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "indexOfObjectDynamic", "(Lrtl/NSObject;)I", false, NULL, indexOfObjectReturnType, indexOfObjectParamsType, indexOfObjectKeywordsType);

	//Добавление метода init
	Type* initReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSArray");
	vector<Type*>* initKeywordsType = new vector<Type*>;
	vector<Type*>* initParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "initDynamic", "()Lrtl/NSArray;", false, NULL, initReturnType, initParamsType, initKeywordsType);

	//Добавление метода isEqualToArray
	Type* isEqualToArrayReturnType = new Type(INT_TYPE);
	vector<Type*>* isEqualToArrayKeywordsType = new vector<Type*>{ new Type(CLASS_NAME_TYPE, "rtl/NSArray") };
	vector<Type*>* isEqualToArrayParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "isEqualToArrayDynamic", "(Lrtl/NSArray;)I", false, NULL, isEqualToArrayReturnType, isEqualToArrayParamsType, isEqualToArrayKeywordsType);

	//Добавление метода lastObject
	Type* lastObjectReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSObject");
	vector<Type*>* lastObjectKeywordsType = new vector<Type*>;
	vector<Type*>* lastObjectParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "lastObjectDynamic", "()Lrtl/NSObject;", false, NULL, lastObjectReturnType, lastObjectParamsType, lastObjectKeywordsType);

	//Добавление метода objectAtIndex
	Type* objectAtIndexReturnType = new Type(CLASS_NAME_TYPE, "rtl/NSObject");
	vector<Type*>* objectAtIndexKeywordsType = new vector<Type*>{ new Type(INT_TYPE) };
	vector<Type*>* objectAtIndexParamsType = new vector<Type*>;
	nsarray->Methods->addMethod(constantTable, "objectAtIndexDynamic", "(I)Lrtl/NSObject;", false, NULL, objectAtIndexReturnType, objectAtIndexParamsType, objectAtIndexKeywordsType);

	//Добавление FieldRef
	constantTable->findOrAddFieldRefConstant("rtl/NSArray", "array", "[Lrtl/NSObject");

	//Добавление MethodRef
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "<init>", "()V");
	constantTable->findOrAddMethodRefConstant("rtl/NSArray", "<init>", "()V");
	constantTable->findOrAddMethodRefConstant("rtl/NSArray", "<init>", "(Lrtl/NSArray;)V");
	constantTable->findOrAddMethodRefConstant("rtl/NSArray", "<init>", "([Lrtl/NSObject;)V");
	constantTable->findOrAddMethodRefConstant("rtl/NSString", "cStringDynamic", "()[C");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "<init>", "([C)V");
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "descriptionDynamic", "()Lrtl/NSString;");
	constantTable->findOrAddMethodRefConstant("java/lang/String", "toCharArray", "()[C");
	constantTable->findOrAddMethodRefConstant("rtl/NSString", "stringWithCStringStatic", "([C)Lrtl/NSString;");
	constantTable->findOrAddMethodRefConstant("rtl/NSArray", "containsObjectDynamic", "(Lrtl/NSObject;)I");
	constantTable->findOrAddMethodRefConstant("java/lang/System", "arraycopy", "(Ljava/lang/Object;ILjava/lang/Object;II)V");
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "initDynamic", "()Lrtl/NSObject;");
	constantTable->findOrAddMethodRefConstant("rtl/NSObject", "isEqualDynamic", "(Lrtl/NSObject;)I");
	constantTable->findOrAddMethodRefConstant("rtl/NSArray", "initDynamic", "()Lrtl/NSArray;");

	//Добавление класса NSArray в таблицу классов
	(*items)["rtl/NSArray"] = nsarray;
}

void ClassesTable::fillFieldRefs()
{
	auto iter = items->cbegin();
	while (iter != items->cend())
	{
		iter->second->fillFieldRefs();
		++iter;
	}
}

void ClassesTable::fillMethodRefs()
{
	auto iter = items->cbegin();
	while (iter != items->cend())
	{
		iter->second->fillMethodRefs();
		++iter;
	}
}

void ClassesTable::fillLiterals()
{
	auto iter = items->cbegin();
	while (iter != items->cend())
	{
		iter->second->fillLiterals();
		++iter;
	}
}

string ClassesTable::getFullClassName(string name)
{
	if (name.find("global/") != string::npos)
		return name;
	if (name.find("rtl/") != string::npos)
		return name;
	string fullName;
	if (name == "NSString" || name == "NSArray" || name == "NSObject" || name == "InOutFuncs")
		fullName = "rtl/" + name;
	else
		fullName = "global/" + name;
	if (items->count(fullName) == 0) {
		string msg = "Class '" + name + "' not found";
		throw new std::exception(msg.c_str());
	}
	return fullName;
}

void ClassesTable::semanticTransform()
{
	auto iter = items->cbegin();
	while (iter != items->cend())
	{
		iter->second->semanticTransform();
		++iter;
	}
}
