#include "tables.h"
#include <algorithm>
#include <string>
// ------------------------------ Обход дерева -------------------------------

void getTypesFromInitDeclaratorType(vector<Init_declarator_node*>* declarators, Type_node* typeNode, vector<string>* varsNames, vector<Type*>* varsTypes);

// ---------- Function_and_class_list_node ----------

void Function_and_class_list_node::fillTables()
{
	ClassesTable::initRTL(); //Инициализировать RTL

    for (int i = 0; i < FunctionsAndClasses->size(); i++)
    { //Для каждого элемента в списке
        if (FunctionsAndClasses->at(i).class_block != NULL)
        { //Если элемент - класс
            Class_block_node* cur = FunctionsAndClasses->at(i).class_block;
            if (cur->type == IMPLEMENTATION_CLASS_BLOCK_TYPE)
            { // Реализация
                Class_implementation_node* curImplementation = (Class_implementation_node*)cur;
                string className = curImplementation->ClassName; // Имя класса
                string *superclassName = curImplementation->SuperclassName == NULL ? NULL : new string(curImplementation->SuperclassName); // Имя суперкласса
				ClassesTableElement *element = ClassesTable::addClass(className, superclassName, true, curImplementation); // Добавление класса в таблицу

				if (curImplementation->Body != NULL)
				{
					// Добавление переменных
					map<string, Expression_node*> initializers;
					map<string, Type*> vars = curImplementation->Body->getVariables(&initializers);
					for (auto it = vars.begin(); it != vars.end(); it++) {
						if (element->Fields->items.count(it->first) && element->Fields->items[it->first]->InitialValue != NULL && initializers[it->first] != NULL) {
							string msg = "Variable '" + it->first + "' redifinition in class '" + className + "'\n";
							throw new std::exception(msg.c_str());
						}
						if (element->Fields->items.count(it->first) && (element->Fields->items[it->first]->DescriptorStr != it->second->getDescriptor() || !element->Fields->items[it->first]->type->equal(it->second))) {
							string msg = "Variable '" + it->first + "' in class '" + className + "' has conflict types.\n";
							throw new std::exception(msg.c_str());
						}
						if (!element->Fields->items.count(it->first))
							element->Fields->addField(element->ConstantTable, it->first, it->second->getDescriptor(), false, it->second, initializers[it->first]);
					}

					
					vector<Type*> varTypes;
					vector<string> instanceVariables;
					if (curImplementation->Body->Variables != NULL)
						instanceVariables = curImplementation->Body->getInstanceVariables(&varTypes);

					if (element->IsHaveInterface) { // У класса был интерфейс
						// Проверить instance variables
						if (instanceVariables.size() > 0) {
							map<string, FieldsTableElement*> instVar;
							for (auto item : element->Fields->items)
							{
								if (item.second->IsInstance)
									instVar[item.first] = item.second;
							}

							if (instanceVariables.size() != instVar.size())
							{
								string msg = "Instance variables have different size in interface and implementation in class'" + className + "'\n";
								throw new std::exception(msg.c_str());
							}
							// Сверить instance variables
							for (int i = 0; i < instanceVariables.size(); i++)
							{
								if (!instVar.count(instanceVariables[i])) {
									string msg = "Instance variable '" + instanceVariables[i] + "' not found in interface '" + className + "'\n";
									throw new std::exception(msg.c_str());
								}

								if (instVar[instanceVariables[i]]->InstanceIndex != i+1)
								{
									string msg = "Instance variable '" + instanceVariables[i] + "' in class '" + className + "' has different position from the position specified in the interface'\n";
									throw new std::exception(msg.c_str());
								}
								if (varTypes[i]->getDescriptor() != instVar[instanceVariables[i]]->DescriptorStr)
								{
									string msg = "Instance variable '" + instanceVariables[i] + "' in class '" + className + "' has different type from the type specified in the interface'\n";
									throw new std::exception(msg.c_str());
								}
							}
						}

						// Добавление методов
						map<string, vector<string>*> keywordsNames; //Словарь для имен keywords
						map<string, vector<Type*>*> keywordsTypes; //Словарь для типов keywords
						map<string, vector<string>*> parametersNames; //Словарь для имен параметров
						map<string, vector<Type*>*> parametersTypes; //Словарь для типов параметров
						map<string, bool> isClassMethod; //Словарь, содержащий принадлежность метода к классу
						map<string, Statement_node*> startNodes; //Словарь, содержащий стартовые узлы тел методов
						map<string, Type*> methods = curImplementation->Body->getMethods(&keywordsNames, &keywordsTypes, &parametersNames, &parametersTypes, &isClassMethod, &startNodes); //Получение методов
						for (auto it = methods.begin(); it != methods.end(); it++)
						{
							// Формирование дескриптора
							string descriptor = "(";
							for (int i = 0; i < keywordsTypes[it->first]->size(); i++) {
								descriptor += keywordsTypes[it->first]->at(i)->getDescriptor();
							}
							descriptor += ")";
							descriptor += it->second->getDescriptor();
							if (element->Methods->items.count(it->first))
							{ //Метод есть. Проверка.
								MethodsTableElement* curMethod = element->Methods->items[it->first];
								if (descriptor != curMethod->DescriptorStr) {
									string msg = "Method '" + it->first + "' in class '" + className + "' has different type from the type specified in the interface\n";
									throw new std::exception(msg.c_str());
								}

								curMethod->BodyStart = startNodes[it->first]; //Добавление тела
								if (curMethod->BodyStart != NULL) {
									//Получение и добавление локальных переменных внутри метода
									vector<string> varsNames;
									vector<Type*> varsTypes;
									curMethod->BodyStart->findLocalVariables(&varsNames, &varsTypes, element, *keywordsNames[it->first], *parametersNames[it->first]);
									for (int i = 0; i < varsNames.size(); i++)
									{
										curMethod->LocalVariables->findOrAddLocalVariable(varsNames[i], varsTypes[i]);
									}
								}
							}
							else
							{ //Метода нет. Добавление.
								bool isClass = isClassMethod[it->first];// Признак принадлежности к классу
								vector<Type*>* curParametersTypes = parametersTypes[it->first]; // Типы параметров
								vector<Type*>* curKeywordsTypes = keywordsTypes[it->first]; // Типы keywords
								Statement_node* curStart = startNodes[it->first]; // Узел начала тела метода

								MethodsTableElement *method = element->Methods->addMethod(element->ConstantTable, it->first, descriptor, isClass, curStart, it->second, curParametersTypes, curKeywordsTypes); //Добавление метода
								
								//Формирование таблицы локальных переменных
								LocalVariablesTable* locals = method->LocalVariables; //Таблица локальных переменных данного метода
								Type* type = new Type(CLASS_NAME_TYPE, element->getClassName()); // Тип для self переменной
								locals->findOrAddLocalVariable("self", type); //Добавление self в таблицу локальных переменных
								//Добавление keywords в таблицу локальных переменных
								for (int i = 0; i < keywordsNames[it->first]->size(); i++) {
									locals->findOrAddLocalVariable(keywordsNames[it->first]->at(i), keywordsTypes[it->first]->at(i));
								}
								//Добавление parameters в таблицу локальных переменных
								for (int i = 0; i < parametersNames[it->first]->size(); i++) {
									locals->findOrAddLocalVariable(parametersNames[it->first]->at(i), parametersTypes[it->first]->at(i));
								}
								//Получение и добавление локальных переменных внутри метода
								vector<string> varsNames;
								vector<Type*> varsTypes;
								if (method->BodyStart != NULL)
									method->BodyStart->findLocalVariables(&varsNames, &varsTypes, element, *keywordsNames[it->first], *parametersNames[it->first]);
								for (int i = 0; i < varsNames.size(); i++)
								{
									locals->findOrAddLocalVariable(varsNames[i], varsTypes[i]);
								}
							}
						}
					}
					else { // У класса не было интерфейса
						// Добавить instance variables
						for (int i = 0; i < instanceVariables.size(); i++)
						{
							element->Fields->addField(element->ConstantTable, instanceVariables[i], varTypes[i]->getDescriptor(), true, varTypes[i], NULL);
						}

						// Добавление методов
						map<string, vector<string>*> keywordsNames; //Словарь для имен keywords
						map<string, vector<Type*>*> keywordsTypes; //Словарь для типов keywords
						map<string, vector<string>*> parametersNames; //Словарь для имен параметров
						map<string, vector<Type*>*> parametersTypes; //Словарь для типов параметров
						map<string, bool> isClassMethod; //Словарь, содержащий принадлежность метода к классу
						map<string, Statement_node*> startNodes; //Словарь, содержащий стартовые узлы тел методов
						map<string, Type*> methods = curImplementation->Body->getMethods(&keywordsNames, &keywordsTypes, &parametersNames, &parametersTypes, &isClassMethod, &startNodes); //Получение методов
						for (auto it = methods.begin(); it != methods.end(); it++)
						{
							// Формирование дескриптора
							string descriptor = "(";
							for (int i = 0; i < keywordsTypes[it->first]->size(); i++) {
								descriptor += keywordsTypes[it->first]->at(i)->getDescriptor();
							}
							descriptor += ")";
							descriptor += it->second->getDescriptor();

							bool isClass = isClassMethod[it->first];// Признак принадлежности к классу
							vector<Type*>* curParametersTypes = parametersTypes[it->first]; // Типы параметров
							vector<Type*>* curKeywordsTypes = keywordsTypes[it->first]; // Типы keywords
							Statement_node* curStart = startNodes[it->first]; // Узел начала тела метода

							MethodsTableElement *method = element->Methods->addMethod(element->ConstantTable, it->first, descriptor, isClass, curStart, it->second, curParametersTypes, curKeywordsTypes); //Добавление метода
							
							
							//Формирование таблицы локальных переменных
							LocalVariablesTable* locals = method->LocalVariables; //Таблица локальных переменных данного метода
							Type* type = new Type(CLASS_NAME_TYPE, element->getClassName()); // Тип для self переменной
							locals->findOrAddLocalVariable("self", type); //Добавление self в таблицу локальных переменных
							//Добавление keywords в таблицу локальных переменных
							for (int i = 0; i < keywordsNames[it->first]->size(); i++) {
								locals->findOrAddLocalVariable(keywordsNames[it->first]->at(i), keywordsTypes[it->first]->at(i));
							}
							//Добавление parameters в таблицу локальных переменных
							for (int i = 0; i < parametersNames[it->first]->size(); i++) {
								locals->findOrAddLocalVariable(parametersNames[it->first]->at(i), parametersTypes[it->first]->at(i));
							}
							//Получение и добавление локальных переменных внутри метода
							vector<string> varsNames;
							vector<Type*> varsTypes;
							if (method->BodyStart != NULL)
								method->BodyStart->findLocalVariables(&varsNames, &varsTypes, element, *keywordsNames[it->first], *parametersNames[it->first]);
							for (int i = 0; i < varsNames.size(); i++)
							{
								locals->findOrAddLocalVariable(varsNames[i], varsTypes[i]);
							}
						}
					}


				}
            }
            else if (cur->type == INTERFACE_CLASS_BLOCK_TYPE)
            { // Интерфейс
                Class_interface_node* curInterface = (Class_interface_node*)cur;
                string className = curInterface->ClassName; // Имя класса
				string* superclassName = curInterface->SuperclassName == NULL ? NULL : new string(curInterface->SuperclassName); // Имя суперкласса
                ClassesTableElement* element = ClassesTable::addClass(className, superclassName, false, curInterface); // Добавление класса в таблицу

				if (curInterface->Body != NULL) {
					// Добавление переменных
					map<string, Expression_node*> initializers;
					map<string, Type*> vars = curInterface->Body->getVariables(&initializers);
					for (auto it = vars.begin(); it != vars.end(); it++) {
						if (element->Fields->items.count(it->first) && element->Fields->items[it->first]->InitialValue != NULL && initializers[it->first] != NULL) {
							string msg = "Variable '" + it->first + "' redifinition in class '" + className + "'\n";
							throw new std::exception(msg.c_str());
						}
						if (element->Fields->items.count(it->first) && (element->Fields->items[it->first]->DescriptorStr != it->second->getDescriptor() || element->Fields->items[it->first]->type->ArrSize != it->second->ArrSize)) {
							string msg = "Variable '" + it->first + "' in class '" + className + "' has conflict types.\n";
							throw new std::exception(msg.c_str());
						}
						element->Fields->addField(element->ConstantTable, it->first, it->second->getDescriptor(), false, it->second, initializers[it->first]);
					}

					// Добавление instance variables
					vector<Type*> varTypes;
					vector<string> instanceVariables = curInterface->Body->getInstanceVariables(&varTypes);
					for (int i = 0; i < instanceVariables.size(); i++)
					{
						element->Fields->addField(element->ConstantTable, instanceVariables[i], varTypes[i]->getDescriptor(), true, varTypes[i], NULL);
					}

					// Добавление методов
					map<string, vector<string>*> keywordsNames; //Словарь для имен keywords
					map<string, vector<Type*>*> keywordsTypes; //Словарь для типов keywords
					map<string, vector<string>*> parametersNames; //Словарь для имен параметров
					map<string, vector<Type*>*> parametersTypes; //Словарь для типов параметров
					map<string, bool> isClassMethod; //Словарь, содержащий принадлежность метода к классу
					map<string, Type*> methods = curInterface->Body->getMethods(&keywordsNames, &keywordsTypes, &parametersNames, &parametersTypes, &isClassMethod); //Получение методов
					for (auto it = methods.begin(); it != methods.end(); it++)
					{
						// Формирование дескриптора
						string descriptor = "(";
						for (int i = 0; i < keywordsTypes[it->first]->size(); i++) {
							descriptor += keywordsTypes[it->first]->at(i)->getDescriptor();
						}
						descriptor += ")";
						descriptor += it->second->getDescriptor();

						bool isClass = isClassMethod[it->first];// Признак принадлежности к классу
						vector<Type*>* curParametersTypes = parametersTypes[it->first]; // Типы параметров
						vector<Type*>* curKeywordsTypes = keywordsTypes[it->first]; // Типы keywords

						MethodsTableElement *method = element->Methods->addMethod(element->ConstantTable, it->first, descriptor, isClass, NULL, it->second, curParametersTypes, curKeywordsTypes); //Добавление метода
					

						//Формирование таблицы локальных переменных
						LocalVariablesTable* locals = method->LocalVariables; //Таблица локальных переменных данного метода
						Type* type = new Type(CLASS_NAME_TYPE, element->getClassName()); // Тип для self переменной
						locals->findOrAddLocalVariable("self", type); //Добавление self в таблицу локальных переменных
						//Добавление keywords в таблицу локальных переменных
						for (int i = 0; i < keywordsNames[it->first]->size(); i++) {
							locals->findOrAddLocalVariable(keywordsNames[it->first]->at(i), keywordsTypes[it->first]->at(i));
						}
						//Добавление parameters в таблицу локальных переменных
						for (int i = 0; i < parametersNames[it->first]->size(); i++) {
							locals->findOrAddLocalVariable(parametersNames[it->first]->at(i), parametersTypes[it->first]->at(i));
						}
					}

					// Добавление свойств
					map<string, bool> isReadonly; //Значение аттрибута readonly для property
					map<string, Type*> propertiesTypes = curInterface->Body->getProperties(&isReadonly);
					for (auto it = propertiesTypes.cbegin(); it != propertiesTypes.cend(); it++)
					{
						element->Properties->addProperty(element->ConstantTable, it->first, it->second->getDescriptor(), isReadonly[it->first], it->second);
					}
				}
            }
        }
		else if (FunctionsAndClasses->at(i).function != NULL)
		{
			Function_node* cur = FunctionsAndClasses->at(i).function; //Узел функции
			Type* returnType; //Возвращаемый тип
			Statement_node* body; //Узел начала тела функции
			string functionName = cur->getFunction(&returnType, &body); //Имя функции

			// Формирование дескриптора
			string descriptor = "()";
			descriptor += returnType->getDescriptor();

			vector<Type*> *params = new vector<Type*>;
			FunctionsTableElement* element = FunctionsTable::addFunction(functionName, descriptor, body, params, returnType); // Добавление функции в таблицу

			// Формирование таблицы локальных переменных
			LocalVariablesTable* locals = element->LocalVariables; //Таблица локальных переменных данной функции
			Type* type = new Type(CLASS_NAME_TYPE, "rtl/!Program!"); //Тип для this
			locals->findOrAddLocalVariable("self", type); //Добавление self в таблицу локальных переменных
			vector<string> varsNames;
			vector<Type*> varsTypes;
			ClassesTableElement* classElem = ClassesTable::items->at("rtl/!Program!");
			vector<string> kwNames;
			vector<string> paramNames;
			element->BodyStart->findLocalVariables(&varsNames, &varsTypes, classElem, kwNames, paramNames);
			for (int i = 0; i < varsNames.size(); i++)
			{
				locals->findOrAddLocalVariable(varsNames[i], varsTypes[i]);
			}
		}
    }

	ClassesTable::fillFieldRefs(); // Найти и заполнить field refs для классов
	FunctionsTable::fillFieldRefs(); //Найти и заполнить fields refs в функции

	ClassesTable::fillMethodRefs(); //Найти и заполнить method refs для классов
	FunctionsTable::fillMethodRefs(); //Найти и заполнить method refs в функции

	ClassesTable::fillLiterals(); // Найти и заполнить string и integer константы
	FunctionsTable::fillLiterals(); // Найти и заполнить string и integer константы



	FunctionsTable::convertToClassProgramMethods(); // Преобразовать функции в статические методы класса Program
	ClassesTable::semanticTransform(); //Преобразование дерева в классах

	ClassesTable::addConstantsToTable(); // Добавление констант типа Class с переменных
}

//---------- Program_node ----------

void Program_node::fillClassesTable()
{
    list->fillTables();
}

//---------- Interface_body_node ---------- 

vector<string> Interface_body_node::getInstanceVariables(vector<Type*>* varTypes)
{
	vector<string> res;
	if (Variables != NULL) {
		Instance_variables_declaration_node* first = Variables->First;
		while (first != NULL)
		{
			vector<Type*>* types = new vector<Type*>;
			vector <string> names = first->getInstanceVariables(types);
			for (int i = 0; i < names.size(); i++)
			{
				if (std::find(res.begin(), res.end(), names[i]) != res.end())
				{
					string msg = "Variable '" + names[i] + "' redeclaration";
					throw new std::exception(msg.c_str());
				}
				res.push_back(names[i]);
				varTypes->push_back(types->at(i));
			}
			first = first->Next;
		}
	}
	return res;
}

map<string, Type*> Interface_body_node::getVariables(map<string, Expression_node*>* initializers)
{
	vector<Interface_declaration_list_node::interface_declaration>* declarations = Declaration_list->Declarations; //Список объявлений
	map<string, Type*> res;
	for (auto it = declarations->cbegin(); it < declarations->cend(); it++)
	{
		Declaration_node* declaration = it->declaration;
		if (declaration != NULL)
		{
			 map<string,Type*> cur = declaration->getDeclaration(initializers);
			 for (auto iterator = cur.begin(); iterator != cur.end(); iterator++)
			 {
				 res[iterator->first] = iterator->second;
			 }
		}
	}
	return res;
}

map<string, Type*> Interface_body_node::getMethods(map<string, vector<string>*>* keywordsNames, map<string, vector<Type*>*> *keywordsTypes, map<string, vector<string>*>* parametersNames, map<string, vector<Type*>*> *parametersTypes, map<string, bool>* isClassMethod)
{
	map<string, Type*> res;
	if (Declaration_list != NULL)
	{
		vector<Interface_declaration_list_node::interface_declaration>* declarations = Declaration_list->Declarations; //Список объявлений
		
		for (auto it = declarations->cbegin(); it < declarations->cend(); it++)
		{
			Method_declaration_node* declaration = it->method_declaration;
			if (declaration != NULL)
			{
				vector<string>* curKeywordsNames = new vector<string>;
				vector<Type*>* curKeywordsTypes = new vector<Type*>;
				vector<string>* curParametersNames = new vector<string>;
				vector<Type*>* curParametersTypes = new vector<Type*>;
				bool isClass;
				Type* returnType = new Type(VOID_TYPE);
				string methodName = declaration->getMethod(&returnType, curKeywordsNames, curKeywordsTypes, curParametersNames, curParametersTypes, &isClass);
				if (res.count(methodName))
				{
					string msg = "Method '" + methodName + "' redeclaration.\n";
					throw new std::exception(msg.c_str());
				}
				(*keywordsNames)[methodName] = curKeywordsNames;
				(*keywordsTypes)[methodName] = curKeywordsTypes;
				(*parametersNames)[methodName] = curParametersNames;
				(*parametersTypes)[methodName] = curParametersTypes;
				(*isClassMethod)[methodName] = isClass;
				res[methodName] = returnType;
			}
		}
	}
	return res;
}

map<string, Type*> Interface_body_node::getProperties(map<string, bool>* isReadonly)
{
	map<string, Type*> res;
	if (Declaration_list != NULL)
	{
		vector<Interface_declaration_list_node::interface_declaration>* declarations = Declaration_list->Declarations; // Список объявлений
		for (auto it = declarations->cbegin(); it < declarations->cend(); it++)
		{
			Property_node* property = it -> property; //Свойство
			if (property != NULL)
			{
				Type* type = property->type->toDataType(); //Тип
				for (char* name : *property->Names->Identifier_names)
				{//Для каждого имени
					if (res.count(string(name)))
					{ // Переопределение свойства
						string msg = "Property '" + string(name) + "' redeclaration.";
						throw new std::exception(msg.c_str());
					}
					res[string(name)] = type; //Добавить тип
					(*isReadonly)[string(name)] = property->Attribute->type == READONLY_ATTRIBUTE_TYPE; //Добавить атрибут
				}
			}
		}
	}
	return res;
}

//---------- Instance_variables_declaration_node ----------

vector<string> Instance_variables_declaration_node::getInstanceVariables(vector<Type*>* types)
{
    vector<string> names; //Вектор имен полей
    vector<Declarator_node*> variablesDeclarators = *DeclaratorList->Declarators; //Вектор деклараторов
    for (auto it = variablesDeclarators.begin(); it != variablesDeclarators.end(); it++)
    {
        names.push_back((*it)->Identifier); //Добавление имени переменной
        type_type type = this->type->type; //Получение datatype
        Expression_node* arrSize = (*it)->Expression; //Размер массива
        if (type == CLASS_NAME_TYPE)
        {
			string className = ClassesTable::getFullClassName(string(this->type->ClassName)); //Имя класса
			strcpy(this->type->ClassName, className.c_str()); //Преобразование имени класса в узле
			if (arrSize != NULL)
			{ // тип класса и массив
				Type* curType = new Type(type, className, arrSize);
				types->push_back(curType);
			}
			else
			{ // тип класса 
				Type* curType = new Type(type, className);
				types->push_back(curType);
			}
        }
        else
        {
            if (arrSize != NULL)
			{ // Примитивный тип и массив
				Type* curType = new Type(type, arrSize);
				types->push_back(curType);
            }
            else
			{ // Примитивный тип 
				Type* curType = new Type(type);
				types->push_back(curType);
            }
        }
    }
	return names;
}

// ---------- Implementation_body_node ----------
vector<string> Implementation_body_node::getInstanceVariables(vector<Type*> *varTypes)
{
	Instance_variables_declaration_node* first = Variables->First;
	vector<string> res;
	while (first != NULL)
	{
		vector<Type*>* types = new vector<Type*>;
		vector <string> names = first->getInstanceVariables(types);
		for (int i = 0; i < names.size(); i++)
		{
			if (std::find(res.begin(), res.end(), names[i]) != res.end())
			{
				string msg = "Variable '" + names[i] + "' redeclaration";
				throw new std::exception(msg.c_str());
			}
			res.push_back(names[i]);
			varTypes->push_back(types->at(i));
		}
		first = first->Next;
	}
	return res;
}

map<string, Type*> Implementation_body_node::getVariables(map<string, Expression_node*>* initializers)
{
	map<string, Type*> res;
	if (Declaration_list != NULL)
	{
		vector<Implementation_definition_list_node::implementation_definition>* definitions = Declaration_list->Definitions; //Список определений
		for (auto it = definitions->cbegin(); it < definitions->cend(); it++)
		{
			Declaration_node* declaration = it->declaration;
			if (declaration != NULL)
			{
				map<string, Type*> cur = declaration->getDeclaration(initializers);
				for (auto iterator = cur.begin(); iterator != cur.end(); iterator++)
				{
					res[iterator->first] = iterator->second;
				}
			}
		}
	}
	return res;
}

map<string, Type*> Implementation_body_node::getMethods(map<string, vector<string>*>* keywordsNames, map<string, vector<Type*>*>* keywordsTypes, map<string, vector<string>*>* parametersNames, map<string, vector<Type*>*>* parametersTypes, map<string, bool>* isClassMethod, map<string, Statement_node*> *bodyStartNode)
{
	map<string, Type*> res;
	if (Declaration_list != NULL)
	{
		vector<Implementation_definition_list_node::implementation_definition>* definitions = Declaration_list->Definitions; //Список объявлений
		for (auto it = definitions->cbegin(); it < definitions->cend(); it++)
		{
			Method_definition_node* definition = it->method_definition;
			if (definition != NULL)
			{
				vector<string>* curKeywordsNames = new vector<string>;
				vector<Type*>* curKeywordsTypes = new vector<Type*>;
				vector<string>* curParametersNames = new vector<string>;
				vector<Type*>* curParametersTypes = new vector<Type*>;
				bool isClass;
				Type* returnType = new Type(VOID_TYPE);
				Statement_node* bodyStart;
				string methodName = definition->getMethod(&returnType, curKeywordsNames, curKeywordsTypes, curParametersNames, curParametersTypes, &isClass, &bodyStart);
				if (res.count(methodName))
				{
					string msg = "Method '" + methodName + "' redifinition.\n";
					throw new std::exception(msg.c_str());
				}
				(*keywordsNames)[methodName] = curKeywordsNames;
				(*keywordsTypes)[methodName] = curKeywordsTypes;
				(*parametersNames)[methodName] = curParametersNames;
				(*parametersTypes)[methodName] = curParametersTypes;
				(*isClassMethod)[methodName] = isClass;
				(*bodyStartNode)[methodName] = bodyStart;
				res[methodName] = returnType;
			}
		}
	}
	return res;
}

// ---------- Declaration_node ----------

map<string, Type*> Declaration_node::getDeclaration(map<string, Expression_node*>* initializators)
{
	vector<Init_declarator_node*>* declarators = init_declarator_list->getElements(); //Деклараторы
	map<string, Type*> res;
	for (auto it = declarators->begin(); it < declarators->end(); it++)
	{
		string name = string((*it)->Declarator); //Имя
		type_type type = this->typeNode->type; //Тип
		Expression_node* arrSize = (*it)->ArraySize; //Размер массива
		Expression_node* initializer = (*it)->expression; // Инициализатор
		Expression_list_node* initializerList = (*it)->InitializerList; // Инициализатор массива
		bool isArr = (*it)->isArray;
		if (type == CLASS_NAME_TYPE)
		{
			string className = ClassesTable::getFullClassName(string(this->typeNode->ClassName));
			strcpy(this->typeNode->ClassName, className.c_str()); //Преобразование имени класса в узле дерева
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
				
				res[name] = curType;
				if (initializators->count(name) && (*initializators)[name] != NULL && initializerList != NULL) {
					string msg = "Variable '" + name + "' redifinition in line: " + to_string(line);
					throw new std::exception(msg.c_str());
				}
				(*initializators)[name] = initializerList;
			}
			else
			{ //Тип класса
				Type* curType = new Type(type, className);
				res[name] = curType;
				if (initializators->count(name) && (*initializators)[name] != NULL && initializer != NULL) {
					string msg = "Variable '" + name + "' redifinition in line: " + to_string(line);
					throw new std::exception(msg.c_str());
				}
				(*initializators)[name] = initializer;
			}
		}
		else
		{ 
			if (isArr)
			{ // Массив примитивного типа
				int arraySize;
				Type* curType;
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
					else if (initializerList != NULL) {
						arraySize = initializerList->getElements()->size();
					}
					curType = new Type(type, arraySize);
				}
				
				res[name] = curType;
				if (initializators->count(name) && (*initializators)[name] != NULL && initializerList != NULL) {
					string msg = "Variable '" + name + "' redifinition in line: " + to_string(line);
					throw new std::exception(msg.c_str());
				}
				(*initializators)[name] = initializerList;
			}
			else
			{ //Примитивный тип
				Type* curType = new Type(type);
				res[name] = curType;
				if (initializators->count(name) && (*initializators)[name] != NULL && initializer != NULL) {
					string msg = "Variable '" + name + "' redifinition in line: " + to_string(line);
					throw new std::exception(msg.c_str());
				}
				(*initializators)[name] = initializer;
			}
		}
	}
	return res;
}

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

